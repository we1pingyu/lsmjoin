#pragma once

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <condition_variable>
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
#include "external_hash_join.hpp"
#include "external_hybrid_hash_join.hpp"
#include "index.hpp"
#include "merge.hpp"
#include "rocksdb/compaction_filter.h"
#include "rocksdb/db.h"
#include "rocksdb/filter_policy.h"
#include "rocksdb/iostats_context.h"
#include "rocksdb/merge_operator.h"
#include "rocksdb/perf_context.h"
#include "rocksdb/slice_transform.h"
#include "rocksdb/statistics.h"
#include "rocksdb/table.h"
#include "thread_pool.hpp"
using namespace std;

void HashJoin(ExpConfig& config, ExpContext& context, RunResult& run_result) {
  HASHJOIN::HashJoin(config, context, run_result);
}

void HybridHashJoin(ExpConfig& config, ExpContext& context,
                    RunResult& run_result) {
  HYBRIDHASHJOIN::HybridHashJoin(config, context, run_result);
}

void SortMergeForEagerLazy(ExpConfig& config, ExpContext& context,
                           RunResult& run_result, rocksdb::Iterator* it_r,
                           rocksdb::Iterator* it_s, int& matches) {
  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE;
  string temp_r_key, temp_r_value, temp_s_key, temp_s_value, value_r, value_s;
  std::vector<std::string> value_split;
  Status s;
  string tmp;
  int count1 = 0, count2 = 0;
  Timer timer;
  double data_time = 0.0, index_time = 0.0, post_time = 0.0;
  // double string_process_time = 0.0;
  while (it_r->Valid() && it_s->Valid()) {
    // Timer string_timer = Timer();
    temp_r_key = it_r->key().ToString();
    temp_s_key = it_s->key().ToString();
    // string_process_time += string_timer.elapsed();
    if (temp_r_key == temp_s_key) {
      // string_timer = Timer();
      temp_r_value = it_r->value().ToString();
      temp_s_value = it_s->value().ToString();
      // string_process_time += string_timer.elapsed();
      timer = Timer();
      value_split =
          boost::split(value_split, temp_r_value, boost::is_any_of(":"));
      post_time += timer.elapsed();
      if (IsCoveringIndex(config.r_index) || !IsIndex(config.r_index)) {
        count1 += value_split.size();
      } else {
        timer = Timer();
        std::unordered_set<string> unique_values(value_split.begin(),
                                                 value_split.end());
        post_time += timer.elapsed();

        for (auto x : unique_values) {
          timer = Timer();
          s = context.db_r->Get(ReadOptions(), x.substr(0, PRIMARY_SIZE), &tmp);
          data_time += timer.elapsed();

          // string_timer = Timer();
          bool is_match =
              s.ok() && tmp.compare(0, SECONDARY_SIZE, temp_r_key) == 0;
          // string_process_time += string_timer.elapsed();

          if (is_match) count1++;
        }
      }
      timer = Timer();
      value_split =
          boost::split(value_split, temp_s_value, boost::is_any_of(":"));
      post_time += timer.elapsed();
      if (IsCoveringIndex(config.s_index) || !IsIndex(config.s_index)) {
        count2 += value_split.size();
      } else {
        timer = Timer();
        std::unordered_set<string> unique_values(value_split.begin(),
                                                 value_split.end());
        post_time += timer.elapsed();

        for (auto x : unique_values) {
          timer = Timer();
          s = context.db_s->Get(ReadOptions(), x.substr(0, PRIMARY_SIZE), &tmp);
          data_time += timer.elapsed();

          // string_timer = Timer();
          bool is_match =
              s.ok() && tmp.compare(0, SECONDARY_SIZE, temp_s_key) == 0;
          // string_process_time += string_timer.elapsed();

          if (is_match) count2++;
        }
      }

      matches += count1 * count2;
      count1 = 0;
      count2 = 0;
      timer = Timer();
      it_r->Next();
      it_s->Next();
      index_time += timer.elapsed();
    } else if (temp_r_key < temp_s_key) {
      timer = Timer();
      it_r->Next();
      index_time += timer.elapsed();
    } else if (temp_r_key > temp_s_key) {
      timer = Timer();
      it_s->Next();
      index_time += timer.elapsed();
    }
  }
  run_result.get_data_time += data_time;
  run_result.get_index_time += index_time;
  run_result.post_list_time += post_time;
  // run_result.string_process_time += string_process_time;
}

