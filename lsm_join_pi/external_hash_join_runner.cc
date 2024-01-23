// C++ program to implement
// external sorting using
// merge sort
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "boost/unordered_map.hpp"
#include "exp_utils.hpp"
#include "expconfig.hpp"
#include "expcontext.hpp"
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

uint64_t probing(int num_buckets, string prefix_r, string prefix_s) {
  uint64_t matches = 0;
  struct timespec t1, t2, t3, t4;
  double time = 0.0, time2 = 0.0, time3 = 0.0, time4 = 0.0;
  uint64_t line_count = 0;
  for (int i = 0; i < num_buckets; i++) {
    ifstream r_in;
    char* buf_r = new char[4096];
    r_in.rdbuf()->pubsetbuf(buf_r, 4096);
    r_in.open(prefix_r + "_" + to_string(i));
    // multimap<string, string> arr;
    boost::unordered_multimap<string, string*, CustomHash> arr;
    string line;
    clock_gettime(CLOCK_MONOTONIC, &t3);
    while (getline(r_in, line)) {
      line_count++;
      std::istringstream iss(line);
      std::string first, second;
      if (getline(iss, first, ',') && getline(iss, second)) {
        // continue;
        cout << first << " " << second << endl;
        string* ptr = new string(second);
        clock_gettime(CLOCK_MONOTONIC, &t1);
        arr.emplace(first, &second);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        time3 += ((t2.tv_sec - t1.tv_sec) +
                  (t2.tv_nsec - t1.tv_nsec) / 1000000000.0);
      }
    }
    clock_gettime(CLOCK_MONOTONIC, &t4);
    time +=
        ((t4.tv_sec - t3.tv_sec) + (t4.tv_nsec - t3.tv_nsec) / 1000000000.0);
    ifstream s_in;
    char* buf_s = new char[4096];
    s_in.rdbuf()->pubsetbuf(buf_s, 4096);
    s_in.open(prefix_s + "_" + to_string(i));
    // cout << "file open io time: " << time4 << endl;
    while (getline(s_in, line)) {
      std::istringstream iss(line);
      std::string first, second;
      if (getline(iss, first, ',') && getline(iss, second)) {
        clock_gettime(CLOCK_MONOTONIC, &t1);
        matches += arr.count(first);
        clock_gettime(CLOCK_MONOTONIC, &t2);
        time2 += ((t2.tv_sec - t1.tv_sec) +
                  (t2.tv_nsec - t1.tv_nsec) / 1000000000.0);
      }
    }

    r_in.close();
    s_in.close();
    delete[] buf_r;
    delete[] buf_s;
    // arr.~multimap();
  }
  cout << "file read io time: " << time << endl;
  cout << "line count: " << line_count << endl;
  cout << "multimap insert time: " << time3 << endl;
  cout << "multimap count time: " << time2 << endl;
  return matches;
}

void partitioning(DB* db, string prefix, int num_buckets, int VALUE_SIZE,
                  int SECONDARY_SIZE, bool is_S = false) {
  ofstream* out = new ofstream[num_buckets];
  string fileName;
  double hash_time = 0.0, time2 = 0.0;
  struct timespec t1, t2, t3, t4;
  for (int i = 0; i < num_buckets; i++) {
    fileName = prefix + "_" + to_string(i);
    out[i].open(fileName);
  }
  string secondary_key, primary_key;
  rocksdb::Iterator* it = db->NewIterator(ReadOptions());
  int count = 0;
  clock_gettime(CLOCK_MONOTONIC, &t1);
  if (is_S) {
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
      // switch secondary_key and primary_key
      secondary_key = it->key().ToString();
      primary_key = it->value().ToString().substr(0, SECONDARY_SIZE) +
                    it->value().ToString().substr(SECONDARY_SIZE,
                                                  VALUE_SIZE - SECONDARY_SIZE);
      int hash = BKDRhash2(secondary_key, num_buckets);
      out[hash] << secondary_key << "," << primary_key << "\n";
    }
  } else {
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
      secondary_key = it->value().ToString().substr(0, SECONDARY_SIZE);
      primary_key = it->key().ToString() +
                    it->value().ToString().substr(SECONDARY_SIZE,
                                                  VALUE_SIZE - SECONDARY_SIZE);
      // cout << SECONDARY_SIZE << endl;
      // cout << it->value().ToString() << " "
      //      << it->value().ToString().substr(0, SECONDARY_SIZE) << endl;
      // show secondary key and primary key
      clock_gettime(CLOCK_MONOTONIC, &t3);
      int hash = BKDRhash2(secondary_key, num_buckets);
      clock_gettime(CLOCK_MONOTONIC, &t4);
      out[hash] << secondary_key << "," << primary_key << "\n";
      time2 +=
          ((t4.tv_sec - t3.tv_sec) + (t4.tv_nsec - t3.tv_nsec) / 1000000000.0);
    }
  }

  clock_gettime(CLOCK_MONOTONIC, &t2);
  hash_time +=
      ((t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0);
  cout << "lsm read io time: " << hash_time << endl;
  cout << "hash func time: " << time2 << endl;
  for (int i = 0; i < num_buckets; i++) out[i].close();
  delete it;
  delete[] out;
}

