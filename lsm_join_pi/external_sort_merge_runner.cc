// C++ program to implement
// external sorting using
// merge sort
#include <algorithm>
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

class StringAppendOperator : public rocksdb::AssociativeMergeOperator {
 public:
  char delim_;  // The delimiter is inserted between elements

  StringAppendOperator(char delim_char) : delim_(delim_char) {}

  virtual bool Merge(const Slice& key, const Slice* existing_value,
                     const Slice& value, std::string* new_value,
                     Logger* logger) const override {
    // Clear the *new_value for writing.
    assert(new_value);
    new_value->clear();

    if (!existing_value) {
      new_value->assign(value.data(), value.size());
    } else {
      new_value->reserve(existing_value->size() + 1 + value.size());
      new_value->assign(existing_value->data(), existing_value->size());
      new_value->append(1, delim_);
      new_value->append(value.data(), value.size());
    }
    return true;
  }

  virtual const char* Name() const override { return "StringAppendOperator"; }
};

int main(int argc, char* argv[]) {
  char junk;
  uint64_t n;
  double m;
  uint64_t r_tuples = 1e7, s_tuples = 2e7;
  vector<uint64_t> R, S, P;
  double eps = 0.9;
  int k = 2;
  int c = 2;
  int M = 64;
  int B = 32;
  bool ingestion = false;
  for (int i = 1; i < argc; i++) {
    if (sscanf(argv[i], "--r_tuples=%lf%c", &m, &junk) == 1) {
      r_tuples = m;
    }
    if (sscanf(argv[i], "--s_tuples=%lf%c", &m, &junk) == 1) {
      s_tuples = m;
    } else if (sscanf(argv[i], "--epsilon=%lf%c", &m, &junk) == 1) {
      eps = m;
    } else if (sscanf(argv[i], "--k=%lu%c", (unsigned long*)&n, &junk) == 1) {
      k = n;
    } else if (sscanf(argv[i], "--c=%lu%c", (unsigned long*)&n, &junk) == 1) {
      c = n;
    } else if (sscanf(argv[i], "--M=%lu%c", (unsigned long*)&n, &junk) == 1) {
      M = n;
    } else if (sscanf(argv[i], "--B=%lu%c", (unsigned long*)&n, &junk) == 1) {
      B = n;
    } else if (strcmp(argv[i], "--ingestion") == 0) {
      ingestion = true;
    }
  }
  cout << "r_tuples: " << r_tuples << endl;
  cout << "s_tuples: " << s_tuples << endl;
  cout << "epsilon: " << eps << endl;
  cout << "k: " << k << endl;
  cout << "c: " << c << endl;
  cout << "B: " << B << endl;
  cout << "M: " << M << "MB" << endl;
  M = n << 20;
  int PRIMARY_SIZE = 10;
  int SECONDARY_SIZE = 10;
  int VALUE_SIZE = 4096 / B - SECONDARY_SIZE;
  struct timespec t1, t2, t3;
  auto rng = std::default_random_engine{};
  generatePK(s_tuples, P, c);
  generateData(r_tuples, s_tuples, eps, k, R, S);

  string db_r_path = "/tmp/wiki_128_R";
  string db_s_path = "/tmp/wiki_128_S";
  rocksdb::Options rocksdb_opt;
  rocksdb_opt.create_if_missing = true;
  rocksdb_opt.compression = kNoCompression;
  rocksdb_opt.bottommost_compression = kNoCompression;
  // rocksdb_opt.use_direct_reads = true;
  // rocksdb_opt.use_direct_io_for_flush_and_compaction = true;
  rocksdb::BlockBasedTableOptions table_options;
  table_options.filter_policy.reset(NewBloomFilterPolicy(10, false));
  table_options.no_block_cache = true;
  rocksdb_opt.table_factory.reset(NewBlockBasedTableFactory(table_options));
  rocksdb_opt.statistics = rocksdb::CreateDBStatistics();
  if (ingestion) {
    rocksdb::DestroyDB(db_r_path, rocksdb::Options());
    rocksdb::DestroyDB(db_s_path, rocksdb::Options());
  }
  rocksdb::DB* db_r = nullptr;
  rocksdb::DB::Open(rocksdb_opt, db_r_path, &db_r);
  rocksdb::DB* db_s = nullptr;
  rocksdb::DB::Open(rocksdb_opt, db_s_path, &db_s);

  uint64_t random_key;
  string tmp_key, tmp_value;
  rocksdb::ReadOptions readOptions;
  string tmp;
  uint64_t num_running_compactions, num_pending_compactions,
      num_running_flushes, num_pending_flushes;
  std::map<std::string, uint64_t> stats;

  shuffle(R.begin(), R.end(), rng);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  if (ingestion) {
    cout << "ingesting " << r_tuples << " tuples with size "
         << PRIMARY_SIZE + VALUE_SIZE << "... " << endl;
    ingest_pk_data(r_tuples, db_r, R, VALUE_SIZE, SECONDARY_SIZE, PRIMARY_SIZE);
  }
  clock_gettime(CLOCK_MONOTONIC, &t2);
  auto ingest_time1 =
      (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;

  shuffle(S.begin(), S.end(), rng);
  clock_gettime(CLOCK_MONOTONIC, &t1);
  if (ingestion) {
    cout << "ingesting " << s_tuples << " tuples with size "
         << PRIMARY_SIZE + VALUE_SIZE << "... " << endl;
    ingest_data(s_tuples, db_s, P, S, VALUE_SIZE, SECONDARY_SIZE, PRIMARY_SIZE);
  }
  clock_gettime(CLOCK_MONOTONIC, &t2);
  auto ingest_time2 =
      (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
  cout << "hash join" << endl;
  cout << "ingest_time: " << ingest_time1 + ingest_time1 << " (" << ingest_time1
       << "+" << ingest_time2 << ")" << endl;

  cout << "Serializing data" << endl;
  clock_gettime(CLOCK_MONOTONIC, &t1);
  int run_size = int(M / (PRIMARY_SIZE + VALUE_SIZE)) - 1;
  // double run_size = 10;
  cout << "run_size: " << run_size << endl;
  string output_file_r = "/tmp/output_r.txt";
  string output_file_s = "/tmp/output_s.txt";
  ReadOptions read_options;
  int num_ways = s_tuples / run_size + 1;
  externalSort(db_s, output_file_s, num_ways, run_size, VALUE_SIZE,
               SECONDARY_SIZE);
  clock_gettime(CLOCK_MONOTONIC, &t2);
  auto sort_time =
      (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
  cout << "sort_time: " << sort_time << endl;

  int matches = 0;
  ifstream in_s(output_file_s);
  string line_r;
  string line_s;
  string temp_r;
  string temp_s;
  int count1 = 0, count2 = 1;
  char* buf1 = new char[4096];
  char* buf2 = new char[4096];
  in_s.rdbuf()->pubsetbuf(buf2, 4096);
  rocksdb::Iterator* it_r = db_r->NewIterator(read_options);
  it_r->SeekToFirst();
  if (!it_r->Valid() || !getline(in_s, line_s)) {
    cout << "error" << endl;
  };
  while (it_r->Valid() && line_s != "") {
    // std::istringstream iss_r(line_r);
    std::istringstream iss_s(line_s);
    std::string first_r, second_r, first_s, second_s;
    // cout << "line_r_start: " << line_r << endl;
    if (getline(iss_s, first_s, ',') && getline(iss_s, second_s)) {
      first_r = it_r->key().ToString();
      if (first_r == first_s) {
        temp_r = first_r;
        temp_s = first_s;
        // cout << "matches_before: " << matches << endl;
        count1++;
        while (getline(in_s, line_s)) {
          std::istringstream temp_iss_s(line_s);
          std::string temp_first_s, temp_second_s;
          if (getline(temp_iss_s, temp_first_s, ',') &&
              getline(temp_iss_s, temp_second_s)) {
            if (temp_first_s == temp_s) {
              // cout << "first_s: " << temp_first_s << endl;
              count2++;
              continue;
            } else {
              break;
            }
          }
        }
        matches += count1 * count2;
        // cout << "matches_after: " << matches << endl;
        count1 = 0;
        count2 = 1;
      }
      if (first_r > first_s) {
        if (!getline(in_s, line_s)) break;
      }
      if (first_r < first_s) {
        it_r->Next();
      }
    } else {
      break;
    }
    // cout << "line_r_end: " << line_r << endl;
  }
  cout << "matches: " << matches << endl;
  clock_gettime(CLOCK_MONOTONIC, &t2);
  auto sort_merge_time =
      (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
  cout << "sort_merge_time: " << sort_merge_time << endl;
  delete it_r;
  db_r->Close();
  db_s->Close();
  delete db_r;
  delete db_s;
}
