#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

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
using namespace std;

int main(int argc, char* argv[]) {
  rocksdb::Options rocksdb_opt;
  rocksdb_opt.create_if_missing = true;
  rocksdb_opt.compression = kNoCompression;
  rocksdb_opt.bottommost_compression = kNoCompression;
  rocksdb::BlockBasedTableOptions table_options;
  table_options.filter_policy.reset(NewBloomFilterPolicy(10, false));
  table_options.no_block_cache = true;
  rocksdb_opt.table_factory.reset(NewBlockBasedTableFactory(table_options));
  rocksdb_opt.statistics = rocksdb::CreateDBStatistics();
  rocksdb::DB* db = nullptr;
  string db_path = "/tmp/test";
  rocksdb::DB::Open(rocksdb_opt, db_path, &db);
  int tuples = 100;
  string key, value, result;
  Status s;
  ReadOptions read_options;
  for (uint64_t i = 0; i < tuples; i++) {
    key = to_string(i);
    value = string(10, '0');
    s = db->Put(WriteOptions(), key, value);
    s = db->Get(read_options, to_string(i + 1), &result);
    cout << s.ToString() << endl;
  }
}