tuple<int, double, double> HashJoin(ExpConfig& config, ExpContext& context) {
  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE, VALUE_SIZE = config.VALUE_SIZE;

  cout << "hash index" << endl;

  cout << "Serializing data" << endl;
  Timer timer1 = Timer();
  int num_buckets = min(int(config.M / 4096) - 1, 500);
  cout << "num_buckets: " << num_buckets << endl;
  rocksdb::get_perf_context()->Reset();
  // uint64_t matches = externalHash(db_r, db_s, "/tmp/r", "/tmp/s",
  // num_buckets);
  partitioning(context.db_r, "/tmp/r", num_buckets, VALUE_SIZE, SECONDARY_SIZE);
  partitioning(context.db_s, "/tmp/s", num_buckets, VALUE_SIZE, SECONDARY_SIZE,
               true);

  auto hash_time = timer1.elapsed();

  cout << "partition time: " << hash_time << endl;

  uint64_t matches = probing(num_buckets, "/tmp/s", "/tmp/r");

  auto hash_join_time = timer1.elapsed();

  return {matches, hash_time, hash_join_time};
}

int main(int argc, char* argv[]) {
  parseCommandLine(argc, argv);
  ExpConfig& config = ExpConfig::getInstance();
  ExpContext& context = ExpContext::getInstance();
  context.InitDB();

  uint64_t sum_join_read_io = 0;
  double sum_val_time = 0, sum_get_time = 0, sum_sort_merge_time = 0;

  for (int i = 0; i < config.num_loop; i++) {
    cout << "-------------------------" << endl;
    cout << "loop: " << i << endl;
    cout << "-------------------------" << endl;
    config.this_loop = i;
    vector<uint64_t> R, S, P;
    context.GenerateData(R, S, P);

    Timer timer1 = Timer();

    auto [matches, val_time, get_time] = HashJoin(config, context);

    uint64_t join_read_io = get_perf_context()->block_read_count;
    cout << "join read io: " << join_read_io << endl;
    cout << "matches: " << matches << endl;
    cout << "val_time: " << val_time << endl;
    cout << "get_time: " << get_time << endl;
    auto sort_merge_time = timer1.elapsed();
    cout << "sort_merge_time: " << sort_merge_time << endl;

    sum_join_read_io += join_read_io;
    sum_val_time += val_time;
    sum_get_time += get_time;
    sum_sort_merge_time += sort_merge_time;

    R.clear();
    S.clear();
    P.clear();
  }

  cout << "-------------------------" << endl;
  cout << "sum_join_read_io: " << sum_join_read_io << endl;
  cout << "sum_val_time: " << sum_val_time << endl;
  cout << "sum_get_time: " << sum_get_time << endl;
  cout << "sum_sort_merge_time: " << sum_sort_merge_time << endl;
  cout << "-------------------------" << endl;

  context.db_r->Close();
  context.db_s->Close();
  delete context.db_r;
  delete context.db_s;
}
