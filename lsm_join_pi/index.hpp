#pragma once

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <random>
#include <regex>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "exp_utils.hpp"
#include "rocksdb/db.h"
#include "rocksdb/file_system.h"
#include "rocksdb/iostats_context.h"
#include "rocksdb/perf_context.h"
#include "rocksdb/sst_file_reader.h"
#include "rocksdb/statistics.h"

using namespace std;
using namespace ROCKSDB_NAMESPACE;

void waitForUpdate(DB *db) {
  uint64_t num_running_flushes, num_pending_flushes, num_running_compactions,
      num_pending_compactions;
  while (true) {
    db->GetIntProperty(DB::Properties::kNumRunningFlushes,
                       &num_running_flushes);
    db->GetIntProperty(DB::Properties::kMemTableFlushPending,
                       &num_pending_flushes);
    db->GetIntProperty(DB::Properties::kNumRunningCompactions,
                       &num_running_compactions);
    db->GetIntProperty(DB::Properties::kCompactionPending,
                       &num_pending_compactions);
    if (num_running_compactions == 0 && num_pending_compactions == 0 &&
        num_running_flushes == 0 && num_pending_flushes == 0)
      break;
  }
}

uint64_t randomNumber(int n = 10) {
  uint64_t max_val = pow(10, n);
  return rand() * 13131 % max_val;
}

void generatePK(uint64_t r, std::vector<uint64_t> &R, int c = 1, int n = 10) {
  static int seed = 123;
  srand(seed++);
  std::mt19937 gen(seed);
  std::uniform_int_distribution<> dis(1, 2 * c - 1);
  uint64_t x;
  int y;
  while (R.size() < r) {
    x = randomNumber(n);
    y = dis(gen);
    for (int j = 0; j < y; ++j) {
      R.push_back(x);
    }
  }
}

// R is the column with primary index
void generateData(uint64_t s, uint64_t r, double eps, int k,
                  std::vector<uint64_t> &S, std::vector<uint64_t> &R,
                  int n = 10) {
  static int seed = 123;
  srand(seed);
  std::mt19937 gen(seed++);
  std::uniform_int_distribution<> dis(1, 2 * k - 1);
  uint64_t x, y;
  for (int i = 0; i < s; ++i) {
    x = randomNumber(n);
    S.push_back(x);
    if (((double)rand() / RAND_MAX) > eps) {
      y = dis(gen);
      for (int j = 0; j < y; ++j) {
        R.push_back(x);
      }
    }
  }
  cout << "S before size: " << S.size() << endl;
  cout << "R before size: " << R.size() << endl;
  while (R.size() < r) {
    x = randomNumber(n);
    y = dis(gen);
    for (int j = 0; j < y; ++j) {
      R.push_back(x);
    }
  }
}

void ingest_pk_data(uint64_t tuples, DB *db, const std::vector<uint64_t> &data,
                    int VALUE_SIZE, int SECONDARY_SIZE, int PRIMARY_SIZE) {
  std::string tmp, tmp_key, tmp_value;
  for (uint64_t i = 0; i < tuples; i++) {
    if ((i + 1) % 5000000 == 0) {
      cout << (i + 1) / 1000000 << " million" << endl;
    }
    tmp = to_string(data[i]);
    tmp_key =
        string(PRIMARY_SIZE - min(PRIMARY_SIZE, int(tmp.length())), '0') + tmp;
    // cout << tmp_key << " " << tmp_value << endl;
    db->Put(WriteOptions(), tmp_key, string(VALUE_SIZE, '0'));
  }
  waitForUpdate(db);
}

void ingest_data(uint64_t tuples, DB *db, const std::vector<uint64_t> &pk,
                 const std::vector<uint64_t> &data, int VALUE_SIZE,
                 int SECONDARY_SIZE, int PRIMARY_SIZE) {
  std::string tmp, tmp_key, tmp_value;

  cout << "pk size: " << pk.size() << endl;
  bool use_pk = (pk.size() != 0);
  // write to file
  ofstream outfile;

  for (uint64_t i = 0; i < tuples; i++) {
    if ((i + 1) % 5000000 == 0) {
      cout << (i + 1) / 1000000 << " million" << endl;
    }
    tmp = use_pk ? std::to_string(pk[i]) : std::to_string(i);
    tmp_key =
        string(PRIMARY_SIZE - min(PRIMARY_SIZE, int(tmp.length())), '0') + tmp;
    tmp = to_string(data[i]);
    tmp_value =
        string(SECONDARY_SIZE - min(SECONDARY_SIZE, int(tmp.length())), '0') +
        tmp + string(VALUE_SIZE - SECONDARY_SIZE, '0');
    db->Put(WriteOptions(), tmp_key, tmp_value);
  }
  waitForUpdate(db);
}