void ConcurrentSortMergeForComp(ExpConfig& config, ExpContext& context,
                                rocksdb::Iterator* it_r,
                                rocksdb::Iterator* it_s, RunResult& run_result,
                                int& matches) {
  int num_threads = config.concurrent_threads;
  std::vector<std::thread> threads;
  std::mutex matches_mutex;

  // 假设键的最小值和最大值已知
  int64_t min_key = 0;
  int64_t max_key = 4000000000;  // 请根据实际情况设置 MAX_KEY

  int64_t range_size = (max_key - min_key + num_threads - 1) / num_threads;

  for (int i = 0; i < num_threads; ++i) {
    int64_t range_start = min_key + i * range_size;
    int64_t range_end = std::min(max_key, range_start + range_size - 1);

    threads.emplace_back([&, range_start, range_end]() {
      // 为每个线程创建新的迭代器
      ReadOptions read_options;
      read_options.auto_prefix_mode = true;
      rocksdb::Iterator* thread_it_r =
          context.ptr_index_r->NewIterator(read_options);
      rocksdb::Iterator* thread_it_s = context.db_s->NewIterator(ReadOptions());

      // 将迭代器定位到范围的起始位置
      std::string start_key_str = std::to_string(range_start);
      start_key_str = std::string(10 - start_key_str.length(), '0') + start_key_str;
      string secondary_key_lower =
          start_key_str + string(config.PRIMARY_SIZE, '0');
      // string_process_time += string_timer.elapsed();

      thread_it_r->Seek(start_key_str);
      thread_it_s->Seek(start_key_str);

      int thread_matches = 0;
      int PRIMARY_SIZE = config.PRIMARY_SIZE,
          SECONDARY_SIZE = config.SECONDARY_SIZE;
      string temp_r_key, temp_r_value, temp_s_key, temp_s_value, value_r,
          value_s;
      Status status;
      string tmp;
      int count1 = 0, count2 = 0;
      double data_time = 0.0, index_time = 0.0;
      Timer timer1 = Timer();
      while (thread_it_r->Valid() && thread_it_s->Valid()) {
        std::string r_key_str = thread_it_r->key().ToString();
        std::string s_key_str = thread_it_s->key().ToString();

        temp_r_key = r_key_str.substr(0, SECONDARY_SIZE);
        temp_s_key = s_key_str.substr(0, SECONDARY_SIZE);

        // 将键转换为整数进行比较
        int64_t r_key_int = std::stoll(temp_r_key);
        int64_t s_key_int = std::stoll(temp_s_key);

        if (r_key_int > range_end && s_key_int > range_end) {
          break;
        }

        if (temp_r_key == temp_s_key) {
          temp_r_value = r_key_str.substr(SECONDARY_SIZE, PRIMARY_SIZE);
          temp_s_value = s_key_str.substr(SECONDARY_SIZE, PRIMARY_SIZE);
          count1++;
          count2++;

          if (IsIndex(config.r_index)) {
            tmp = temp_r_key;
            while (thread_it_r->Valid()) {
              timer1 = Timer();
              thread_it_r->Next();
              index_time += timer1.elapsed();
              if (!thread_it_r->Valid()) break;
              r_key_str = thread_it_r->key().ToString();
              temp_r_key = r_key_str.substr(0, SECONDARY_SIZE);
              if (temp_r_key == tmp) {
                temp_r_value = r_key_str.substr(SECONDARY_SIZE, PRIMARY_SIZE);
                count1++;
              } else
                break;
            }
          }

          tmp = temp_s_key;
          while (thread_it_s->Valid()) {
            timer1 = Timer();
            thread_it_s->Next();
            index_time += timer1.elapsed();
            if (!thread_it_s->Valid()) break;
            s_key_str = thread_it_s->key().ToString();
            temp_s_key = s_key_str.substr(0, SECONDARY_SIZE);
            if (temp_s_key == tmp) {
              temp_s_value = s_key_str.substr(SECONDARY_SIZE, PRIMARY_SIZE);
              count2++;
            } else
              break;
          }

          thread_matches += count1 * count2;
          count1 = 0;
          count2 = 0;
        } else if (temp_r_key < temp_s_key) {
          timer1 = Timer();
          thread_it_r->Next();
          index_time += timer1.elapsed();
        } else if (temp_r_key > temp_s_key) {
          timer1 = Timer();
          thread_it_s->Next();
          index_time += timer1.elapsed();
        }
      }

      // 线程安全地更新匹配数和运行结果
      {
        std::lock_guard<std::mutex> lock(matches_mutex);
        matches += thread_matches;
        run_result.get_data_time += data_time;
        run_result.get_index_time += index_time;
      }

      delete thread_it_r;
      delete thread_it_s;
    });
  }

  // 等待所有线程完成
  for (auto& t : threads) {
    t.join();
  }
}

