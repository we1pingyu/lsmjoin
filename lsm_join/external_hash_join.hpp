#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "boost/unordered_map.hpp"
#include "exp_config.hpp"
#include "exp_context.hpp"
#include "exp_utils.hpp"
#
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

uint64_t concurrent_probing(int num_buckets, string prefix_r, string prefix_s,
                            RunResult& run_result, int nthreads) {
  uint64_t matches = 0;
  double hash_time = 0.0, cpu_time = 0.0;
  uint64_t line_count = 0;
  Timer hash_timer;

  std::mutex mutex;
  int num_threads = nthreads;
  std::vector<std::thread> threads;

  for (int i = 0; i < num_buckets; i++) {
    // 读取并构建 r_in 的哈希表
    ifstream r_in;
    char* buf_r = new char[4096];
    r_in.rdbuf()->pubsetbuf(buf_r, 4096);
    r_in.open(prefix_r + "_" + to_string(i));

    boost::unordered_multimap<string, string*, CustomHash> arr;
    string line;
    double local_cpu_time = 0.0;
    double local_hash_time = 0.0;
    Timer timer1, timer2;

    timer1 = Timer();
    while (getline(r_in, line)) {
      line_count++;
      std::istringstream iss(line);
      std::string first, second;
      if (getline(iss, first, ',') && getline(iss, second)) {
        timer2 = Timer();
        string* ptr = new string(second);
        arr.emplace(first, ptr);
        local_cpu_time += timer2.elapsed();
      }
    }
    local_hash_time += timer1.elapsed() - local_cpu_time;
    r_in.close();
    delete[] buf_r;

    ifstream s_in;
    char* buf_s = new char[4096];
    s_in.rdbuf()->pubsetbuf(buf_s, 4096);
    s_in.open(prefix_s + "_" + to_string(i));

    local_cpu_time = 0.0;
    timer1 = Timer();

    std::vector<std::string> s_lines;
    while (getline(s_in, line)) {
      s_lines.push_back(line);  
    }
    s_in.close();
    delete[] buf_s;

    uint64_t local_matches = 0;
    std::atomic<uint64_t> thread_matches{0};

    auto thread_probe = [&](int start, int end) {
      Timer local_timer;
      for (int j = start; j < end; ++j) {
        std::istringstream iss(s_lines[j]);
        std::string first, second;
        if (getline(iss, first, ',') && getline(iss, second)) {
          local_timer = Timer();
          thread_matches += arr.count(first);  // probe arr
          local_cpu_time += local_timer.elapsed();
        }
      }
    };

    int chunk_size = s_lines.size() / num_threads;
    for (int t = 0; t < num_threads; t++) {
      int start = t * chunk_size;
      int end = (t == num_threads - 1) ? s_lines.size() : start + chunk_size;
      threads.emplace_back(thread_probe, start, end);
    }

    for (auto& t : threads) {
      if (t.joinable()) t.join();
    }
    threads.clear();

    local_matches = thread_matches.load();
    local_hash_time += timer1.elapsed() - local_cpu_time;

    {
      std::lock_guard<std::mutex> lock(mutex);
      matches += local_matches;
      hash_time += local_hash_time;
      cpu_time += local_cpu_time;
    }

    // 清理 r_in 的哈希表
    for (auto& pair : arr) {
      delete pair.second;
    }
  }

  run_result.hash_io_time += hash_time;
  run_result.hash_cpu_time += hash_timer.elapsed() - hash_time;
  return matches;
}