void build_composite_index(DB *db, DB *index, uint64_t *data,
                           const std::vector<uint64_t> &pk, uint64_t tuples,
                           int TOTAL_VALUE_SIZE, int SECONDARY_SIZE,
                           int PRIMARY_SIZE) {
  cout << "building composite index..." << endl;
  string secondary_key, value, tmp_secondary, tmp_primary, tmp_value;
  WriteOptions write_options;
  ReadOptions read_options;
  Status s;
  bool use_pk = (pk.size() != 0);
  for (uint64_t i = 0; i < tuples; i++) {
    if ((i + 1) % 5000000 == 0) {
      cout << (i + 1) / 1000000 << " million" << endl;
    }
    tmp_secondary = std::to_string(data[i]);
    tmp_primary = use_pk ? std::to_string(pk[i]) : std::to_string(i);
    secondary_key =
        string(
            SECONDARY_SIZE - min(SECONDARY_SIZE, int(tmp_secondary.length())),
            '0') +
        tmp_secondary +
        string(PRIMARY_SIZE - min(PRIMARY_SIZE, int(tmp_primary.length())),
               '0') +
        tmp_primary;
    index->Put(write_options, secondary_key, NULL);
  }
  waitForUpdate(index);
}

void build_covering_composite_index(DB *db, DB *index, uint64_t *data,
                                    const std::vector<uint64_t> &pk,
                                    uint64_t tuples, int TOTAL_VALUE_SIZE,
                                    int SECONDARY_SIZE, int PRIMARY_SIZE) {
  cout << "building covering composite index..." << endl;
  string secondary_key, value, tmp_secondary, tmp_primary, tmp_value, tmp,
      tmp_key;
  WriteOptions write_options;
  ReadOptions read_options;
  Status s;
  struct timespec t1, t2;
  double index_time = 0.0;
  bool use_pk = (pk.size() != 0);
  for (uint64_t i = 0; i < tuples; i++) {
    if ((i + 1) % 5000000 == 0) {
      cout << (i + 1) / 1000000 << " million" << endl;
    }
    tmp = use_pk ? std::to_string(pk[i]) : std::to_string(i);
    tmp_key =
        string(PRIMARY_SIZE - min(PRIMARY_SIZE, int(tmp.length())), '0') + tmp;
    tmp = to_string(data[i]);
    tmp_value =
        string(SECONDARY_SIZE - min(SECONDARY_SIZE, int(tmp.length())), '0') +
        tmp + string(TOTAL_VALUE_SIZE - SECONDARY_SIZE, '0');
    Timer timer1 = Timer();
    s = db->Get(read_options, tmp_key, &tmp_secondary);
    if (s.ok())
      index->SingleDelete(write_options,
                          tmp_secondary.substr(0, SECONDARY_SIZE) + tmp_key);
    index->Put(write_options, tmp_value.substr(0, SECONDARY_SIZE) + tmp_key,
               string(TOTAL_VALUE_SIZE - SECONDARY_SIZE, '0'));
    index_time += timer1.elapsed();
    db->Put(WriteOptions(), tmp_key, tmp_value);
  }
  waitForUpdate(index);
  waitForUpdate(db);
  cout << "index_time: " << index_time << endl;
}