void SortMergeForComp(ExpConfig& config, ExpContext& context,
                      RunResult& run_result, rocksdb::Iterator* it_r,
                      rocksdb::Iterator* it_s, int& matches) {
  if (config.concurrent_threads > 1) {
    ConcurrentSortMergeForComp(config, context, it_r, it_s, run_result,
                               matches);
    return;
  }
  cout << "SortMergeForComp" << endl;
  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE;
  string temp_r_key, temp_r_value, temp_s_key, temp_s_value, value_r, value_s;
  std::vector<std::string> value_split;
  Status status;
  string tmp;
  int count1 = 0, count2 = 0;
  double data_time = 0.0, index_time = 0.0;
  // double string_process_time = 0.0;
  Timer timer1 = Timer();
  while (it_r->Valid() && it_s->Valid()) {
    // Timer string_timer = Timer();
    // Convert keys to strings once per iterator
    std::string r_key_str = it_r->key().ToString();
    std::string s_key_str = it_s->key().ToString();

    // Perform substr operations on the pre-converted strings
    temp_r_key = r_key_str.substr(0, SECONDARY_SIZE);
    temp_s_key = s_key_str.substr(0, SECONDARY_SIZE);

    // string_process_time += string_timer.elapsed();

    if (temp_r_key == temp_s_key) {
      // string_timer = Timer();
      temp_r_value = r_key_str.substr(SECONDARY_SIZE, PRIMARY_SIZE);
      temp_s_value = s_key_str.substr(SECONDARY_SIZE, PRIMARY_SIZE);
      // string_process_time += string_timer.elapsed();

      if (IsCoveringIndex(config.r_index) || !IsIndex(config.r_index)) {
        count1++;  // number of keys in R
      } else {
        timer1 = Timer();
        status = context.db_r->Get(ReadOptions(), temp_r_value, &value_r);
        data_time += timer1.elapsed();

        // string_timer = Timer();
        bool is_match =
            status.ok() && value_r.compare(0, SECONDARY_SIZE, temp_r_key) == 0;
        // string_process_time += string_timer.elapsed();

        if (is_match) count1++;  // number of keys in R
      }

      if (IsCoveringIndex(config.s_index) || !IsIndex(config.s_index)) {
        count2++;  // number of keys in S
      } else {
        // get s
        timer1 = Timer();
        status = context.db_s->Get(ReadOptions(), temp_s_value, &value_s);
        data_time += timer1.elapsed();

        // Timer string_timer = Timer();
        bool is_match =
            status.ok() && value_s.compare(0, SECONDARY_SIZE, temp_s_key) == 0;
        // string_process_time += string_timer.elapsed();

        if (is_match) count2++;  // number of keys in S
      }

      if (IsIndex(config.r_index)) {
        tmp = temp_r_key;
        while (it_r->Valid()) {
          timer1 = Timer();
          it_r->Next();
          index_time += timer1.elapsed();
          if (!it_r->Valid()) break;

          // string_timer = Timer();
          string r_key_str = it_r->key().ToString();

          temp_r_key = r_key_str.substr(0, SECONDARY_SIZE);

          // string_process_time += string_timer.elapsed();
          if (temp_r_key == tmp) {
            // string_timer = Timer();

            temp_r_value = r_key_str.substr(SECONDARY_SIZE, PRIMARY_SIZE);

            // string_process_time += string_timer.elapsed();

            if (IsCoveringIndex(config.r_index)) {
              count1++;
            } else {
              timer1 = Timer();
              status = context.db_r->Get(ReadOptions(), temp_r_value, &value_r);
              data_time += timer1.elapsed();

              // string_timer = Timer();
              bool is_match = status.ok() && value_r.compare(0, SECONDARY_SIZE,
                                                             temp_r_key) == 0;
              // string_process_time += string_timer.elapsed();

              if (is_match) count1++;
            }
          } else
            break;
        }
      }

      if (IsIndex(config.s_index)) {
        tmp = temp_s_key;
        while (it_s->Valid()) {
          timer1 = Timer();
          it_s->Next();
          index_time += timer1.elapsed();
          if (!it_s->Valid()) break;
          // string_timer = Timer();
          string s_key_str = it_s->key().ToString();
          temp_s_key = s_key_str.substr(0, SECONDARY_SIZE);
          // string_process_time += string_timer.elapsed();
          if (temp_s_key == tmp) {
            // string_timer = Timer();
            temp_s_value = s_key_str.substr(SECONDARY_SIZE, PRIMARY_SIZE);
            // string_process_time += string_timer.elapsed();
            if (IsCoveringIndex(config.s_index)) {
              count2++;
            } else {
              timer1 = Timer();
              status = context.db_s->Get(ReadOptions(), temp_s_value, &value_s);
              data_time += timer1.elapsed();

              // string_timer = Timer();
              bool is_match = status.ok() && value_s.compare(0, SECONDARY_SIZE,
                                                             temp_s_key) == 0;
              // string_process_time += string_timer.elapsed();

              if (is_match) count2++;
            }
          } else
            break;
        }
      }

      matches += count1 * count2;
      count1 = 0;
      count2 = 0;
    } else if (temp_r_key < temp_s_key) {
      timer1 = Timer();
      it_r->Next();
      index_time += timer1.elapsed();
    } else if (temp_r_key > temp_s_key) {
      timer1 = Timer();
      it_s->Next();
      index_time += timer1.elapsed();
    }
  }
  run_result.get_data_time += data_time;
  run_result.get_index_time += index_time;
  // run_result.string_process_time += string_process_time;
}

void SortMerge(ExpConfig& config, ExpContext& context, RunResult& run_result,
               rocksdb::Iterator* it_r, rocksdb::Iterator* it_s) {
  Timer timer1 = Timer();
  // double run_size = 10;

  int matches = 0, count1 = 0, count2 = 0;
  std::vector<std::string> value_split;
  Status status;
  rocksdb::get_perf_context()->Reset();
  it_r->SeekToFirst();
  it_s->SeekToFirst();
  double val_time = 0.0, get_time = 0.0;
  if (IsLazyIndex(config.r_index) || IsEagerIndex(config.r_index) ||
      IsLazyIndex(config.s_index) || IsEagerIndex(config.s_index)) {
    SortMergeForEagerLazy(config, context, run_result, it_r, it_s, matches);
  } else if (IsCompIndex(config.r_index) || IsCompIndex(config.s_index)) {
    SortMergeForComp(config, context, run_result, it_r, it_s, matches);
  }

  run_result.matches = matches;
  delete it_r;
  delete it_s;

  return;
}

