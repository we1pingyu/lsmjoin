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

namespace HYBRIDHASHJOIN {

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

int BKDRhash2(const std::string& str, int buckets) {
  unsigned int hash = 0;
  unsigned int seed = 131;

  for (char c : str) {
    hash = hash * seed + c;
  }

  return hash % buckets;
}

uint64_t hybrid_probing(int num_buckets, string prefix_r, string prefix_s,
                        RunResult& run_result, int memory_budget) {
  uint64_t matches = 0;
  double hash_time = 0.0, cpu_time = 0.0;
  Timer timer1 = Timer(), timer2 = Timer(), hash_timer = Timer();

  // Calculate the number of in-memory buckets
  int in_memory_buckets = memory_budget / (sizeof(string) * 2);

  for (int i = 0; i < num_buckets; i++) {
    boost::unordered_multimap<string, string, CustomHash> arr;

    // Process in-memory bucket
    if (i < in_memory_buckets) {
      ifstream r_in(prefix_r + "_" + to_string(i));
      string line;
      timer1 = Timer();
      while (getline(r_in, line)) {
        std::istringstream iss(line);
        std::string first, second;
        if (getline(iss, first, ',') && getline(iss, second)) {
          timer2 = Timer();
          arr.emplace(first, second);
          cpu_time += timer2.elapsed();
        }
      }
      hash_time += timer1.elapsed() - cpu_time;
      r_in.close();

      ifstream s_in(prefix_s + "_" + to_string(i));
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
      s_in.close();
    }
    // Process disk-based bucket
    else {
      ifstream r_in(prefix_r + "_" + to_string(i));
      ifstream s_in(prefix_s + "_" + to_string(i));
      string r_line, s_line;
      timer1 = Timer();
      while (getline(r_in, r_line)) {
        std::istringstream r_iss(r_line);
        std::string r_first, r_second;
        if (getline(r_iss, r_first, ',') && getline(r_iss, r_second)) {
          s_in.clear();
          s_in.seekg(0);
          while (getline(s_in, s_line)) {
            std::istringstream s_iss(s_line);
            std::string s_first, s_second;
            if (getline(s_iss, s_first, ',') && getline(s_iss, s_second)) {
              timer2 = Timer();
              if (r_first == s_first) {
                matches++;
              }
              cpu_time += timer2.elapsed();
            }
          }
        }
      }
      hash_time += timer1.elapsed() - cpu_time;
      r_in.close();
      s_in.close();
    }
  }
  run_result.hash_io_time += hash_time;
  run_result.hash_cpu_time += hash_timer.elapsed() - hash_time;
  return matches;
}

void hybrid_partitioning(DB* db, string prefix, int num_buckets, int VALUE_SIZE,
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
  if (index_type == IndexType::Primary) {
    for (it->SeekToFirst(); it->Valid();) {
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

void HybridHashJoin(ExpConfig& config, ExpContext& context,
                    RunResult& run_result) {
  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE, VALUE_SIZE = config.VALUE_SIZE;

  cout << "hybrid hash join" << endl;

  cout << "Serializing data" << endl;
  Timer timer1 = Timer();
  int buckets_size =
      int((config.M - 3 * 4096) / (PRIMARY_SIZE + VALUE_SIZE) / 2) - 1;
  int num_buckets = min(
      int(config.r_tuples * (config.this_loop + 1) / buckets_size + 1), 500);
  cout << "num_buckets: " << num_buckets << endl;
  rocksdb::get_perf_context()->Reset();
  hybrid_partitioning(context.db_r, config.db_r + "_hj", num_buckets,
                      VALUE_SIZE, SECONDARY_SIZE, config.r_index, run_result);
  hybrid_partitioning(context.db_s, config.db_s + "_hj", num_buckets,
                      VALUE_SIZE, SECONDARY_SIZE, config.s_index, run_result);

  run_result.partition_time = timer1.elapsed();

  // Calculate memory budget for in-memory processing
  int memory_budget = config.M - 3 * 4096;

  run_result.matches =
      hybrid_probing(num_buckets, config.db_r + "_hj", config.db_s + "_hj",
                     run_result, memory_budget);

  return;
}
}  // namespace HYBRIDHASHJOIN