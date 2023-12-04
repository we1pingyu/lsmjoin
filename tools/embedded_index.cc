//  Copyright (c) 2011-present, Facebook, Inc.  All rights reserved.
//  This source code is licensed under both the GPLv2 (found in the
//  COPYING file in the root directory) and Apache 2.0 License
//  (found in the LICENSE.Apache file in the root directory).
//

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "bloom/bloom_filter.hpp"
#include "boost/unordered_set.hpp"
#include "cache/cache_entry_roles.h"
#include "cache/cache_key.h"
#include "db/compaction/compaction_picker.h"
#include "db/dbformat.h"
#include "db/pinned_iterators_manager.h"
#include "file/file_prefetch_buffer.h"
#include "file/file_util.h"
#include "file/random_access_file_reader.h"
#include "logging/logging.h"
#include "memory/memory_allocator_impl.h"
#include "monitoring/perf_context_imp.h"
#include "options/cf_options.h"
#include "port/lang.h"
#include "port/malloc.h"
#include "port/port.h"  // noexcept
#include "rocksdb/cache.h"
#include "rocksdb/comparator.h"
#include "rocksdb/convenience.h"
#include "rocksdb/env.h"
#include "rocksdb/file_system.h"
#include "rocksdb/filter_policy.h"
#include "rocksdb/iterator.h"
#include "rocksdb/options.h"
#include "rocksdb/slice.h"
#include "rocksdb/snapshot.h"
#include "rocksdb/statistics.h"
#include "rocksdb/status.h"
#include "rocksdb/system_clock.h"
#include "rocksdb/table.h"
#include "rocksdb/table_properties.h"
#include "rocksdb/trace_record.h"
#include "table/block_based/binary_search_index_reader.h"
#include "table/block_based/block.h"
#include "table/block_based/block_based_table_factory.h"
#include "table/block_based/block_based_table_iterator.h"
#include "table/block_based/block_based_table_reader.cc"
#include "table/block_based/block_prefix_index.h"
#include "table/block_based/block_type.h"
#include "table/block_based/filter_block.h"
#include "table/block_based/filter_policy_internal.h"
#include "table/block_based/full_filter_block.h"
#include "table/block_based/hash_index_reader.h"
#include "table/block_based/partitioned_filter_block.h"
#include "table/block_based/partitioned_index_reader.h"
#include "table/block_fetcher.h"
#include "table/format.h"
#include "table/get_context.h"
#include "table/internal_iterator.h"
#include "table/meta_blocks.h"
#include "table/multiget_context.h"
#include "table/persistent_cache_helper.h"
#include "table/persistent_cache_options.h"
#include "table/sst_file_dumper.h"
#include "table/sst_file_writer_collectors.h"
#include "table/two_level_iterator.h"
#include "test_util/sync_point.h"
#include "util/coding.h"
#include "util/crc32c.h"
#include "util/hash.h"
#include "util/stop_watch.h"
#include "util/string_util.h"
using namespace ROCKSDB_NAMESPACE;
using namespace std;

struct DecodeEntry {
  inline const char* operator()(const char* p, const char* limit,
                                uint32_t* shared, uint32_t* non_shared,
                                uint32_t* value_length) {
    // We need 2 bytes for shared and non_shared size. We also need one more
    // byte either for value size or the actual value in case of value delta
    // encoding.
    assert(limit - p >= 3);
    *shared = reinterpret_cast<const unsigned char*>(p)[0];
    *non_shared = reinterpret_cast<const unsigned char*>(p)[1];
    *value_length = reinterpret_cast<const unsigned char*>(p)[2];
    if ((*shared | *non_shared | *value_length) < 128) {
      // Fast path: all three values are encoded in one byte each
      p += 3;
    } else {
      if ((p = GetVarint32Ptr(p, limit, shared)) == nullptr) return nullptr;
      if ((p = GetVarint32Ptr(p, limit, non_shared)) == nullptr) return nullptr;
      if ((p = GetVarint32Ptr(p, limit, value_length)) == nullptr) {
        return nullptr;
      }
    }

    // Using an assert in place of "return null" since we should not pay the
    // cost of checking for corruption on every single key decoding
    assert(!(static_cast<uint32_t>(limit - p) < (*non_shared + *value_length)));
    return p;
  }
};

