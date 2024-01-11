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

class ExpContext {
 public:
  static ExpContext &getInstance() {
    static ExpContext instance;
    return instance;
  }

  void initDB() {
    struct timespec t1, t2;

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
    rocksdb::DB::Open(rocksdb_opt, db_r_path, &db_r);
    rocksdb::DB::Open(rocksdb_opt, db_s_path, &db_s);
    rocksdb::DB *index_r = nullptr;

    string tmp_key, tmp_value;

    rocksdb::ReadOptions readOptions;
    string tmp;

    shuffle(R.begin(), R.end(), rng);
  }

  void ingest() {
    struct timespec t1, t2;

    // ingestion phrase
    clock_gettime(CLOCK_MONOTONIC, &t1);
    if (config.ingestion) {
      cout << "ingesting r " << config.r_tuples << " tuples with size "
           << config.PRIMARY_SIZE + config.VALUE_SIZE << "... " << endl;
      ingest_pk_data(config.r_tuples, db_r, R, config.VALUE_SIZE,
                     config.SECONDARY_SIZE, config.PRIMARY_SIZE);
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
    auto ingest_time1 =
        (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;

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
    clock_gettime(CLOCK_MONOTONIC, &t1);
    if (config.ingestion) {
      cout << "ingesting s " << config.s_tuples << " tuples with size "
           << config.PRIMARY_SIZE + config.VALUE_SIZE << "... " << endl;
      ingest_data(config.s_tuples, db_s, P, S, config.VALUE_SIZE,
                  config.SECONDARY_SIZE, config.PRIMARY_SIZE);
    }
    clock_gettime(CLOCK_MONOTONIC, &t2);
    auto ingest_time2 =
        (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;

    cout << "ingest_time: " << ingest_time1 + ingest_time2 << " ("
         << ingest_time1 << "+" << ingest_time2 << ")" << endl;
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

 private:
  ExpContext()
      : config(ExpConfig::getInstance()), rng(std::default_random_engine{}) {}
};