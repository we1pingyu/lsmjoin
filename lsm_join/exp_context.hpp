#pragma once

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "compaction.hpp"
#include "exp_config.hpp"
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

using namespace ROCKSDB_NAMESPACE;

class StringAppendOperator : public rocksdb::AssociativeMergeOperator {
 public:
  char delim_;  // The delimiter is inserted between elements

  // Constructor: also specify the delimiter character.
  StringAppendOperator(char delim_char) : delim_(delim_char) {}

  virtual bool Merge(const Slice &key, const Slice *existing_value,
                     const Slice &value, std::string *new_value,
                     Logger *logger) const override {
    // Clear the *new_value for writing.
    assert(new_value);
    new_value->clear();

    if (!existing_value) {
      // No existing_value. Set *new_value = value
      new_value->assign(value.data(), value.size());
    } else {
      // Generic append (existing_value != null).
      // Reserve *new_value to correct size, and apply concatenation.
      new_value->reserve(existing_value->size() + 1 + value.size());
      new_value->assign(existing_value->data(), existing_value->size());
      new_value->append(1, delim_);
      new_value->append(value.data(), value.size());
    }
    return true;
  }

  virtual const char *Name() const override { return "StringAppendOperator"; }
};

class ExpContext {
 public:
  static ExpContext &getInstance() {
    static ExpContext instance;
    return instance;
  }

