#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "expconfig.hpp"
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

class Timer {
 public:
  Timer() { clock_gettime(CLOCK_MONOTONIC, &t1); }

  double elapsed() {
    clock_gettime(CLOCK_MONOTONIC, &t2);
    return (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
  }

 private:
  struct timespec t1, t2;
};

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

  void initDB() {
    string db_r_path = "/tmp/wiki_128_R";
    string db_s_path = "/tmp/wiki_128_S";
    rocksdb_opt.create_if_missing = true;
    rocksdb_opt.compression = kNoCompression;
    rocksdb_opt.bottommost_compression = kNoCompression;
    generatePK(config.s_tuples, P, config.c);  // P: primary keys
    generateData(config.r_tuples, config.s_tuples, config.eps, config.k, R, S);
    rocksdb_opt.statistics = rocksdb::CreateDBStatistics();
    table_options.filter_policy.reset(NewBloomFilterPolicy(10));
    table_options.no_block_cache = true;
    rocksdb_opt.table_factory.reset(NewBlockBasedTableFactory(table_options));
    if (config.ingestion) {
      rocksdb::DestroyDB(db_r_path, rocksdb::Options());
      rocksdb::DestroyDB(db_s_path, rocksdb::Options());
    }
    db_r = nullptr;
    db_s = nullptr;
    index_r = nullptr;
    rocksdb::DB::Open(rocksdb_opt, db_r_path, &db_r);
    rocksdb::DB::Open(rocksdb_opt, db_s_path, &db_s);

    shuffle(R.begin(), R.end(), rng);
  }

  void ingest() {
    // ingestion phrase
    Timer timer1 = Timer();
    if (config.ingestion) {
      cout << "ingesting r " << config.r_tuples << " tuples with size "
           << config.PRIMARY_SIZE + config.VALUE_SIZE << "... " << endl;
      ingest_pk_data(config.r_tuples, db_r, R, config.VALUE_SIZE,
                     config.SECONDARY_SIZE, config.PRIMARY_SIZE);
    }
    auto ingest_time1 = timer1.elapsed();

    string index_path = "/tmp/wiki_128_R_index";
    rocksdb::DestroyDB(index_path, rocksdb::Options());
    if (config.index_type == "comp") {
      table_options.filter_policy.reset(NewBloomFilterPolicy(10));
      table_options.whole_key_filtering = false;
      rocksdb_opt.table_factory.reset(NewBlockBasedTableFactory(table_options));
      rocksdb_opt.prefix_extractor.reset(
          NewCappedPrefixTransform(config.SECONDARY_SIZE));
    }
    if (config.index_type == "lazy") {
      rocksdb_opt.merge_operator.reset(new StringAppendOperator(':'));
    }
    shuffle(S.begin(), S.end(), rng);
    timer1 = Timer();
    if (config.ingestion) {
      cout << "ingesting s " << config.s_tuples << " tuples with size "
           << config.PRIMARY_SIZE + config.VALUE_SIZE << "... " << endl;
      ingest_data(config.s_tuples, db_s, P, S, config.VALUE_SIZE,
                  config.SECONDARY_SIZE, config.PRIMARY_SIZE);
    }
    auto ingest_time2 = timer1.elapsed();

    cout << "ingest_time: " << ingest_time1 + ingest_time2 << " ("
         << ingest_time1 << "+" << ingest_time2 << ")" << endl;
  }

  // build index for R
  void build_index() {
    rocksdb_opt.write_buffer_size = (config.M - 3 * 4096) / 2;
    rocksdb_opt.max_bytes_for_level_base =
        rocksdb_opt.write_buffer_size *
        rocksdb_opt.max_bytes_for_level_multiplier;

    // build index
    auto index_path = "/tmp/wiki_128_R_index";
    rocksdb::DestroyDB(index_path, Options());
    rocksdb::DB::Open(rocksdb_opt, index_path, &index_r);
    Timer timer1 = Timer();
    if (config.index_type == "lazy")
      build_lazy_index(db_r, index_r, R.data(), P, config.r_tuples,
                       config.VALUE_SIZE, config.SECONDARY_SIZE,
                       config.PRIMARY_SIZE);
    else if (config.index_type == "eager")
      build_eager_index(db_r, index_r, R.data(), P, config.r_tuples,
                        config.VALUE_SIZE, config.SECONDARY_SIZE,
                        config.PRIMARY_SIZE);
    else
      build_composite_index(db_r, index_r, R.data(), P, config.r_tuples,
                            config.VALUE_SIZE, config.SECONDARY_SIZE,
                            config.PRIMARY_SIZE);
    auto index_build_time2 = timer1.elapsed();

    cout << config.index_type << endl;
    // cout << "index_build_time: " << index_build_time1 + index_build_time2
    //      << " (" << index_build_time1 << "+" << index_build_time2 << ")"
    //      << endl;
    // TODO: index_build_time1 is not used

    cout << "index_build_time: " << index_build_time2 << endl;

    // vector<uint64_t>().swap(S);
  }

  // forbid copy and move
  ExpContext(ExpContext const &) = delete;
  void operator=(ExpContext const &) = delete;

  // config parameters
  vector<uint64_t> R, S,
      P;  // R: left relation, S: right relation, P: primary keys
  rocksdb::DB *db_r;
  rocksdb::DB *db_s;
  rocksdb::Options rocksdb_opt;
  rocksdb::BlockBasedTableOptions table_options;
  std::default_random_engine rng;
  ExpConfig &config;
  rocksdb::DB *index_r;

 private:
  ExpContext()
      : config(ExpConfig::getInstance()), rng(std::default_random_engine{}) {}
};