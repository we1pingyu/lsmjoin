#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "boost/unordered_map.hpp"
#include "exp_config.hpp"
#include "exp_context.hpp"
#include "exp_utils.hpp"
#include "index.hpp"
#include "rocksdb/compaction_filter.h"
#include "rocksdb/db.h"
#include "rocksdb/filter_policy.h"
#include "rocksdb/iostats_context.h"
#include "rocksdb/merge_operator.h"
#include "rocksdb/perf_context.h"
#include "rocksdb/slice_transform.h"
#include "rocksdb/statistics.h"
#include "rocksdb/table.h"
using namespace std;

namespace HASHJOIN {
// Hash Join
struct MinHeapNode {
  string secondary_key;
  string primary_key;
};

int BKDRhash2(const std::string& str, int buckets) {
  unsigned int hash = 0;
  unsigned int seed = 131;

  for (char c : str) {
    hash = hash * seed + c;
  }

  return hash % buckets;
}

struct CustomHash {
  std::size_t operator()(const string& key) const {
    unsigned int hash = 0;
    unsigned int seed = 131;
    for (char c : key) {
      hash = hash * seed + c;
    }
    return hash;
  }
};

uint64_t probing(int num_buckets, string prefix_r, string prefix_s,
                 RunResult& run_result) {
  uint64_t matches = 0;
  double hash_time = 0.0, cpu_time = 0.0;
  uint64_t line_count = 0;
  Timer timer1 = Timer(), timer2 = Timer(), hash_timer = Timer();
  for (int i = 0; i < num_buckets; i++) {
    ifstream r_in;
    char* buf_r = new char[4096];
    r_in.rdbuf()->pubsetbuf(buf_r, 4096);
    r_in.open(prefix_r + "_" + to_string(i));
    // multimap<string, string> arr;
    boost::unordered_multimap<string, string*, CustomHash> arr;
    string line;
    timer1 = Timer();
    while (getline(r_in, line)) {
      line_count++;
      std::istringstream iss(line);
      std::string first, second;
      if (getline(iss, first, ',') && getline(iss, second)) {
        timer2 = Timer();
        string* ptr = new string(second);
        arr.emplace(first, ptr);
        cpu_time += timer2.elapsed();
      }
    }
    hash_time += timer1.elapsed() - cpu_time;
    ifstream s_in;
    char* buf_s = new char[4096];
    s_in.rdbuf()->pubsetbuf(buf_s, 4096);
    s_in.open(prefix_s + "_" + to_string(i));
    cpu_time = 0.0;
    timer1 = Timer();
    while (getline(s_in, line)) {
      std::istringstream iss(line);
      std::string first, second;
      if (getline(iss, first, ',') && getline(iss, second)) {
        timer2 = Timer();
        matches += arr.count(first);
        cpu_time += timer2.elapsed();
      }
    }
    hash_time = hash_time + timer1.elapsed() - cpu_time;

    r_in.close();
    s_in.close();
    delete[] buf_r;
    delete[] buf_s;
    // arr.~multimap();
  }
  run_result.hash_io_time += hash_time;
  run_result.hash_cpu_time += hash_timer.elapsed() - hash_time;
  return matches;
}

void partitioning(DB* db, string prefix, int num_buckets, int VALUE_SIZE,
                  int SECONDARY_SIZE, IndexType index_type,
                  RunResult& run_result) {
  ofstream* out = new ofstream[num_buckets];
  string fileName;
  double hash_time = 0.0, data_time = 0.0;
  Timer timer = Timer();
  Timer hash_timer = Timer();
  for (int i = 0; i < num_buckets; i++) {
    fileName = prefix + "_" + to_string(i);
    out[i].open(fileName);
  }
  string secondary_key, primary_key;
  rocksdb::Iterator* it = db->NewIterator(ReadOptions());
  int count = 0;
  if (index_type == IndexType::Primary) {
    for (it->SeekToFirst(); it->Valid();) {
      // switch secondary_key and primary_key
      secondary_key = it->key().ToString();
      std::string value_str = it->value().ToString();
      primary_key =
          value_str.substr(0, SECONDARY_SIZE) +
          value_str.substr(SECONDARY_SIZE, VALUE_SIZE - SECONDARY_SIZE);
      int hash = BKDRhash2(secondary_key, num_buckets);
      timer = Timer();
      out[hash] << secondary_key << "," << primary_key << "\n";
      hash_time += timer.elapsed();
      timer = Timer();
      it->Next();
      if (!it->Valid()) {
        break;
      }
      data_time += timer.elapsed();
    }
  } else {
    for (it->SeekToFirst(); it->Valid();) {
      std::string value_str = it->value().ToString();
      secondary_key = value_str.substr(0, SECONDARY_SIZE);
      primary_key =
          it->key().ToString() +
          value_str.substr(SECONDARY_SIZE, VALUE_SIZE - SECONDARY_SIZE);
      int hash = BKDRhash2(secondary_key, num_buckets);
      timer = Timer();
      out[hash] << secondary_key << "," << primary_key << "\n";
      hash_time += timer.elapsed();
      timer = Timer();
      it->Next();
      if (!it->Valid()) {
        break;
      }
      data_time += timer.elapsed();
    }
  }
  timer = Timer();
  for (int i = 0; i < num_buckets; i++) out[i].close();
  hash_time += timer.elapsed();
  run_result.hash_cpu_time += hash_timer.elapsed() - data_time - hash_time;
  run_result.get_data_time += data_time;
  run_result.hash_io_time += hash_time;
  delete it;
  delete[] out;
}

void HashJoin(ExpConfig& config, ExpContext& context, RunResult& run_result) {
  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE, VALUE_SIZE = config.VALUE_SIZE;

  cout << "hash index" << endl;

  cout << "Serializing data" << endl;
  Timer timer1 = Timer();
  int buckets_size =
      int((config.M - 3 * 4096) / (PRIMARY_SIZE + VALUE_SIZE) / 2) - 1;
  int num_buckets = min(
      int(config.r_tuples * (config.this_loop + 1) / buckets_size + 1), 500);
  cout << "num_buckets: " << num_buckets << endl;
  rocksdb::get_perf_context()->Reset();
  partitioning(context.db_r, config.db_r + "_hj", num_buckets, VALUE_SIZE,
               SECONDARY_SIZE, config.r_index, run_result);
  partitioning(context.db_s, config.db_s + "_hj", num_buckets, VALUE_SIZE,
               SECONDARY_SIZE, config.s_index, run_result);

  run_result.partition_time = timer1.elapsed();

  run_result.matches = probing(num_buckets, config.db_r + "_hj",
                               config.db_s + "_hj", run_result);

  return;
}
}  // namespace HASHJOIN