void NonIndexExternalSortMerge(ExpConfig& config, ExpContext& context,
                               RunResult& run_result) {
  cout << "external sort merge" << endl;
  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE, VALUE_SIZE = config.VALUE_SIZE;

  cout << "Serializing data" << endl;
  // Sort R
  Timer timer1 = Timer();
  int run_size =
      max(int((config.M - 3 * 4096) / (PRIMARY_SIZE + VALUE_SIZE) / 2) - 1,
          int(config.r_tuples * (config.this_loop + 1) / 500));
  cout << "run_size: " << run_size << endl;
  string prefix_r = config.db_r + "_sj_output";
  // double run_size = 10;
  string output_file_r = prefix_r + ".txt";
  string prefix_s = config.db_s + "_sj_output";
  string output_file_s = prefix_s + ".txt";
  ReadOptions read_options;
  std::vector<std::string> value_split;
  Status status;
  // sort R
  cout << "Sort R" << endl;
  int num_ways_r = config.r_tuples * (config.this_loop + 1) / run_size + 1;
  MERGE::externalSort(context.db_r, output_file_r, num_ways_r, run_size,
                      VALUE_SIZE, SECONDARY_SIZE, run_result, prefix_r);
  // sort S
  cout << "Sort S" << endl;
  int num_ways_s = config.s_tuples * (config.this_loop + 1) / run_size + 1;
  MERGE::externalSort(context.db_s, output_file_s, num_ways_s, run_size,
                      VALUE_SIZE, SECONDARY_SIZE, run_result, prefix_s);

  int matches = 0;
  ifstream in_r(output_file_r);  // File on which sorting needs to be applied
  ifstream in_s(output_file_s);  // File on which sorting needs to be applied
  if (!in_r.is_open()) {
    cout << "Unable to open file R" << endl;
    exit(1);
  };
  if (!in_s.is_open()) {
    cout << "Unable to open file S" << endl;
    exit(1);
  };

  string line_r;
  string line_s;
  string temp_r_key, temp_r_value, temp_s_key, temp_s_value;
  string tmp;
  int count1 = 1, count2 = 1;
  char* buf1 = new char[4096];
  char* buf2 = new char[4096];
  in_r.rdbuf()->pubsetbuf(buf1, 4096);
  in_s.rdbuf()->pubsetbuf(buf2, 4096);
  Timer timer2 = Timer();
  if (!getline(in_s, line_s) || !getline(in_r, line_r)) {
    DebugPrint("!getline(in_r, line_r): " + to_string(!getline(in_r, line_r)));
    DebugPrint("!getline(in_s, line_s): " + to_string(!getline(in_s, line_s)));
  };

  if (in_s.eof() || in_r.eof()) {
    DebugPrint("in_s.eof(): " + to_string(in_s.eof()));
    DebugPrint("in_r.eof(): " + to_string(in_r.eof()));
    exit(1);
  };

  while (line_s != "" && line_r != "") {
    std::istringstream iss_r(line_r);  // string stream for line_r
    std::istringstream iss_s(line_s);  // string stream for line_s
    if (getline(iss_r, temp_r_key, ',') && getline(iss_r, temp_r_value) &&
        getline(iss_s, temp_s_key, ',') && getline(iss_s, temp_s_value)) {
      if (temp_r_key == temp_s_key) {
        while (getline(in_r, line_r)) {  // read next line_r
          std::istringstream temp_iss_r(line_r);
          std::string temp_first_r, temp_second_r;
          if (getline(temp_iss_r, temp_first_r, ',') &&
              getline(temp_iss_r, temp_second_r)) {
            if (temp_first_r == temp_r_key) {
              count1++;
              continue;
            } else {
              break;
            }
          }
        }
        while (getline(in_s, line_s)) {  // read next line_s
          std::istringstream temp_iss_s(line_s);
          std::string temp_first_s, temp_second_s;
          if (getline(temp_iss_s, temp_first_s, ',') &&
              getline(temp_iss_s, temp_second_s)) {
            if (temp_first_s == temp_s_key) {
              count2++;
              continue;
            } else {
              break;
            }
          }
        }

        matches += count1 * count2;
        // cout << "matches_after: " << matches << endl;
        count1 = 1;
        count2 = 1;
      } else if (temp_r_key < temp_s_key) {
        if (!getline(in_r, line_r)) break;
      } else if (temp_r_key > temp_s_key) {
        if (!getline(in_s, line_s)) break;
      }
    } else {
      break;
    }
  }
  run_result.sort_io_time += timer2.elapsed();
  run_result.matches = matches;
  return;
}