  void WaitCompaction(DB *db, Compactor *compactor, bool theory = false) {
    if (theory) {
      // db->Flush(FlushOptions());
      while (compactor->compactions_left_count > 0);
      // while (compactor->requires_compaction(db)) {
      //   while (compactor->compactions_left_count > 0)
      //     ;
      // }
    } else {
      // db->Flush(FlushOptions());
      uint64_t num_running_flushes, num_pending_flushes,
          num_running_compactions, num_pending_compactions;
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
  }

  vector<uint64_t> ReadDatabase(string &file_path,
                                uint64_t records = 10000000ULL) {
    cout << file_path << endl;
    ifstream in(file_path, ios::binary);
    if (!in) {
      std::cerr << "Cannot open file.\n";
      return {};
    }
    uint64_t tuples;
    in.read(reinterpret_cast<char *>(&tuples), sizeof(uint64_t));
    tuples = min(records, tuples);

    uint64_t part_size = tuples;
    uint64_t last_part_size = part_size;

    std::vector<uint64_t> data;
    data.resize(part_size);
    if (config.this_loop == config.num_loop - 1) {
      data.resize(last_part_size);
    }
    in.seekg(sizeof(uint64_t) * part_size * config.this_loop + sizeof(uint64_t),
             ios::beg);
    in.read(reinterpret_cast<char *>(data.data()),
            sizeof(uint64_t) * data.size());
    uint64_t modulus = static_cast<uint64_t>(std::pow(10, config.PRIMARY_SIZE));
    for (uint64_t &num : data) {
      num %= modulus;
    }
    // shuffle(data.begin(), data.end(), rng);
    cout << "Read part " << config.this_loop + 1 << " of " << config.num_loop
         << " with " << data.size() << " tuples" << endl;
    in.close();
    return data;
  }

  void SetCompaction(rocksdb::Options &rocksdb_opt, Compactor *&compactor,
                     size_t tuples, bool is_covering_index = true) {
    CompactorOptions compactor_opt;
    rocksdb_opt.compaction_style = rocksdb::kCompactionStyleNone;
    // rocksdb_opt.max_bytes_for_level_multiplier = config.T;
    rocksdb_opt.disable_auto_compactions = true;
    rocksdb_opt.write_buffer_size = config.M / 2 - 3 * 4096;
    compactor_opt.K = config.K;
    compactor_opt.size_ratio = config.T;
    compactor_opt.buffer_size = config.M / 2 - 3 * 4096;

    rocksdb_opt.max_background_jobs = 1;
    // rocksdb_opt.target_file_size_multiplier = config.T;
    if (is_covering_index)
      compactor_opt.entry_size = 4096 / config.B;
    else
      compactor_opt.entry_size = config.PRIMARY_SIZE + config.SECONDARY_SIZE;
    compactor_opt.bits_per_element = 10;
    compactor_opt.num_entries = tuples;
    compactor_opt.levels =
        Compactor::estimate_levels(
            compactor_opt.num_entries, compactor_opt.size_ratio,
            compactor_opt.entry_size, compactor_opt.buffer_size) *
        (config.K - 1);
    rocksdb_opt.num_levels = int(compactor_opt.levels);
    rocksdb_opt.target_file_size_base = 1 * 1024 * 1024;
    // rocksdb_opt.write_buffer_size = rocksdb_opt.target_file_size_base;
    // cout << "target_file_size_base: " << rocksdb_opt.target_file_size_base
    //      << endl;
    compactor = new Compactor(compactor_opt, rocksdb_opt);
    rocksdb_opt.listeners.emplace_back(compactor);
    // rocksdb_opt.target_file_size_multiplier = config.T;
  }

  void InitDB() {
    rocksdb_opt.compression = rocksdb::kNoCompression;
    rocksdb_opt.bottommost_compression = kNoCompression;
    rocksdb_opt.level_compaction_dynamic_level_bytes = false;
    rocksdb_opt.max_bytes_for_level_multiplier = config.T;
    rocksdb_opt.max_open_files = 512;
    rocksdb_opt.advise_random_on_open = false;
    rocksdb_opt.random_access_max_buffer_size = 0;
    rocksdb_opt.avoid_unnecessary_blocking_io = true;
    rocksdb_opt.create_if_missing = true;
    if (config.direct_io) {
      rocksdb_opt.use_direct_reads = true;
      rocksdb_opt.use_direct_io_for_flush_and_compaction = true;
    }
    rocksdb_opt.statistics = rocksdb::CreateDBStatistics();
    table_options.filter_policy.reset(NewBloomFilterPolicy(10));
    if (config.cache_size != 0) {
      table_options.block_cache = rocksdb::NewLRUCache(config.cache_size);
      cout << "block_cache_size: " << config.cache_size << endl;
    } else
      table_options.no_block_cache = true;
    // table_options.block_size = config.page_size;
    rocksdb_opt.table_factory.reset(NewBlockBasedTableFactory(table_options));
    if (!config.skip_ingestion) {
      rocksdb::DestroyDB(config.db_r, rocksdb_opt);
      rocksdb::DestroyDB(config.db_s, rocksdb_opt);
    }
    db_r = nullptr;
    db_s = nullptr;
    ptr_index_r = nullptr;
    ptr_index_s = nullptr;
    compactor_r = nullptr;
    compactor_s = nullptr;
    compactor_index_r = nullptr;
    compactor_index_s = nullptr;
    rocksdb::DB::Open(rocksdb_opt, config.db_r, &db_r);
    rocksdb::DB::Open(rocksdb_opt, config.db_s, &db_s);
    table_options.filter_policy.reset(NewBloomFilterPolicy(config.bpk));
    rocksdb_opt.table_factory.reset(NewBlockBasedTableFactory(table_options));
    if (IsCompIndex(config.r_index) || IsCompIndex(config.s_index)) {
      table_options.filter_policy.reset(NewBloomFilterPolicy(config.bpk));
      table_options.whole_key_filtering = false;
      rocksdb_opt.table_factory.reset(NewBlockBasedTableFactory(table_options));
      rocksdb_opt.prefix_extractor.reset(
          NewCappedPrefixTransform(config.SECONDARY_SIZE));
    }
    if (IsLazyIndex(config.r_index) || IsLazyIndex(config.s_index)) {
      rocksdb_opt.merge_operator.reset(new StringAppendOperator(':'));
    }
  }

  void GenerateData(vector<uint64_t> &R, vector<uint64_t> &S,
                    vector<uint64_t> &P, vector<uint64_t> &SP) {
    if (config.is_public_data) {
      R = ReadDatabase(config.public_r, config.r_tuples);
      config.r_tuples = R.size();
      S = ReadDatabase(config.public_s, config.s_tuples);
      config.s_tuples = S.size();
    } else {
      generateData(config.s_tuples, config.r_tuples, config.eps_s, config.k_r,
                   config.k_s, S, R, config.skew);
    }

    generatePK(config.r_tuples, P, config.c_r);  // generate Primary keys for R
    // generatePK(config.s_tuples, SP, config.c);  // generate Primary keys
    // for
    // S
    if (config.c_s == 1)
      for (int i = 0; i < config.s_tuples; i++) {
        SP.push_back(i + config.this_loop * config.s_tuples);
      }
    else {
      generatePK(config.s_tuples, SP, config.c_s);
    }
  }

  auto IngestS(vector<uint64_t> &S, vector<uint64_t> &P) {
    // shuffle(S.begin(), S.end(), rng);
    // ingestion phrase
    Timer timer1 = Timer();

    cout << "ingesting s " << config.s_tuples << " tuples with size "
         << config.PRIMARY_SIZE + config.VALUE_SIZE << "... " << endl;

    if (config.s_index == IndexType::Primary) {
      ingest_pk_data(config.s_tuples, db_s, S, config.VALUE_SIZE,
                     config.SECONDARY_SIZE, config.PRIMARY_SIZE);
    } else if (IsNonCoveringIndex(config.s_index) ||
               config.s_index == IndexType::Regular) {
      ingest_data(config.s_tuples, db_s, P, S, config.VALUE_SIZE,
                  config.SECONDARY_SIZE, config.PRIMARY_SIZE);
    }
    auto ingest_time1 = timer1.elapsed();
    return ingest_time1;
  }

  auto IngestR(vector<uint64_t> &R, vector<uint64_t> &P) {
    // shuffle(R.begin(), R.end(), rng);
    Timer timer1 = Timer();
    cout << "ingesting r " << config.r_tuples << " tuples with size "
         << config.PRIMARY_SIZE + config.VALUE_SIZE << "... " << endl;
    if (IsNonCoveringIndex(config.r_index) ||
        config.r_index == IndexType::Regular) {
      ingest_data(config.r_tuples, db_r, P, R, config.VALUE_SIZE,
                  config.SECONDARY_SIZE, config.PRIMARY_SIZE);
    }
    auto ingest_time2 = timer1.elapsed();
    return ingest_time2;
  }

  void Ingest(vector<uint64_t> &R, vector<uint64_t> &S, vector<uint64_t> &P,
              vector<uint64_t> &SP) {
    double ingest_time1 = 0.0, ingest_time2 = 0.0;
    ingest_time1 = IngestS(S, SP);

    ingest_time2 = IngestR(R, P);
    cout << "ingest_time: " << ingest_time1 + ingest_time2 << " ("
         << ingest_time1 << "+" << ingest_time2 << ")" << endl;
  }

  double BuildNonCoveringIndex(vector<uint64_t> &data,
                               const vector<uint64_t> &primary, int tuples,
                               IndexType index_type, rocksdb::DB *db,
                               rocksdb::DB *index, double &eager_time,
                               double &update_time, double &post_time) {
    Timer timer1 = Timer();
    if (index_type == IndexType::Lazy)
      build_lazy_index(db, index, data.data(), primary, tuples,
                       config.VALUE_SIZE, config.SECONDARY_SIZE,
                       config.PRIMARY_SIZE, update_time);
    else if (index_type == IndexType::Eager)
      build_eager_index(db, index, data.data(), primary, tuples,
                        config.VALUE_SIZE, config.SECONDARY_SIZE,
                        config.PRIMARY_SIZE, update_time, eager_time);
    else if (index_type == IndexType::Comp)
      build_composite_index(db, index, data.data(), primary, tuples,
                            config.VALUE_SIZE, config.SECONDARY_SIZE,
                            config.PRIMARY_SIZE, update_time);

    double index_build_time2 = timer1.elapsed();

    cout << IndexTypeToString(index_type) << endl;
    return index_build_time2;
  }

  auto BuildCoveringIndex(vector<uint64_t> &data,
                          const vector<uint64_t> &primary, int tuples,
                          IndexType index_type, rocksdb::DB *db,
                          rocksdb::DB *index, double &sync_time,
                          double &eager_time, double &update_time,
                          double &post_time) {
    // shuffle(data.begin(), data.end(), rng);

    int PRIMARY_SIZE = config.PRIMARY_SIZE,
        SECONDARY_SIZE = config.SECONDARY_SIZE, VALUE_SIZE = config.VALUE_SIZE;

    cout << "ingesting and building covering index " << tuples
         << " tuples with size " << PRIMARY_SIZE + VALUE_SIZE << "... " << endl;
    Timer timer1 = Timer();
    double db_ingest_time = 0.0;
    if (index_type == IndexType::CComp)
      db_ingest_time += build_covering_composite_index(
          db, index, data.data(), primary, tuples, VALUE_SIZE, SECONDARY_SIZE,
          PRIMARY_SIZE, sync_time, update_time);
    else if (index_type == IndexType::CLazy)
      db_ingest_time += build_covering_lazy_index(
          db, index, data.data(), primary, tuples, VALUE_SIZE, SECONDARY_SIZE,
          PRIMARY_SIZE, sync_time, update_time, post_time);
    else if (index_type == IndexType::CEager)
      db_ingest_time += build_covering_eager_index(
          db, index, data.data(), primary, tuples, VALUE_SIZE, SECONDARY_SIZE,
          PRIMARY_SIZE, sync_time, update_time, post_time, eager_time);

    cout << IndexTypeToString(index_type) << endl;
    double ingest_time2 = timer1.elapsed() - db_ingest_time;
    return ingest_time2;
  }

  double BuildIndexForR(vector<uint64_t> &R, vector<uint64_t> &P,
                        double &sync_time, double &eager_time,
                        double &update_time, double &post_time) {
    if (config.this_loop == 0) {
      // build index
      rocksdb::DestroyDB(config.r_index_path, Options());
      if (config.theory && IsIndex(config.r_index)) {
        SetCompaction(rocksdb_opt, compactor_index_r, config.r_tuples,
                      IsCoveringIndex(config.r_index));
      }
      rocksdb::DB::Open(rocksdb_opt, config.r_index_path, &ptr_index_r);
    }
    double index_build_time = 0.0;
    if (IsCoveringIndex(config.r_index)) {
      index_build_time = BuildCoveringIndex(
          R, P, config.r_tuples, config.r_index, db_r, ptr_index_r, sync_time,
          eager_time, update_time, post_time);
    } else if (IsNonCoveringIndex(config.r_index)) {
      index_build_time = BuildNonCoveringIndex(
          R, P, config.r_tuples, config.r_index, db_r, ptr_index_r, eager_time,
          update_time, post_time);
    }
    Timer timer1 = Timer();
    if (IsIndex(config.r_index))
      WaitCompaction(ptr_index_r, compactor_index_r, config.theory);
    update_time += timer1.elapsed();
    double timer2 = timer1.elapsed();
    return index_build_time + timer2;
  }

  double BuildIndexForS(vector<uint64_t> &S, vector<uint64_t> &P,
                        double &sync_time, double &eager_time,
                        double &update_time, double &post_time) {
    if (config.this_loop == 0) {
      rocksdb::DestroyDB(config.s_index_path, Options());
      if (config.theory && IsIndex(config.s_index)) {
        SetCompaction(rocksdb_opt, compactor_index_s, config.s_tuples,
                      IsCoveringIndex(config.s_index));
      }
      rocksdb::DB::Open(rocksdb_opt, config.s_index_path, &ptr_index_s);
    }
    double index_build_time = 0.0;

    if (IsCoveringIndex(config.s_index)) {
      index_build_time = BuildCoveringIndex(
          S, P, config.s_tuples, config.s_index, db_s, ptr_index_s, sync_time,
          eager_time, update_time, post_time);
    } else if (IsNonCoveringIndex(config.s_index)) {
      index_build_time = BuildNonCoveringIndex(
          S, P, config.s_tuples, config.s_index, db_s, ptr_index_s, eager_time,
          update_time, post_time);
    }
    Timer timer1 = Timer();
    if (IsIndex(config.s_index))
      WaitCompaction(ptr_index_s, compactor_index_s, config.theory);
    update_time += timer1.elapsed();
    double timer2 = timer1.elapsed();
    return index_build_time + timer2;
  }

  double BuildIndex(vector<uint64_t> &R, vector<uint64_t> &S,
                    vector<uint64_t> &P, vector<uint64_t> &SP,
                    double &sync_time, double &eager_time, double &update_time,
                    double &post_time) {
    if (config.this_loop == 0) {
      rocksdb_opt.write_buffer_size = (config.M - 3 * 4096) / 2;
      rocksdb_opt.max_bytes_for_level_base =
          rocksdb_opt.write_buffer_size *
          rocksdb_opt.max_bytes_for_level_multiplier;
    }

    double index_build_time1 =
        BuildIndexForR(R, P, sync_time, eager_time, update_time, post_time);

    double io_time_R = sync_time + eager_time + update_time;
    double index_build_cpu_time_R = index_build_time1 - io_time_R;

    double index_build_time2 =
        BuildIndexForS(S, SP, sync_time, eager_time, update_time, post_time);

    // becase sync_time also includes the time for building index R, we need to
    // subtract it
    double io_time_S = sync_time + eager_time + update_time - io_time_R;
    double index_build_cpu_time_S = index_build_time2 - io_time_S;

    cout << "index_build_cpu_time_R: " << index_build_cpu_time_R << endl;
    cout << "index_build_cpu_time_S: " << index_build_cpu_time_S << endl;

    cout << "index_build_time: " << index_build_time1 + index_build_time2
         << endl;
    return index_build_time1 + index_build_time2;
  }

  // forbid copy and move
  ExpContext(ExpContext const &) = delete;
  void operator=(ExpContext const &) = delete;

  // // config parameters
  // vector<uint64_t> R, S,
  //     P;  // R: left relation, S: right relation, P: primary keys
  rocksdb::DB *db_r;
  rocksdb::DB *db_s;
  rocksdb::Options rocksdb_opt;
  rocksdb::BlockBasedTableOptions table_options;
  std::default_random_engine rng;
  ExpConfig &config;
  rocksdb::DB *ptr_index_r;
  rocksdb::DB *ptr_index_s;
  Compactor *compactor_r;
  Compactor *compactor_s;
  Compactor *compactor_index_r;
  Compactor *compactor_index_s;

 private:
  ExpContext()
      : config(ExpConfig::getInstance()),
        rng(std::default_random_engine{123}) {}
};