void concurrent_partitioning(DB* db, const std::string& prefix, int num_buckets,
                             int VALUE_SIZE, int SECONDARY_SIZE,
                             IndexType index_type, RunResult& run_result,
                             int nthreads) {
  double hash_time = 0.0, data_time = 0.0;
  Timer total_timer;

  // 打开输出文件
  std::vector<std::ofstream> out(num_buckets);
  for (int i = 0; i < num_buckets; i++) {
    out[i].open(prefix + "_" + std::to_string(i));
  }

  // 为每个输出文件创建一个互斥锁
  std::vector<std::mutex> out_mutexes(num_buckets);

  // 定义键空间的范围
  uint64_t min_key = 0;
  uint64_t max_key = 4000000000;  // 根据您的实际数据调整最大键值

  uint64_t total_keys = max_key - min_key + 1;
  int num_threads = nthreads;
  uint64_t keys_per_thread = (total_keys + num_threads - 1) / num_threads;

  std::atomic<double> atomic_hash_time(0.0);
  std::atomic<double> atomic_data_time(0.0);

  // 创建线程
  std::vector<std::thread> threads;
  for (int i = 0; i < num_threads; ++i) {
    uint64_t start_key = min_key + i * keys_per_thread;
    uint64_t end_key = std::min(start_key + keys_per_thread - 1, max_key);

    threads.emplace_back([&, start_key, end_key]() {
      rocksdb::Iterator* it = db->NewIterator(ReadOptions());

      // 将 start_key 转换为字符串
      std::string start_key_str = std::to_string(start_key);
      start_key_str =
          std::string(10 - start_key_str.length(), '0') + start_key_str;
      it->Seek(start_key_str);

      double local_hash_time = 0.0;
      double local_data_time = 0.0;
      Timer timer;

      while (it->Valid()) {
        std::string key_str = it->key().ToString();

        uint64_t current_key = std::stoull(key_str);

        if (current_key > end_key) {
          break;
        }

        std::string key, value;
        if (index_type == IndexType::Primary) {
          key = key_str;
          std::string value_str = it->value().ToString();
          value = value_str.substr(0, SECONDARY_SIZE) +
                  value_str.substr(SECONDARY_SIZE, VALUE_SIZE - SECONDARY_SIZE);
        } else {
          std::string value_str = it->value().ToString();
          key = value_str.substr(0, SECONDARY_SIZE);
          value = key_str +
                  value_str.substr(SECONDARY_SIZE, VALUE_SIZE - SECONDARY_SIZE);
        }

        timer = Timer();
        int hash = BKDRhash2(key, num_buckets);
        {
          std::lock_guard<std::mutex> lock(out_mutexes[hash]);
          out[hash] << key << "," << value << "\n";
        }
        local_hash_time += timer.elapsed();

        it->Next();
      }

      // atomic_hash_time += local_hash_time;
      // atomic_data_time += local_data_time;

      delete it;
    });
  }

  for (auto& t : threads) {
    if (t.joinable()) t.join();
  }

  for (int i = 0; i < num_buckets; i++) {
    out[i].close();
  }

  run_result.hash_cpu_time +=
      total_timer.elapsed() - atomic_data_time.load() - atomic_hash_time.load();
  run_result.get_data_time += atomic_data_time.load();
  run_result.hash_io_time += atomic_hash_time.load();
}

void ConcurrentHashJoin(ExpConfig& config, ExpContext& context,
                        RunResult& run_result) {
  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE, VALUE_SIZE = config.VALUE_SIZE;

  cout << "hash index" << endl;

  cout << "Serializing data" << endl;
  Timer timer1;
  int buckets_size =
      int((config.M - 3 * 4096) / (PRIMARY_SIZE + VALUE_SIZE) / 2) - 1;
  int num_buckets = min(
      int(config.r_tuples * (config.this_loop + 1) / buckets_size + 1), 500);
  cout << "num_buckets: " << num_buckets << endl;
  rocksdb::get_perf_context()->Reset();

  concurrent_partitioning(context.db_r, config.db_r + "_hj", num_buckets,
                          VALUE_SIZE, SECONDARY_SIZE, config.r_index,
                          run_result, config.concurrent_threads);
  concurrent_partitioning(context.db_s, config.db_s + "_hj", num_buckets,
                          VALUE_SIZE, SECONDARY_SIZE, config.s_index,
                          run_result, config.concurrent_threads);

  run_result.partition_time = timer1.elapsed();

  run_result.matches =
      concurrent_probing(num_buckets, config.db_r + "_hj", config.db_s + "_hj",
                         run_result, config.concurrent_threads);

  return;
}

void HashJoin(ExpConfig& config, ExpContext& context, RunResult& run_result) {
  if (config.concurrent_threads > 1) {
    ConcurrentHashJoin(config, context, run_result);
    return;
  }
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