void ConcurrentSingleIndexExternalSortMerge(ExpConfig& config,
                                            ExpContext& context,
                                            RunResult& run_result,
                                            rocksdb::Iterator* it_s) {
  cout << "external sort merge" << endl;

  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE, VALUE_SIZE = config.VALUE_SIZE;

  cout << "Serializing data" << endl;
  // Sort R
  int run_size =
      max(int((config.M - 3 * 4096) / (PRIMARY_SIZE + VALUE_SIZE) / 2) - 1,
          int(config.r_tuples * (config.this_loop + 1) / 500));
  cout << "run_size: " << run_size << endl;
  double data_time = 0.0, index_time = 0.0, sort_time = 0.0, post_time = 0.0;
  Timer timer;
  string prefix_r = config.db_r + "_sj_output";
  string output_file_r = prefix_r + ".txt";
  ReadOptions read_options;
  std::vector<std::string> value_split;
  Status s;
  int num_ways_r = config.r_tuples * (config.this_loop + 1) / run_size + 1;
  cout << "num_ways_r: " << num_ways_r << endl;

  MERGE::concurrent_externalSort(
      context.db_r, output_file_r, num_ways_r, run_size, VALUE_SIZE,
      SECONDARY_SIZE, run_result, config.concurrent_threads, prefix_r);

  int thread_count = config.concurrent_threads;
  vector<string> chunk_files(thread_count);
  vector<string> chunk_start_keys(thread_count);
  vector<string> chunk_end_keys(thread_count);

  ifstream in_r(output_file_r);

  int total_lines = config.r_tuples;
  string line;

  int lines_per_chunk = total_lines / thread_count;
  vector<int> chunk_line_counts(thread_count, lines_per_chunk);

  for (int i = 0; i < thread_count; ++i) {
    chunk_files[i] = prefix_r + "_chunk_" + to_string(i) + ".txt";
  }

  int current_chunk = 0;
  ofstream out_chunk(chunk_files[current_chunk]);
  int lines_written = 0;
  string start_key, end_key, prev_key;
  while (getline(in_r, line)) {
    istringstream iss(line);
    string key;
    getline(iss, key, ',');
    if (lines_written == 0) {
      start_key = key;
      chunk_start_keys[current_chunk] = start_key;
    }
    if (lines_written >= chunk_line_counts[current_chunk] && key != prev_key) {
      chunk_end_keys[current_chunk] = prev_key;
      out_chunk.close();
      current_chunk++;
      out_chunk.open(chunk_files[current_chunk]);
      lines_written = 0;
      chunk_start_keys[current_chunk] = key;
    }
    out_chunk << line << '\n';
    lines_written++;
    prev_key = key;
  }
  if (out_chunk.is_open()) {
    chunk_end_keys[current_chunk] = prev_key;
    out_chunk.close();
  }
  in_r.close();

  vector<thread> threads;
  vector<int> thread_matches(thread_count, 0);

  for (int i = 0; i < thread_count; ++i) {
    threads.emplace_back([&, i]() {
      ifstream chunk_in(chunk_files[i]);
      rocksdb::Iterator* thread_it_s = context.db_s->NewIterator(ReadOptions());
      thread_it_s->Seek(chunk_start_keys[i]);

      string line_r;
      string temp_r_key, temp_r_value, temp_s_key, temp_s_value;
      string tmp;
      int local_matches = 0;
      int count1 = 1, count2 = 0;
      vector<string> local_value_split;
      Status local_s;

      while (getline(chunk_in, line_r)) {
        istringstream iss_r(line_r);
        bool is_success =
            getline(iss_r, temp_r_key, ',') && getline(iss_r, temp_r_value);
        if (!is_success) {
          continue;
        }

        while (thread_it_s->Valid()) {
          if (IsCompIndex(config.s_index)) {
            string s_key = thread_it_s->key().ToString();
            temp_s_key = s_key.substr(0, SECONDARY_SIZE);
            temp_s_value = s_key.substr(SECONDARY_SIZE, PRIMARY_SIZE);
          } else {
            temp_s_key = thread_it_s->key().ToString();
            temp_s_value = thread_it_s->value().ToString();
          }

          if (temp_s_key > chunk_end_keys[i]) {
            break;
          }

          if (temp_r_key == temp_s_key) {
            if (IsIndex(config.s_index)) {
              local_value_split.clear();
              boost::split(local_value_split, temp_s_value,
                           boost::is_any_of(":"));
            } else {
              local_value_split = {temp_s_value};
            }

            if (IsCoveringIndex(config.s_index) || !IsIndex(config.s_index)) {
              count2 += local_value_split.size();
              if (config.s_index == IndexType::CComp) {
                tmp = temp_s_key;
                while (thread_it_s->Valid()) {
                  thread_it_s->Next();
                  if (!thread_it_s->Valid()) break;
                  string s_key = thread_it_s->key().ToString();
                  temp_s_key = s_key.substr(0, SECONDARY_SIZE);
                  if (temp_s_key == tmp) {
                    count2++;
                  } else {
                    break;
                  }
                }
              }
            } else {
              if (IsEagerIndex(config.s_index) || IsLazyIndex(config.s_index)) {
                std::unordered_set<string> unique_values(
                    local_value_split.begin(), local_value_split.end());
                for (auto x : unique_values) {
                  local_s = context.db_s->Get(ReadOptions(),
                                              x.substr(0, PRIMARY_SIZE), &tmp);
                  if (local_s.ok() &&
                      tmp.substr(0, SECONDARY_SIZE) == temp_s_key)
                    count2++;
                }
              } else {
                string value_s;
                local_s =
                    context.db_s->Get(ReadOptions(), temp_s_value, &value_s);
                if (local_s.ok() &&
                    value_s.substr(0, SECONDARY_SIZE) == temp_s_key)
                  count2++;
                tmp = temp_s_key;
                while (thread_it_s->Valid()) {
                  thread_it_s->Next();
                  if (!thread_it_s->Valid()) break;
                  temp_s_key =
                      thread_it_s->key().ToString().substr(0, SECONDARY_SIZE);
                  temp_s_value = thread_it_s->key().ToString().substr(
                      SECONDARY_SIZE, PRIMARY_SIZE);
                  if (temp_s_key == tmp) {
                    local_s = context.db_s->Get(ReadOptions(), temp_s_value,
                                                &value_s);
                    if (local_s.ok() &&
                        value_s.substr(0, SECONDARY_SIZE) == temp_s_key)
                      count2++;
                  } else {
                    break;
                  }
                }
              }
            }

            local_matches += count1 * count2;
            count1 = 1;
            count2 = 0;
            break;  
          } else if (temp_r_key < temp_s_key) {
            break;
          } else {
            thread_it_s->Next();
          }
        }
      }
      thread_matches[i] = local_matches;
      delete thread_it_s;
    });
  }

  for (auto& th : threads) {
    if (th.joinable()) {
      th.join();
    }
  }

  // 汇总所有线程的匹配数
  int matches = 0;
  for (int i = 0; i < thread_count; ++i) {
    matches += thread_matches[i];
  }

  run_result.matches = matches;
  // 如果需要，更新其他时间指标

  delete it_s;
  return;
}