void composite_index_nested_loop(DB *index_r, DB *index_s, DB *data_r,
                                 DB *data_s, int SECONDARY_SIZE,
                                 int PRIMARY_SIZE, bool validation = true) {
  cout << "composite index nested loop joining..." << endl;
  cout << "validation: " << validation << endl;
  string secondary_key_lower, secondary_key_upper, value, tmp_r, tmp_s;
  ReadOptions read_options;
  read_options.total_order_seek = false;
  read_options.auto_prefix_mode = false;
  rocksdb::Iterator *it_r = data_r->NewIterator(read_options);
  rocksdb::Iterator *it_s = index_s->NewIterator(read_options);
  Slice upper_bound_slice;
  uint64_t matches = 0;
  int r_num = 0;
  Status s1, s2;
  struct timespec t1, t2;
  double valid_time = 0.0;
  int valid_count = 0;
  int total_io = 0;
  vector<int> avg_io;
  for (it_r->SeekToFirst(); it_r->Valid(); it_r->Next()) {
    r_num++;
    // if (matches % 1000000 == 0) {
    //   cout << matches / 1000000 << " million" << endl;
    // }
    tmp_r = it_r->value().ToString().substr(0, SECONDARY_SIZE);
    secondary_key_lower = tmp_r + string(PRIMARY_SIZE, '0');
    secondary_key_upper = tmp_r + string(PRIMARY_SIZE, '9');
    // upper_bound_slice = Slice(secondary_key_upper);
    // read_options.iterate_upper_bound = &upper_bound_slice;
    // it_s = index_s->NewIterator(read_options);
    it_s->Seek(secondary_key_lower);
    // rocksdb::get_perf_context()->Reset();
    for (; it_s->Valid() && it_s->key().ToString() <= secondary_key_upper;
         it_s->Next()) {
      tmp_s = it_s->key().ToString().substr(0, SECONDARY_SIZE);
      if (tmp_s == tmp_r) {
        if (validation) {
          clock_gettime(CLOCK_MONOTONIC, &t1);
          s2 = data_s->Get(
              read_options,
              it_s->key().ToString().substr(SECONDARY_SIZE, PRIMARY_SIZE),
              &value);
          if (s2.ok() && value.substr(0, SECONDARY_SIZE) == tmp_s) matches++;
          clock_gettime(CLOCK_MONOTONIC, &t2);
          valid_time += ((t2.tv_sec - t1.tv_sec) +
                         (t2.tv_nsec - t1.tv_nsec) / 1000000000.0);
        } else
          matches++;
      }
    }
    // total_io = get_perf_context()->block_read_count;
    // avg_io.emplace_back(total_io);
    // double sum = std::accumulate(std::begin(avg_io), std::end(avg_io), 0.0);
    // double mean = sum / avg_io.size();
    // cout << "composite once io: " << mean << endl;
  }
  cout << "matches: " << matches << endl;
  cout << "r_num: " << r_num << endl;
  cout << "valid_time: " << valid_time << endl;
  delete it_r;
  delete it_s;
}

void build_covering_lazy_index(DB *db, DB *index, uint64_t *data,
                               const std::vector<uint64_t> &pk, uint64_t tuples,
                               int TOTAL_VALUE_SIZE, int SECONDARY_SIZE,
                               int PRIMARY_SIZE) {
  cout << "building covering lazy index..." << endl;
  string secondary_key, value, tmp_secondary, tmp_primary, tmp, tmp_key,
      tmp_value;
  std::vector<std::string> value_split;
  WriteOptions write_options;
  ReadOptions read_options;
  Status s;
  bool use_pk = (pk.size() != 0);
  struct timespec t1, t2;
  double index_time = 0.0;
  for (uint64_t i = 0; i < tuples; i++) {
    if ((i + 1) % 5000000 == 0) {
      cout << (i + 1) / 1000000 << " million" << endl;
    }
    tmp = use_pk ? std::to_string(pk[i]) : std::to_string(i);
    tmp_key =
        string(PRIMARY_SIZE - min(PRIMARY_SIZE, int(tmp.length())), '0') + tmp;
    tmp = to_string(data[i]);
    tmp_value =
        string(SECONDARY_SIZE - min(SECONDARY_SIZE, int(tmp.length())), '0') +
        tmp + string(TOTAL_VALUE_SIZE - SECONDARY_SIZE, '0');
    Timer timer1 = Timer();
    s = db->Get(read_options, tmp_key,
                &tmp_secondary);  // 回DataTable Check key是否存在
    if (s.ok()) {
      s = index->Get(read_options, tmp_secondary.substr(0, SECONDARY_SIZE),
                     &tmp);
      if (s.ok()) {
        boost::split(value_split, tmp, boost::is_any_of(":"));
        for (auto it = value_split.begin(); it != value_split.end();) {
          if (it->substr(0, PRIMARY_SIZE) == tmp_key)
            it = value_split.erase(it);  // 删除重复的primary key
          else
            ++it;
        }
        if (value_split.size() == 0) {
          index->Delete(write_options, tmp_secondary.substr(0, SECONDARY_SIZE));
        } else {
          string new_value = value_split[0];
          for (size_t i = 1; i < value_split.size(); ++i)
            new_value = new_value + ":" + value_split[i];
          index->Put(write_options, tmp_secondary.substr(0, SECONDARY_SIZE),
                     new_value);
        }
      }
    }
    index->Merge(WriteOptions(), tmp_value.substr(0, SECONDARY_SIZE),
                 tmp_key + string(TOTAL_VALUE_SIZE - SECONDARY_SIZE,
                                  '0'));  // Lazy Index
    index_time += timer1.elapsed();
    db->Put(WriteOptions(), tmp_key, tmp_value);
  }
  waitForUpdate(index);
  waitForUpdate(db);
  cout << "index_time: " << index_time << endl;
}

