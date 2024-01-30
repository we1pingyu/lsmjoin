#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

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

  vector<uint64_t> ReadDatabase(string &file_path) {
    ifstream in(file_path, ios::binary);
    if (!in) {
      std::cerr << "Cannot open file.\n";
      return {};
    }
    uint64_t tuples;
    in.read(reinterpret_cast<char *>(&tuples), sizeof(uint64_t));
    tuples = min(static_cast<uint64_t>(10000000ULL), tuples);

    uint64_t part_size = tuples / config.num_loop;
    uint64_t last_part_size = tuples - (part_size * (config.num_loop - 1));

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
    cout << "Read part " << config.this_loop + 1 << " of " << config.num_loop
         << " with " << data.size() << " tuples" << endl;
    in.close();
    return data;
  }

  void InitDB() {
    rocksdb_opt.create_if_missing = true;
    rocksdb_opt.compression = kNoCompression;
    rocksdb_opt.bottommost_compression = kNoCompression;
    rocksdb_opt.statistics = rocksdb::CreateDBStatistics();
    table_options.filter_policy.reset(NewBloomFilterPolicy(10));
    table_options.no_block_cache = true;
    rocksdb_opt.table_factory.reset(NewBlockBasedTableFactory(table_options));
    if (config.ingestion) {
      rocksdb::DestroyDB(config.db_r, rocksdb::Options());
      rocksdb::DestroyDB(config.db_s, rocksdb::Options());
    }
    db_r = nullptr;
    db_s = nullptr;
    ptr_index_r = nullptr;
    ptr_index_s = nullptr;
    rocksdb::DB::Open(rocksdb_opt, config.db_r, &db_r);
    rocksdb::DB::Open(rocksdb_opt, config.db_s, &db_s);

    if (IsCompIndex(config.r_index) || IsCompIndex(config.s_index)) {
      table_options.filter_policy.reset(NewBloomFilterPolicy(10));
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
      R = ReadDatabase(config.public_r);
      config.r_tuples = R.size();
      S = ReadDatabase(config.public_s);
      config.s_tuples = S.size();
    } else {
      generateData(config.s_tuples, config.r_tuples, config.eps, config.k, S,
                   R);
    }

    generatePK(config.r_tuples, P, config.c);  // generate Primary keys for R
    // generatePK(config.s_tuples, SP, config.c);  // generate Primary keys for
    // S
    for (int i = 0; i < config.s_tuples; i++) {
      SP.push_back(i + config.this_loop * config.s_tuples);
    }
  }

  auto IngestS(vector<uint64_t> &S, vector<uint64_t> &P) {
    // shuffle(S.begin(), S.end(), rng);
    // ingestion phrase
    Timer timer1 = Timer();

    if (config.s_index == IndexType::Primary) {
      cout << "ingesting s " << config.s_tuples << " tuples with size "
           << config.PRIMARY_SIZE + config.VALUE_SIZE << "... " << endl;

      ingest_pk_data(config.s_tuples, db_s, S, config.VALUE_SIZE,
                     config.SECONDARY_SIZE, config.PRIMARY_SIZE);
    } else if (IsNonCoveringIndex(config.s_index) ||
               config.s_index == IndexType::Regular) {
      cout << "ingesting s " << config.s_tuples << " tuples with size "
           << config.PRIMARY_SIZE + config.VALUE_SIZE << "... " << endl;

      ingest_data(config.s_tuples, db_s, P, S, config.VALUE_SIZE,
                  config.SECONDARY_SIZE, config.PRIMARY_SIZE);
    }

    auto ingest_time1 = timer1.elapsed();
    return ingest_time1;
  }

  auto IngestR(vector<uint64_t> &R, vector<uint64_t> &P) {
    // shuffle(R.begin(), R.end(), rng);
    Timer timer1 = Timer();
    if (IsNonCoveringIndex(config.r_index) ||
        config.r_index == IndexType::Regular) {
      cout << "ingesting r " << config.r_tuples << " tuples with size "
           << config.PRIMARY_SIZE + config.VALUE_SIZE << "... " << endl;
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
    // cout << "ingest_time: " << ingest_time1 + ingest_time2 << " ("
    //      << ingest_time1 << "+" << ingest_time2 << ")" << endl;
  }

  double BuildNonCoveringIndex(vector<uint64_t> &data,
                               const vector<uint64_t> &primary, int tuples,
                               IndexType index_type, rocksdb::DB *db,
                               rocksdb::DB *index) {
    Timer timer1 = Timer();
    if (index_type == IndexType::Lazy)
      build_lazy_index(db, index, data.data(), primary, tuples,
                       config.VALUE_SIZE, config.SECONDARY_SIZE,
                       config.PRIMARY_SIZE);
    else if (index_type == IndexType::Eager)
      build_eager_index(db, index, data.data(), primary, tuples,
                        config.VALUE_SIZE, config.SECONDARY_SIZE,
                        config.PRIMARY_SIZE);
    else if (index_type == IndexType::Comp)
      build_composite_index(db, index, data.data(), primary, tuples,
                            config.VALUE_SIZE, config.SECONDARY_SIZE,
                            config.PRIMARY_SIZE);

    double index_build_time2 = timer1.elapsed();

    cout << IndexTypeToString(index_type) << endl;
    return index_build_time2;
  }

  auto BuildCoveringIndex(vector<uint64_t> &data,
                          const vector<uint64_t> &primary, int tuples,
                          IndexType index_type, rocksdb::DB *db,
                          rocksdb::DB *index) {
    // shuffle(data.begin(), data.end(), rng);

    int PRIMARY_SIZE = config.PRIMARY_SIZE,
        SECONDARY_SIZE = config.SECONDARY_SIZE, VALUE_SIZE = config.VALUE_SIZE;

    cout << "ingesting and building covering index " << tuples
         << " tuples with size " << PRIMARY_SIZE + VALUE_SIZE << "... " << endl;
    double ingest_time2 = 0.0;
    if (index_type == IndexType::CComp)
      ingest_time2 += build_covering_composite_index(
          db, index, data.data(), primary, tuples, VALUE_SIZE, SECONDARY_SIZE,
          PRIMARY_SIZE);
    else if (index_type == IndexType::CLazy)
      ingest_time2 +=
          build_covering_lazy_index(db, index, data.data(), primary, tuples,
                                    VALUE_SIZE, SECONDARY_SIZE, PRIMARY_SIZE);
    else if (index_type == IndexType::CEager)
      ingest_time2 +=
          build_covering_eager_index(db, index, data.data(), primary, tuples,
                                     VALUE_SIZE, SECONDARY_SIZE, PRIMARY_SIZE);

    cout << IndexTypeToString(index_type) << endl;
    return ingest_time2;
  }

  double BuildIndexForR(vector<uint64_t> &R, vector<uint64_t> &P) {
    if (config.this_loop == 0) {
      // build index
      rocksdb::DestroyDB(config.r_index_path, Options());
      rocksdb::DB::Open(rocksdb_opt, config.r_index_path, &ptr_index_r);
    }
    double index_build_time = 0.0;
    if (IsCoveringIndex(config.r_index)) {
      index_build_time = BuildCoveringIndex(R, P, config.r_tuples,
                                            config.r_index, db_r, ptr_index_r);
    } else if (IsNonCoveringIndex(config.r_index)) {
      index_build_time = BuildNonCoveringIndex(
          R, P, config.r_tuples, config.r_index, db_r, ptr_index_r);
    }
    return index_build_time;
  }

  double BuildIndexForS(vector<uint64_t> &S, vector<uint64_t> &P) {
    if (config.this_loop == 0) {
      rocksdb::DestroyDB(config.s_index_path, Options());
      rocksdb::DB::Open(rocksdb_opt, config.s_index_path, &ptr_index_s);
    }
    double index_build_time = 0.0;

    if (IsCoveringIndex(config.s_index)) {
      index_build_time = BuildCoveringIndex(S, P, config.s_tuples,
                                            config.s_index, db_s, ptr_index_s);
    } else if (IsNonCoveringIndex(config.s_index)) {
      index_build_time = BuildNonCoveringIndex(
          S, P, config.s_tuples, config.s_index, db_s, ptr_index_s);
    }
    return index_build_time;
  }

  // build index for R
  double BuildIndex(vector<uint64_t> &R, vector<uint64_t> &S,
                    vector<uint64_t> &P, vector<uint64_t> &SP) {
    if (config.this_loop == 0) {
      rocksdb_opt.write_buffer_size = (config.M - 3 * 4096) / 2;
      rocksdb_opt.max_bytes_for_level_base =
          rocksdb_opt.write_buffer_size *
          rocksdb_opt.max_bytes_for_level_multiplier;
    }

    double index_build_time1 = BuildIndexForR(R, P);
    double index_build_time2 = BuildIndexForS(S, SP);

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

 private:
  ExpContext()
      : config(ExpConfig::getInstance()), rng(std::default_random_engine{}) {}
};