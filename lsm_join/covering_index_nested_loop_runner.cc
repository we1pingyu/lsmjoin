#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

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
using namespace std;
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
int main(int argc, char *argv[]) {
  char junk;
  uint64_t n;
  double m;
  string index_type;
  vector<uint64_t> R, S, P;
  uint64_t r_tuples = 1e7, s_tuples = 2e7;
  double eps = 0.9;
  int k = 2;
  int c = 1;
  int M = 64;
  int B = 32;
  bool ingestion = false;
  for (int i = 1; i < argc; i++) {
    if (strncmp(argv[i], "--index=", 8) == 0) {
      index_type = argv[i] + 8;
    } else if (sscanf(argv[i], "--s_tuples=%lf%c", &m, &junk) == 1) {
      s_tuples = m;
    } else if (sscanf(argv[i], "--r_tuples=%lf%c", &m, &junk) == 1) {
      r_tuples = m;
    } else if (sscanf(argv[i], "--epsilon=%lf%c", &m, &junk) == 1) {
      eps = m;
    } else if (sscanf(argv[i], "--k=%lu%c", (unsigned long *)&n, &junk) == 1) {
      k = n;
    } else if (sscanf(argv[i], "--c=%lu%c", (unsigned long *)&n, &junk) == 1) {
      c = n;
    } else if (sscanf(argv[i], "--M=%lu%c", (unsigned long *)&n, &junk) == 1) {
      M = n;
    } else if (sscanf(argv[i], "--B=%lu%c", (unsigned long *)&n, &junk) == 1) {
      B = n;
    } else if (strcmp(argv[i], "--ingestion") == 0) {
      ingestion = true;
    }
  }

  cout << "index_type: " << index_type << endl;
  cout << "r_tuples: " << r_tuples << endl;
  cout << "s_tuples: " << s_tuples << endl;
  cout << "epsilon: " << eps << endl;
  cout << "k: " << k << endl;
  cout << "c: " << c << endl;
  cout << "B: " << B << endl;
  cout << "M: " << M << "MB" << endl;
  M = n << 20;

  struct timespec t1, t2;
  auto rng = std::default_random_engine{};

  string db_r_path = "/tmp/wiki_128_R";
  string db_s_path = "/tmp/wiki_128_S";
  rocksdb::Options rocksdb_opt;
  rocksdb_opt.create_if_missing = true;
  rocksdb_opt.compression = kNoCompression;
  rocksdb_opt.bottommost_compression = kNoCompression;
  // rocksdb_opt.use_direct_reads = true;
  // rocksdb_opt.use_direct_io_for_flush_and_compaction = true;
  generatePK(s_tuples, P, c);
  generateData(r_tuples, s_tuples, eps, k, R, S);
  rocksdb::BlockBasedTableOptions table_options;
  rocksdb_opt.statistics = rocksdb::CreateDBStatistics();
  table_options.filter_policy.reset(NewBloomFilterPolicy(10));
  table_options.no_block_cache = true;
  rocksdb_opt.table_factory.reset(NewBlockBasedTableFactory(table_options));
  if (ingestion) {
    rocksdb::DestroyDB(db_r_path, rocksdb::Options());
  }
  rocksdb::DestroyDB(db_s_path, rocksdb::Options());
  rocksdb::DB *db_r = nullptr;
  rocksdb::DB::Open(rocksdb_opt, db_r_path, &db_r);
  rocksdb::DB *db_s = nullptr;
  rocksdb::DB::Open(rocksdb_opt, db_s_path, &db_s);
  rocksdb::DB *index_r = nullptr;
  rocksdb::DB *index_s = nullptr;
  int PRIMARY_SIZE = 10;
  int SECONDARY_SIZE = 10;
  int VALUE_SIZE = 4096 / B - SECONDARY_SIZE;
  string tmp_key, tmp_value;

  rocksdb::ReadOptions readOptions;
  string tmp;

  shuffle(R.begin(), R.end(), rng);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  if (ingestion) {
    cout << "ingesting r " << r_tuples << " tuples with size "
         << PRIMARY_SIZE + VALUE_SIZE << "... " << endl;
    ingest_data(r_tuples, db_r, vector<uint64_t>(), R, VALUE_SIZE,
                SECONDARY_SIZE, PRIMARY_SIZE);
  }
  clock_gettime(CLOCK_MONOTONIC, &t2);
  auto ingest_time1 =
      (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;

  if (index_type == "comp") {
    table_options.filter_policy.reset(NewBloomFilterPolicy(10));
    table_options.whole_key_filtering = false;
    rocksdb_opt.table_factory.reset(NewBlockBasedTableFactory(table_options));
    rocksdb_opt.prefix_extractor.reset(
        NewCappedPrefixTransform(SECONDARY_SIZE));
  }
  if (index_type == "lazy") {
    rocksdb_opt.merge_operator.reset(new StringAppendOperator(':'));
  }
  rocksdb_opt.write_buffer_size = M - 3 * 4096;
  rocksdb_opt.max_bytes_for_level_base =
      rocksdb_opt.write_buffer_size *
      rocksdb_opt.max_bytes_for_level_multiplier;
  shuffle(S.begin(), S.end(), rng);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  string index_path = "/tmp/wiki_128_S_index";
  rocksdb::DestroyDB(index_path, rocksdb::Options());
  rocksdb::DB::Open(rocksdb_opt, index_path, &index_s);
  cout << "ingesting and building covering index s " << s_tuples
       << " tuples with size " << PRIMARY_SIZE + VALUE_SIZE << "... " << endl;
  if (index_type == "comp")
    build_covering_composite_index(db_s, index_s, S.data(), P, s_tuples,
                                   VALUE_SIZE, SECONDARY_SIZE, PRIMARY_SIZE);
  else if (index_type == "lazy")
    build_covering_lazy_index(db_s, index_s, S.data(), P, s_tuples, VALUE_SIZE,
                              SECONDARY_SIZE, PRIMARY_SIZE);
  else
    build_covering_eager_index(db_s, index_s, S.data(), P, s_tuples, VALUE_SIZE,
                               SECONDARY_SIZE, PRIMARY_SIZE);
  clock_gettime(CLOCK_MONOTONIC, &t2);
  auto ingest_time2 =
      (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;

  cout << index_type << endl;
  cout << "total_ingest_time: " << ingest_time1 + ingest_time1 << " ("
       << ingest_time1 << "+" << ingest_time2 << ")" << endl;

  clock_gettime(CLOCK_MONOTONIC, &t1);
  cout << "joining ... " << endl;
  rocksdb_opt.statistics->Reset();
  rocksdb::get_iostats_context()->Reset();
  rocksdb::get_perf_context()->Reset();
  if (index_type == "lazy")
    lazy_index_nested_loop(index_r, index_s, db_r, db_s, SECONDARY_SIZE,
                           PRIMARY_SIZE, false);
  else if (index_type == "eager")
    eager_index_nested_loop(index_r, index_s, db_r, db_s, SECONDARY_SIZE,
                            PRIMARY_SIZE, false);
  else
    composite_index_nested_loop(index_r, index_s, db_r, db_s, SECONDARY_SIZE,
                                PRIMARY_SIZE, false);
  clock_gettime(CLOCK_MONOTONIC, &t2);
  auto join_time =
      (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
  cout << "join read io: " << get_perf_context()->block_read_count << endl;
  cout << "join_time: " << join_time << endl;
  db_r->Close();
  db_s->Close();
  delete db_r;
  delete db_s;
  return 0;
}