void SingleIndexExternalSortMerge(ExpConfig& config, ExpContext& context,
                                  RunResult& run_result,
                                  rocksdb::Iterator* it_s) {
  if (config.concurrent_threads > 1) {
    ConcurrentSingleIndexExternalSortMerge(config, context, run_result, it_s);
    return;
  }
  cout << "external sort merge" << endl;

  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE, VALUE_SIZE = config.VALUE_SIZE;

  cout << "Serializing data" << endl;
  // Sort R
  int run_size =
      max(int((config.M - 3 * 4096) / (PRIMARY_SIZE + VALUE_SIZE) / 2) - 1,
          int(config.r_tuples * (config.this_loop + 1) / 500));
  cout << "run_size: " << run_size << endl;
  double data_time = 0.0, index_time = 0.0, sort_time = 0.0, post_time = 0.0,
         string_process_time = 0.0;
  Timer timer;
  // double run_size = 10;
  string prefix_r = config.db_r + "_sj_output";
  string output_file_r = prefix_r + ".txt";
  string prefix_s = config.db_s + "_sj_output";
  string output_file_s = prefix_s + ".txt";
  ReadOptions read_options;
  std::vector<std::string> value_split;
  Status s;
  int num_ways_r = config.r_tuples * (config.this_loop + 1) / run_size + 1;
  cout << "num_ways_r: " << num_ways_r << endl;
  // S is indexed, already sorted
  MERGE::externalSort(context.db_r, output_file_r, num_ways_r, run_size,
                      VALUE_SIZE, SECONDARY_SIZE, run_result, prefix_r);

  int matches = 0;
  ifstream in_r(output_file_r);  // File on which sorting needs to be applied
  if (!in_r.is_open()) {
    cerr << "Unable to open file" << endl;
  };
  string line_r;
  string line_s;
  string temp_r_key, temp_r_value, temp_s_key, temp_s_value;
  string tmp;
  int count1 = 1, count2 = 0;
  char* buf1 = new char[4096];
  char* buf2 = new char[4096];
  in_r.rdbuf()->pubsetbuf(buf2, 4096);
  it_s->SeekToFirst();
  if (!it_s->Valid() || !getline(in_r, line_r)) {
    DebugPrint("!it_s->Valid(): " + to_string(!it_s->Valid()));
    DebugPrint("!getline(in_r, line_r): " + to_string(!getline(in_r, line_r)));
  };

  while (it_s->Valid() && line_r != "") {
    std::istringstream iss_r(line_r);  // string stream for line_r
    timer = Timer();
    bool is_success =
        getline(iss_r, temp_r_key, ',') && getline(iss_r, temp_r_value);
    sort_time += timer.elapsed();

    if (is_success) {
      // Timer string_timer = Timer();

      if (IsCompIndex(config.s_index)) {
        string s_key = it_s->key().ToString();
        temp_s_key = s_key.substr(0, SECONDARY_SIZE);
        temp_s_value = s_key.substr(SECONDARY_SIZE, PRIMARY_SIZE);
      } else {
        temp_s_key = it_s->key().ToString();
        temp_s_value = it_s->value().ToString();
      }

      // string_process_time += string_timer.elapsed();

      if (temp_r_key == temp_s_key) {
        timer = Timer();
        while (getline(in_r, line_r)) {  // read next line_r
          std::istringstream temp_iss_r(line_r);
          std::string temp_first_r, temp_second_r;
          if (getline(temp_iss_r, temp_first_r, ',') &&
              getline(temp_iss_r, temp_second_r)) {
            if (temp_first_r == temp_r_key) {
              count1++;
              continue;
            } else {
              break;
            }
          }
        }
        sort_time += timer.elapsed();
        if (IsIndex(config.s_index)) {
          timer = Timer();
          value_split =
              boost::split(value_split, temp_s_value, boost::is_any_of(":"));
          post_time += timer.elapsed();
        } else {
          value_split = {temp_s_value};
        }
        if (IsCoveringIndex(config.s_index) || !IsIndex(config.s_index)) {
          count2 += value_split.size();
          if (config.s_index == IndexType::CComp) {
            tmp = temp_s_key;
            while (it_s->Valid()) {
              // timer = Timer();
              it_s->Next();
              // index_time += timer.elapsed();

              if (!it_s->Valid()) break;

              // string_timer = Timer();
              string s_key = it_s->key().ToString();
              temp_s_key = s_key.substr(0, SECONDARY_SIZE);
              // string_process_time += string_timer.elapsed();
              if (temp_s_key == tmp) {
                count2++;
              } else
                break;
            }
          }
        } else {
          if (IsEagerIndex(config.s_index) || IsLazyIndex(config.s_index)) {
            timer = Timer();
            std::unordered_set<string> unique_values(value_split.begin(),
                                                     value_split.end());
            post_time += timer.elapsed();
            for (auto x : unique_values) {
              timer = Timer();
              s = context.db_s->Get(ReadOptions(), x.substr(0, PRIMARY_SIZE),
                                    &tmp);
              data_time += timer.elapsed();
              // string_timer = Timer();
              if (s.ok() && tmp.substr(0, SECONDARY_SIZE) == temp_s_key)
                count2++;
              // string_process_time += string_timer.elapsed();
            }
          } else {
            timer = Timer();
            string value_s;
            s = context.db_s->Get(ReadOptions(), temp_s_value, &value_s);
            data_time += timer.elapsed();
            // string_timer = Timer();
            if (s.ok() && value_s.substr(0, SECONDARY_SIZE) == temp_s_key)
              count2++;
            // string_process_time += string_timer.elapsed();
            tmp = temp_s_key;
            while (it_s->Valid()) {
              timer = Timer();
              it_s->Next();
              index_time += timer.elapsed();
              if (!it_s->Valid()) break;
              // string_timer = Timer();

              temp_s_key = it_s->key().ToString().substr(0, SECONDARY_SIZE);
              temp_s_value =
                  it_s->key().ToString().substr(SECONDARY_SIZE, PRIMARY_SIZE);
              // string_process_time += string_timer.elapsed();
              if (temp_s_key == tmp) {
                timer = Timer();
                s = context.db_s->Get(ReadOptions(), temp_s_value, &value_s);
                data_time += timer.elapsed();
                // string_timer = Timer();

                if (s.ok() && value_s.substr(0, SECONDARY_SIZE) == temp_s_key)
                  count2++;
                // string_process_time += string_timer.elapsed();
              } else
                break;
            }
          }
        }

        matches += count1 * count2;
        // cout << "matches_after: " << matches << endl;
        count1 = 1;
        count2 = 0;
      } else if (temp_r_key < temp_s_key) {
        timer = Timer();
        if (!getline(in_r, line_r)) break;
        sort_time += timer.elapsed();
      } else if (temp_r_key > temp_s_key) {
        // timer = Timer();
        it_s->Next();
        // index_time += timer.elapsed();
      }
    } else {
      break;
    }
  }

  run_result.matches = matches;
  run_result.get_data_time += data_time;
  run_result.get_index_time += index_time;
  run_result.sort_io_time += sort_time;
  run_result.post_list_time += post_time;
  // run_result.string_process_time += string_process_time;
  delete it_s;
  return;
}

