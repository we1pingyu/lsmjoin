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
  uint64_t r_tuples = 1e7, s_tuples = 2e7; // TODO S的大小是R的2倍
  double eps = 0.9;
  int k = 2;
  int c = 2;
  int M = 64; // TODO MemTable size, 可能需要小一些
  int B = 32; // 每一个Block中Entry的个数
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
  generatePK(s_tuples, P, c); // P: primary keys
  generateData(r_tuples, s_tuples, eps, k, R, S);
  rocksdb::BlockBasedTableOptions table_options;
  rocksdb_opt.statistics = rocksdb::CreateDBStatistics();
  table_options.filter_policy.reset(NewBloomFilterPolicy(10));
  table_options.no_block_cache = true;
  rocksdb_opt.table_factory.reset(NewBlockBasedTableFactory(table_options));
  if (ingestion) {
    rocksdb::DestroyDB(db_r_path, rocksdb::Options());
    rocksdb::DestroyDB(db_s_path, rocksdb::Options());
  }
  rocksdb::DB *db_r = nullptr;
  rocksdb::DB::Open(rocksdb_opt, db_r_path, &db_r);
  rocksdb::DB *db_s = nullptr;
  rocksdb::DB::Open(rocksdb_opt, db_s_path, &db_s);
  rocksdb::DB *index_r = nullptr;
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
    ingest_pk_data(r_tuples, db_r, R, VALUE_SIZE, SECONDARY_SIZE, PRIMARY_SIZE);
  }
  clock_gettime(CLOCK_MONOTONIC, &t2);
  auto ingest_time1 =
      (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;

  string index_path = "/tmp/wiki_128_R_index";
  rocksdb::DestroyDB(index_path, rocksdb::Options());
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
  shuffle(S.begin(), S.end(), rng);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  if (ingestion) {
    cout << "ingesting s " << s_tuples << " tuples with size "
         << PRIMARY_SIZE + VALUE_SIZE << "... " << endl;
    ingest_data(s_tuples, db_s, P, S, VALUE_SIZE, SECONDARY_SIZE, PRIMARY_SIZE);
  }
  clock_gettime(CLOCK_MONOTONIC, &t2);
  auto ingest_time2 =
      (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;

  cout << "ingest_time: " << ingest_time1 + ingest_time2 << " (" << ingest_time1
       << "+" << ingest_time2 << ")" << endl;

  clock_gettime(CLOCK_MONOTONIC, &t1);
  ReadOptions read_options;
  auto memory_budget = 1000000;
  cout << "joining ... " << endl;
  rocksdb_opt.statistics->Reset();
  rocksdb::get_iostats_context()->Reset();
  rocksdb::get_perf_context()->Reset();
  rocksdb::Iterator *it_r = db_r->NewIterator(read_options);
  rocksdb::Iterator *it_s = db_s->NewIterator(read_options);
  Slice upper_bound_slice;
  uint64_t matches = 0;
  int r_num = 0;
  Status s;
  double valid_time = 0.0;
  int valid_count = 0;
  int total_io = 0;
  vector<int> avg_io;
  string tmp_s, value;

  // for (it_s->SeekToFirst(); it_s->Valid(); it_s->Next()) {
  //   tmp_s = it_s->value().ToString().substr(0, SECONDARY_SIZE);
  //   cout << tmp_s << endl;
  // }
  // cout << "========" << endl;
  // for (it_r->SeekToFirst(); it_r->Valid(); it_r->Next()) {
  //   tmp_s = it_r->key().ToString();
  //   cout << tmp_s << endl;
  // }

  // TODO Build Secondary Index?
  // TODO R left-join S, 遍历R找到S对应的值
  // tmp_s: secondary key, db_r: key-
  for (it_s->SeekToFirst(); it_s->Valid(); it_s->Next()) {
    tmp_s = it_s->value().ToString().substr(0, SECONDARY_SIZE);
    s = db_r->Get(read_options, tmp_s, &value);
    if (s.ok()) matches++;
  }
  cout << "matches: " << matches << endl;
  cout << "r_num: " << r_num << endl; // TODO no increase in r_num
  delete it_r;
  delete it_s;
  clock_gettime(CLOCK_MONOTONIC, &t2);
  auto join_time =
      (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
  cout << "join read io: " << get_perf_context()->block_read_count << endl;
  cout << "join_time: " << join_time << endl;
  // cout << "MTPS: " << double(tuples * 2) / join_time / 1000000.0 << endl;

  db_r->Close();
  db_s->Close();
  delete db_r;
  delete db_s;
  return 0;
}