void build_lazy_index(DB *db, DB *index, uint64_t *data,
                      const std::vector<uint64_t> &pk, uint64_t tuples,
                      int TOTAL_VALUE_SIZE, int SECONDARY_SIZE,
                      int PRIMARY_SIZE) {
  cout << "building lazy index..." << endl;
  string secondary_key, value, tmp_secondary, tmp_primary;
  WriteOptions write_options;
  bool use_pk = (pk.size() != 0);
  for (uint64_t i = 0; i < tuples; i++) {
    if ((i + 1) % 5000000 == 0) {
      cout << (i + 1) / 1000000 << " million" << endl;
    }
    tmp_secondary = to_string(data[i]);
    tmp_primary = use_pk ? std::to_string(pk[i]) : std::to_string(i);
    secondary_key = string(SECONDARY_SIZE -
                               min(SECONDARY_SIZE, int(tmp_secondary.length())),
                           '0') +
                    tmp_secondary;
    value = string(PRIMARY_SIZE - min(PRIMARY_SIZE, int(tmp_primary.length())),
                   '0') +
            tmp_primary;
    // cout << secondary_key << " " << value << endl;
    index->Merge(WriteOptions(), secondary_key, value);
  }

  waitForUpdate(index);
}

void lazy_index_nested_loop(DB *index_r, DB *index_s, DB *db_r, DB *db_s,
                            int SECONDARY_SIZE, int PRIMARY_SIZE,
                            bool validation = true) {
  cout << "lazy index nested loop joining..." << endl;
  cout << "validation: " << validation << endl;
  string secondary_key_lower, secondary_key_upper, value, tmp_secondary,
      tmp_primary;
  ReadOptions read_options;
  rocksdb::Iterator *it_r = db_r->NewIterator(read_options);
  rocksdb::Iterator *it_s = index_s->NewIterator(read_options);
  uint64_t matches = 0;
  Status s;
  int r_num = 0;
  string tmp;
  std::vector<std::string> value_split;
  struct timespec t1, t2;
  double valid_time = 0.0;
  for (it_r->SeekToFirst(); it_r->Valid(); it_r->Next()) {
    tmp_secondary = it_r->value().ToString().substr(0, SECONDARY_SIZE);
    // rocksdb::get_perf_context()->Reset();
    s = index_s->Get(read_options, tmp_secondary, &value);
    if (s.ok()) {
      // if (get_perf_context()->block_read_count > 0)
      //   cout << "lazy read once io: " << get_perf_context()->block_read_count
      //        << endl;
      value_split = boost::split(value_split, value, boost::is_any_of(":"));
      std::set<std::string> value_set(value_split.begin(), value_split.end());
      if (validation) {
        for (auto x : value_set) {
          // cout << x << endl;
          clock_gettime(CLOCK_MONOTONIC, &t1);  // TODO 统一成外面
          s = db_s->Get(read_options, x.substr(0, PRIMARY_SIZE), &tmp);
          if (s.ok() && tmp.substr(0, SECONDARY_SIZE) == tmp_secondary)
            matches++;
          clock_gettime(CLOCK_MONOTONIC, &t2);
          valid_time += ((t2.tv_sec - t1.tv_sec) +
                         (t2.tv_nsec - t1.tv_nsec) / 1000000000.0);
        }
      } else {
        for (auto x : value_set) matches++;
      }
    }
  }
  cout << "matches: " << matches << endl;
  cout << "valid_time: " << valid_time << endl;
  delete it_r;
  delete it_s;
}