void ConcurrentNestedLoop(ExpConfig& config, ExpContext& context,
                          RunResult& result) {
  ReadOptions read_options;
  std::cout << "joining ... " << std::endl;
  context.rocksdb_opt.statistics->Reset();
  rocksdb::get_iostats_context()->Reset();
  rocksdb::get_perf_context()->Reset();

  std::atomic<uint64_t> matches(0);
  std::atomic<double> data_time(0.0);

  // 创建线程池
  int num_threads = config.concurrent_threads;
  ThreadPool pool(num_threads);

  // 定义键空间的范围，假设已知最小和最大键
  uint64_t min_key = 0;
  uint64_t max_key = 4000000000;

  uint64_t total_keys = max_key - min_key + 1;
  uint64_t keys_per_thread = (total_keys + num_threads - 1) / num_threads;

  std::vector<std::future<void>> futures;

  for (int i = 0; i < num_threads; ++i) {
    uint64_t start_key = min_key + i * keys_per_thread;
    uint64_t end_key = std::min(start_key + keys_per_thread - 1, max_key);
    if (i == num_threads - 1) {
      end_key = 9999999999;
    }

    futures.emplace_back(pool.enqueue([&, start_key, end_key]() {
      rocksdb::Iterator* it_r = context.db_r->NewIterator(read_options);
      rocksdb::DB* db_s = context.db_s;
      std::string value;
      Status status;
      uint64_t local_matches = 0;
      double local_data_time = 0.0;

      // 将起始键转换为字符串
      std::string start_key_str = std::to_string(start_key);
      start_key_str =
          std::string(config.PRIMARY_SIZE - start_key_str.length(), '0') +
          start_key_str;
      it_r->Seek(start_key_str);
      cout << "start_key_str: " << it_r->key().ToString() << endl;
      while (it_r->Valid()) {
        std::string key_str = it_r->key().ToString();
        int64_t current_key = std::stoll(key_str);

        if (current_key > end_key) {
          break;
        }

        // 处理键
        std::string tmp_r =
            it_r->value().ToString().substr(0, config.SECONDARY_SIZE);

        Timer timer;
        status = db_s->Get(read_options, tmp_r, &value);
        local_data_time += timer.elapsed();
        if (status.ok()) local_matches++;

        it_r->Next();
      }

      matches += local_matches;
      // data_time += local_data_time;

      delete it_r;
    }));
  }

  // 等待所有线程完成任务
  for (auto& future : futures) {
    future.get();
  }

  result.get_data_time += data_time.load();
  result.matches = matches.load();
}