int main() {
  // ingestion phrase
  std::vector<uint64_t> data;
  std::ifstream in("/home/weiping/code/SOSD/data/wiki_ts_200M_uint64",
                   std::ios::binary);
  uint64_t tuples;
  in.read(reinterpret_cast<char*>(&tuples), sizeof(string));
  data.resize(tuples);
  // Read values.
  in.read(reinterpret_cast<char*>(data.data()), tuples * sizeof(uint64_t));
  in.close();
  auto rng = std::default_random_engine{};
  tuples = tuples / 10;
  int PRIMARY_SIZE = 10;
  int SECONDARY_SIZE = 10;
  int VALUE_SIZE = 118;
  string db_r_path = "/tmp/wiki_128_R";
  string db_s_path = "/tmp/wiki_128_S";
  rocksdb::Options rocksdb_opt;
  rocksdb_opt.create_if_missing = true;
  rocksdb_opt.compression = kNoCompression;
  rocksdb_opt.bottommost_compression = kNoCompression;
  rocksdb::BlockBasedTableOptions table_options;
  rocksdb_opt.statistics = rocksdb::CreateDBStatistics();
  table_options.filter_policy.reset(NewBloomFilterPolicy(10, false));
  table_options.no_block_cache = true;
  rocksdb_opt.table_factory.reset(NewBlockBasedTableFactory(table_options));
  rocksdb::DestroyDB(db_r_path, rocksdb::Options());
  rocksdb::DestroyDB(db_s_path, rocksdb::Options());
  rocksdb::DB* db_r = nullptr;
  rocksdb::DB::Open(rocksdb_opt, db_r_path, &db_r);
  rocksdb::DB* db_s = nullptr;
  rocksdb::DB::Open(rocksdb_opt, db_s_path, &db_s);
  string tmp_key, tmp_value, secondary_key, secondary_value;
  cout << "ingesting " << tuples << " tuples with size "
       << PRIMARY_SIZE + VALUE_SIZE << "... " << endl;
  rocksdb::ReadOptions readOptions;

  uint64_t num_running_compactions, num_pending_compactions,
      num_running_flushes, num_pending_flushes;
  shuffle(data.begin(), data.end(), rng);
  auto ingest_time_start = std::chrono::high_resolution_clock::now();
  for (uint64_t i = 0; i < tuples; i++) {
    if (i % 10000000 == 0) {
      cout << i / 1000000 << " million" << endl;
    }
    tmp_key = to_string(i);
    tmp_value = to_string(data[i]);
    secondary_key =
        string(PRIMARY_SIZE - min(PRIMARY_SIZE, int(tmp_key.length())), '0') +
        tmp_key;
    secondary_value =
        string(SECONDARY_SIZE - min(SECONDARY_SIZE, int(tmp_value.length())),
               '0') +
        tmp_value + string(VALUE_SIZE - SECONDARY_SIZE, '0');
    db_r->Put(WriteOptions(), secondary_key, secondary_value);
  }
  db_r->Flush(FlushOptions());
  while (true) {
    db_r->GetIntProperty(DB::Properties::kNumRunningFlushes,
                         &num_running_flushes);
    db_r->GetIntProperty(DB::Properties::kMemTableFlushPending,
                         &num_pending_flushes);
    db_r->GetIntProperty(DB::Properties::kNumRunningCompactions,
                         &num_running_compactions);
    db_r->GetIntProperty(DB::Properties::kCompactionPending,
                         &num_pending_compactions);
    if (num_running_compactions == 0 && num_pending_compactions == 0 &&
        num_running_flushes == 0 && num_pending_flushes == 0)
      break;
  }
  auto ingest_time_end = chrono::high_resolution_clock::now();
  auto ingest_time1 = chrono::duration_cast<chrono::milliseconds>(
                          ingest_time_end - ingest_time_start)
                          .count() /
                      1000.0;
  shuffle(data.begin(), data.end(), rng);
  ingest_time_start = std::chrono::high_resolution_clock::now();
  for (uint64_t i = 0; i < tuples; i++) {
    if (i % 10000000 == 0) {
      cout << i / 1000000 << " million" << endl;
    }
    tmp_key = to_string(i);
    tmp_value = to_string(data[i]);
    secondary_key =
        string(PRIMARY_SIZE - min(PRIMARY_SIZE, int(tmp_key.length())), '0') +
        tmp_key;
    secondary_value =
        string(SECONDARY_SIZE - min(SECONDARY_SIZE, int(tmp_value.length())),
               '0') +
        tmp_value + string(VALUE_SIZE - SECONDARY_SIZE, '0');
    db_s->Put(WriteOptions(), secondary_key, secondary_value);
  }
  db_s->Flush(FlushOptions());
  while (true) {
    db_s->GetIntProperty(DB::Properties::kNumRunningFlushes,
                         &num_running_flushes);
    db_s->GetIntProperty(DB::Properties::kMemTableFlushPending,
                         &num_pending_flushes);
    db_s->GetIntProperty(DB::Properties::kNumRunningCompactions,
                         &num_running_compactions);
    db_s->GetIntProperty(DB::Properties::kCompactionPending,
                         &num_pending_compactions);
    if (num_running_compactions == 0 && num_pending_compactions == 0 &&
        num_running_flushes == 0 && num_pending_flushes == 0)
      break;
  }
  ingest_time_end = chrono::high_resolution_clock::now();
  auto ingest_time2 = chrono::duration_cast<chrono::milliseconds>(
                          ingest_time_end - ingest_time_start)
                          .count() /
                      1000.0;
  cout << "bloom filter" << endl;
  cout << "ingest_time: " << ingest_time1 + ingest_time1 << " (" << ingest_time1
       << "+" << ingest_time2 << ")" << endl;

  // index building phrase
  struct timespec t1, t2;
  Env* env = Env::Default();
  EnvOptions env_options;
  size_t size = 4096;
  Slice result;
  char* scratch = new char[size];
  uint32_t shared, non_shared, value_length;
  ParsedInternalKey parsed_key;
  string user_key, restart_key;
  int len_str;
  std::vector<std::vector<bloom_filter>> bloom_filter_all;
  // std::vector<std::vector<boost::unordered_set<string>>> bloom_filter_all;
  bloom_parameters parameters;
  // parameters.maximum_size = 400;
  parameters.projected_element_count = 31;
  parameters.maximum_number_of_hashes = 5;
  parameters.false_positive_probability = 0.00001;
  parameters.compute_optimal_parameters();
  std::vector<std::vector<std::pair<string, string>>> zone_map_all;
  std::vector<std::pair<string, string>> global_zone_map_all;
  std::vector<string> files;
  std::vector<std::vector<std::pair<int, int>>> index_all;
  // auto index_time_start = std::chrono::high_resolution_clock::now();
  // auto index_time_end = std::chrono::high_resolution_clock::now();
  double index_time = 0.0;
  std::unique_ptr<RandomAccessFile> file = nullptr;
  for (const auto& filePath : filesystem::directory_iterator(db_s_path)) {
    std::string filename = filePath.path().string();
    // string filename = "/tmp/wiki_128_R/000103.sst";
    if (filename.substr(filename.length() - 4) == ".sst") {
      files.emplace_back(filename);
      ROCKSDB_NAMESPACE::SstFileDumper dumper(
          Options(), filename, Temperature::kUnknown, 2 * 1024 * 1024, false,
          false, false);
      std::vector<std::pair<int, int>> index_sst;
      std::vector<std::vector<std::string>> data_block;
      dumper.DecodeBlock("out.txt", SECONDARY_SIZE, index_sst, data_block);
      env->NewRandomAccessFile(filename, &file, env_options);
      // vector<string> contains_keys;
      std::vector<bloom_filter> bloom_filter_sst;
      // std::vector<boost::unordered_set<string>> bloom_filter_sst;
      std::vector<std::pair<string, string>> zone_map_sst;
      string global_max_value = string(SECONDARY_SIZE, '0');
      string global_min_value = string(SECONDARY_SIZE, '9');
      for (const auto& index : index_sst) {
        size_t offset = index.first;
        size_t n = index.second;
        file->Read(offset, n, &result, scratch);
        size_t restart = 0;
        bloom_filter bloom_filter_block(parameters);
        // boost::unordered_set<string> bloom_filter_block;
        string max_value = string(SECONDARY_SIZE, '0');
        string min_value = string(SECONDARY_SIZE, '9');
        while (true) {
          const char* entry =
              DecodeEntry()(result.data() + restart, result.data() + n, &shared,
                            &non_shared, &value_length);
          Slice key(entry, non_shared), value(entry + non_shared, value_length);
          ParseInternalKey(key, &parsed_key, false);
          user_key = parsed_key.user_key.ToString();
          len_str = user_key.length();
          if (len_str < SECONDARY_SIZE) {
            restart_key.replace(restart_key.length() - len_str, len_str,
                                user_key);
          } else {
            restart_key = user_key;
          }
          // index_time_start = std::chrono::high_resolution_clock::now();
          clock_gettime(CLOCK_MONOTONIC, &t1);
          secondary_key = value.ToString().substr(0, SECONDARY_SIZE);
          if (secondary_key > max_value) {
            max_value = secondary_key;
          }
          if (secondary_key < min_value) {
            min_value = secondary_key;
          }
          if (secondary_key > global_max_value) {
            global_max_value = secondary_key;
          }
          if (secondary_key < global_min_value) {
            global_min_value = secondary_key;
          }
          bloom_filter_block.insert(secondary_key);
          clock_gettime(CLOCK_MONOTONIC, &t2);
          index_time += (t2.tv_sec - t1.tv_sec) +
                        (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
          // index_time_end = std::chrono::high_resolution_clock::now();
          // index_time += chrono::duration_cast<chrono::milliseconds>(
          //                   index_time_end - index_time_start)
          //                   .count() /
          //               1000.0;
          // contains_keys.emplace_back(value.ToString().substr(0, 10));
          // cout << restart_key << " " << value.ToString().substr(0, 10) <<
          // endl;

          restart = static_cast<uint32_t>((value.data() + value.size()) -
                                          result.data());
          if (restart + value.size() >= n) {
            break;
          }
        }
        bloom_filter_sst.emplace_back(bloom_filter_block);
        zone_map_sst.emplace_back(std::make_pair(min_value, max_value));
        // break;
      }
      bloom_filter_all.emplace_back(bloom_filter_sst);
      zone_map_all.emplace_back(zone_map_sst);
      index_all.emplace_back(index_sst);
      global_zone_map_all.emplace_back(
          std::make_pair(global_min_value, global_max_value));
    }
    // break;
  }
  cout << "index_time: " << index_time << endl;

  // cout << files.size() << endl;
  // cout << zone_map_all.size() << endl;
  // cout << bloom_filter_all.size() << endl;
  // joining phrase
  auto join_time_start = std::chrono::high_resolution_clock::now();
  ReadOptions read_options;
  rocksdb::Iterator* it_r = db_r->NewIterator(read_options);
  rocksdb::Iterator* it_s = db_s->NewIterator(read_options);

  string value;
  uint64_t matches = 0;
  vector<double> avg_time;
  // size_t offset, n;

  // int r = 0;
  for (it_r->SeekToFirst(); it_r->Valid(); it_r->Next()) {
    // if (matches % 100 == 0) {
    //   cout << matches << endl;
    // }
    // cout << r++ << endl;
    value = it_r->value().ToString().substr(0, SECONDARY_SIZE);
    clock_gettime(CLOCK_MONOTONIC, &t1);
    for (it_s->SeekToFirst(); it_s->Valid(); it_s->Next()) {
      if (it_s->value().ToString().substr(0, SECONDARY_SIZE) == value) {
        matches++;
      }
    }
    for (uint64_t i = 0; i < 1; i++) {
      if (value >= global_zone_map_all[i].first &&
          value <= global_zone_map_all[i].second) {
        for (uint64_t j = 0; j < zone_map_all[i].size(); j++) {
          if (value >= zone_map_all[i][j].first &&
              value <= zone_map_all[i][j].second) {
            if (bloom_filter_all[i][j].contains(value)) {
              // continue;
              // if (bloom_filter_all[i][j].find(value) !=
              //     bloom_filter_all[i][j].end()) {
              // cout << "filter_time: " << index_time << endl;
              size_t offset = index_all[i][j].first;
              size_t n = index_all[i][j].second;
              env->NewRandomAccessFile(files[i], &file, env_options);
              file->Read(offset, n, &result, scratch);
              size_t restart = 0;
              while (true) {
                const char* entry =
                    DecodeEntry()(result.data() + restart, result.data() + n,
                                  &shared, &non_shared, &value_length);
                Slice v(entry + non_shared, value_length);
                if (v.ToString().substr(0, SECONDARY_SIZE) == value) matches++;
                restart = static_cast<uint32_t>((v.data() + v.size()) -
                                                result.data());
                if (restart + value.size() >= n) break;
              }
              // cout << "decode_time: " << index_time << endl;
            }
          }
        }
      }
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
    index_time =
        (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
    avg_time.emplace_back(index_time);
    double sum = std::accumulate(std::begin(avg_time), std::end(avg_time), 0.0);
    double mean = sum / avg_time.size();
    cout << "iter_time: " << mean << endl;
  }
  auto join_time_end = std::chrono::high_resolution_clock::now();
  auto join_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                       join_time_end - join_time_start)
                       .count() /
                   1000.0;
  cout << "join_time: " << join_time << endl;
  cout << "matches: " << matches << endl;
  delete it_r;
  delete db_r;
  delete db_s;
}