void build_covering_eager_index(DB *db, DB *index, uint64_t *data,
                                const std::vector<uint64_t> &pk,
                                uint64_t tuples, int TOTAL_VALUE_SIZE,
                                int SECONDARY_SIZE, int PRIMARY_SIZE) {
  cout << "building covering eager index..." << endl;
  string secondary_key, value, tmp_secondary, tmp_primary, tmp, tmp_key,
      tmp_value;
  std::vector<std::string> value_split;
  WriteOptions write_options;
  ReadOptions read_options;
  Status s;
  bool use_pk = (pk.size() != 0);
  struct timespec t1, t2;
  double index_time = 0.0;
  for (uint64_t i = 0; i < tuples; i++) {
    if ((i + 1) % 5000000 == 0) {
      cout << (i + 1) / 1000000 << " million" << endl;
    }
    tmp = use_pk ? std::to_string(pk[i]) : std::to_string(i);
    tmp_key =
        string(PRIMARY_SIZE - min(PRIMARY_SIZE, int(tmp.length())), '0') + tmp;
    tmp = to_string(data[i]);
    tmp_value =
        string(SECONDARY_SIZE - min(SECONDARY_SIZE, int(tmp.length())), '0') +
        tmp + string(TOTAL_VALUE_SIZE - SECONDARY_SIZE, '0');
    Timer timer1 = Timer();
    s = db->Get(read_options, tmp_key, &tmp_secondary);
    if (s.ok()) {
      s = index->Get(read_options, tmp_secondary.substr(0, SECONDARY_SIZE),
                     &tmp);
      if (s.ok()) {
        boost::split(value_split, tmp, boost::is_any_of(":"));
        for (auto it = value_split.begin(); it != value_split.end();) {
          if (it->substr(0, PRIMARY_SIZE) == tmp_key)
            it = value_split.erase(it);
          else
            ++it;
        }
        if (value_split.size() == 0) {
          index->Delete(write_options, tmp_secondary.substr(0, SECONDARY_SIZE));
        } else {
          string new_value = value_split[0];
          for (size_t i = 1; i < value_split.size(); ++i)
            new_value = new_value + ":" + value_split[i];
          index->Put(write_options, tmp_secondary.substr(0, SECONDARY_SIZE),
                     new_value);
        }
      }
    }
    secondary_key = tmp_value.substr(0, SECONDARY_SIZE);
    // tmp_primary = tmp_key + string(TOTAL_VALUE_SIZE - SECONDARY_SIZE, '0');
    s = index->Get(read_options, secondary_key, &tmp_primary);
    if (s.ok()) {
      tmp_primary = tmp_key + string(TOTAL_VALUE_SIZE - SECONDARY_SIZE, '0') +
                    ":" + tmp_primary;
      index->Put(write_options, secondary_key, tmp_primary);
    } else {
      index->Put(write_options, secondary_key,
                 tmp_key + string(TOTAL_VALUE_SIZE - SECONDARY_SIZE, '0'));
    }
    index_time += timer1.elapsed();
    db->Put(WriteOptions(), tmp_key, tmp_value);
  }
  waitForUpdate(index);
  waitForUpdate(db);
  cout << "index_time: " << index_time << endl;
}

void build_eager_index(DB *db, DB *index, uint64_t *data,
                       const std::vector<uint64_t> &pk, uint64_t tuples,
                       int TOTAL_VALUE_SIZE, int SECONDARY_SIZE,
                       int PRIMARY_SIZE) {
  cout << "building eager index..." << endl;
  string secondary_key, value, tmp_secondary, tmp_primary, tmp_value, tmp;
  ReadOptions read_options;
  WriteOptions write_options;
  Status s;
  vector<string> value_split;
  bool use_pk = (pk.size() != 0);
  for (uint64_t i = 0; i < tuples; i++) {
    if ((i + 1) % 5000000 == 0) {
      cout << (i + 1) / 1000000 << " million" << endl;
    }
    tmp_secondary = to_string(data[i]);
    tmp_primary = use_pk ? std::to_string(pk[i]) : std::to_string(i);
    secondary_key = string(SECONDARY_SIZE -
                               min(SECONDARY_SIZE, int(tmp_secondary.length())),
                           '0') +
                    tmp_secondary;
    value = string(PRIMARY_SIZE - min(PRIMARY_SIZE, int(tmp_primary.length())),
                   '0') +
            tmp_primary;
    s = index->Get(read_options, secondary_key, &tmp_primary);
    if (s.ok()) {
      tmp_primary = value + ":" + tmp_primary;
      index->Put(write_options, secondary_key, tmp_primary);
    } else {
      index->Put(write_options, secondary_key, value);
    }
  }
  waitForUpdate(index);
}