void NestedLoop(ExpConfig& config, ExpContext& context, RunResult& result) {
  if (config.concurrent_threads > 1) {
    ConcurrentNestedLoop(config, context, result);
    return;
  }
  ReadOptions read_options;
  cout << "joining ... " << endl;
  context.rocksdb_opt.statistics->Reset();
  rocksdb::get_iostats_context()->Reset();
  rocksdb::get_perf_context()->Reset();
  rocksdb::Iterator* it_r = context.db_r->NewIterator(read_options);
  rocksdb::Iterator* it_s = context.db_s->NewIterator(read_options);
  Slice upper_bound_slice;
  uint64_t matches = 0;
  Status status;
  double valid_time = 0.0;
  int valid_count = 0;
  int total_io = 0;
  vector<int> avg_io;
  string tmp_r, value;
  double data_time = 0.0;
  // double string_process_time = 0.0;
  Timer string_timer;
  for (it_r->SeekToFirst(); it_r->Valid(); it_r->Next()) {
    // string_timer = Timer();
    tmp_r = it_r->value().ToString().substr(0, config.SECONDARY_SIZE);
    // string_process_time += string_timer.elapsed();
    Timer timer = Timer();
    status = context.db_s->Get(read_options, tmp_r, &value);
    data_time += timer.elapsed();
    if (status.ok()) matches++;
  }
  result.get_data_time += data_time;
  result.matches = matches;
  // result.string_process_time += string_process_time;

  delete it_r;
  delete it_s;
  return;
}

void IndexNestedLoop(ExpConfig& config, ExpContext& context, RunResult& result,
                     bool covering = true) {
  cout << "index nested loop joining..." << endl;
  string secondary_key_lower, secondary_key_upper, value, tmp_r, tmp_primary;
  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE, VALUE_SIZE = config.VALUE_SIZE;
  ReadOptions read_options;
  if (IsCompIndex(config.r_index) || IsCompIndex(config.s_index)) {
    read_options.total_order_seek = false;
    read_options.auto_prefix_mode = false;
  }
  rocksdb::Iterator* it_r = context.db_r->NewIterator(read_options);
  rocksdb::Iterator* it_s = context.ptr_index_s->NewIterator(read_options);
  uint64_t matches = 0;  // number of matches
  Status s;
  string tmp;
  std::vector<std::string> value_split;
  Timer timer = Timer();
  double data_time = 0.0, index_time = 0.0, post_time = 0.0;
  long long int count = 0;
  long long int count_timer = 0;
  // double string_process_time = 0.0;
  Timer string_timer = Timer();
  if (IsEagerIndex(config.s_index) || IsLazyIndex(config.s_index)) {
    for (it_r->SeekToFirst(); it_r->Valid();) {
      tmp_r = it_r->value().ToString().substr(0, SECONDARY_SIZE);
      timer = Timer();
      s = context.ptr_index_s->Get(read_options, tmp_r, &value);
      index_time += timer.elapsed();
      if (s.ok()) {
        timer = Timer();
        value_split = boost::split(value_split, value, boost::is_any_of(":"));
        post_time += timer.elapsed();

        if (covering) {
          matches += value_split.size();
        } else {
          timer = Timer();
          std::set<std::string> value_set(value_split.begin(),
                                          value_split.end());
          post_time += timer.elapsed();
          for (auto x : value_set) {
            count += 1;
            Timer timer = Timer();
            s = context.db_s->Get(read_options, x.substr(0, PRIMARY_SIZE),
                                  &tmp);
            data_time += timer.elapsed();
            // string_timer = Timer();
            if (s.ok() && tmp.substr(0, SECONDARY_SIZE) == tmp_r) matches++;
            // string_process_time += string_timer.elapsed();
          }
        }
      }
      timer = Timer();
      it_r->Next();
      data_time += timer.elapsed();

      if (!it_r->Valid()) break;
    }
  } else if (IsCompIndex(config.s_index)) {
    // it_r: primary key, value: secondary key
    for (it_r->SeekToFirst(); it_r->Valid();) {
      // string_timer = Timer();
      tmp_r = it_r->value().ToString().substr(0, SECONDARY_SIZE);
      secondary_key_lower = tmp_r + string(PRIMARY_SIZE, '0');
      secondary_key_upper = tmp_r + string(PRIMARY_SIZE, '9');
      // string_process_time += string_timer.elapsed();

      timer = Timer();
      it_s->Seek(secondary_key_lower);
      index_time += timer.elapsed();

      while (true) {
        count += 1;
        // string_timer = Timer();
        if (!it_s->Valid()) {
          break;
        }
        string it_s_key = it_s->key().ToString();
        // string_process_time += string_timer.elapsed();

        if (it_s_key > secondary_key_upper) break;
        // string_timer = Timer();
        string tmp_s = it_s_key.substr(0, SECONDARY_SIZE);
        // string_process_time += string_timer.elapsed();

        if (tmp_s == tmp_r) {
          if (!covering) {
            timer = Timer();
            s = context.db_s->Get(
                read_options,
                it_s->key().ToString().substr(SECONDARY_SIZE, PRIMARY_SIZE),
                &value);
            data_time += timer.elapsed();
            // string_timer = Timer();
            if (s.ok() && value.substr(0, SECONDARY_SIZE) == tmp_s) matches++;
            // string_process_time += string_timer.elapsed();
          } else
            matches++;
        }
        timer = Timer();
        it_s->Next();
        index_time += timer.elapsed();

        // string_timer = Timer();
        bool is_success =
            !it_s->Valid() || it_s->key().ToString() > secondary_key_upper;
        // string_process_time += string_timer.elapsed();
        if (is_success) break;
      }
      timer = Timer();
      it_r->Next();
      data_time += timer.elapsed();
      if (!it_r->Valid()) break;
    }
  }

  result.matches = matches;
  result.get_data_time += data_time;
  result.get_index_time += index_time;
  result.post_list_time += post_time;
  // result.string_process_time += string_process_time;

  cout << "count num:" << count << endl;
  cout << "count_timer: " << count_timer << endl;

  delete it_r;
  delete it_s;
}