void eager_index_nested_loop(DB *index_r, DB *index_s, DB *db_r, DB *db_s,
                             int SECONDARY_SIZE, int PRIMARY_SIZE,
                             bool validation = true) {
  cout << "eager index nested loop joining..." << endl;
  cout << "validation: " << validation << endl;
  struct timespec t1, t2;
  double validation_time = 0.0;
  string secondary_key_lower, secondary_key_upper, value, tmp_secondary,
      tmp_primary;
  ReadOptions read_options;
  rocksdb::Iterator *it_r = db_r->NewIterator(read_options);
  rocksdb::Iterator *it_s = index_s->NewIterator(read_options);
  uint64_t matches = 0;
  Status s;
  int r_num = 0;
  string tmp;
  std::vector<std::string> value_split;
  for (it_r->SeekToFirst(); it_r->Valid(); it_r->Next()) {
    tmp_secondary = it_r->value().ToString().substr(0, SECONDARY_SIZE);
    // rocksdb::get_perf_context()->Reset();
    s = index_s->Get(read_options, tmp_secondary, &value);
    if (s.ok()) {
      // if (get_perf_context()->block_read_count > 0)
      //   cout << "eager azy read once io: " <<
      //   get_perf_context()->block_read_count
      //        << endl;
      boost::split(value_split, value, boost::is_any_of(":"));
      std::set<std::string> value_set(value_split.begin(), value_split.end());
      if (validation) {
        clock_gettime(CLOCK_MONOTONIC, &t1);
        for (auto x : value_set) {
          s = db_s->Get(read_options, x, &tmp);
          // cout << tmp.substr(0, SECONDARY_SIZE) << " " << tmp_secondary <<
          // endl;
          if (s.ok() && tmp.substr(0, SECONDARY_SIZE) == tmp_secondary)
            matches++;
        }
        clock_gettime(CLOCK_MONOTONIC, &t2);
        validation_time += ((t2.tv_sec - t1.tv_sec) +
                            (t2.tv_nsec - t1.tv_nsec) / 1000000000.0);
      } else {
        for (auto x : value_set) matches++;
      }
    }
  }
  cout << "matches: " << matches << endl;
  cout << "validation_time: " << validation_time << endl;
  delete it_r;
  delete it_s;
}

void build_bloom_filter_index(DB *db, uint64_t *data, uint64_t tuples,
                              int TOTAL_VALUE_SIZE, int SECONDARY_SIZE,
                              int PRIMARY_SIZE) {
  cout << "building composite index..." << endl;
  string secondary_key, value, tmp_secondary, tmp_primary;
  string file_path = "/tmp/wiki_128_R/000083.sst";
  SstFileReader *sst_reader = nullptr;
  sst_reader->Open(file_path);
  auto iter = sst_reader->NewIterator(ReadOptions());
  for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
    std::cout << "Key: " << iter->key().ToString()
              << ", Value: " << iter->value().ToString() << std::endl;
  }
}

void bloom_filter_index_nested_loop(DB *data_r, DB *data_s, int SECONDARY_SIZE,
                                    int PRIMARY_SIZE) {
  cout << "bloom filter index nested loop joining..." << endl;
  string secondary_key_lower, secondary_key_upper, value, tmp_r, tmp_s;
  ReadOptions read_options;
  // read_options.auto_prefix_mode = true;
  // std::string upper_bound;
  // Slice upper_bound_slice;
  rocksdb::Iterator *it_r = data_r->NewIterator(read_options);
  rocksdb::Iterator *it_s = data_s->NewIterator(read_options);
  uint64_t matches = 0;
  int r_num = 0;
  Status s1, s2;
  for (it_r->SeekToFirst(); it_r->Valid(); it_r->Next()) {
    r_num++;
    // if (matches % 1000000 == 0) {
    //   cout << matches / 1000000 << " million" << endl;
    // }
    // cout << it_r->key().ToString() << endl;
    value = it_r->value().ToString().substr(PRIMARY_SIZE, SECONDARY_SIZE);
  }
  // Block::Iter *block_iter;
  cout << "matches: " << matches << endl;
  cout << "r_num: " << r_num << endl;
}