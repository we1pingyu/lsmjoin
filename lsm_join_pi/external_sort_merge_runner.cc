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

#include "exp_utils.hpp"
#include "expconfig.hpp"
#include "expcontext.hpp"
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

tuple<int, double, double> ExternalSortMerge(ExpConfig& config,
                                             ExpContext& context) {
  cout << "external sort merge" << endl;

  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE, VALUE_SIZE = config.VALUE_SIZE;

  cout << "Serializing data" << endl;
  // Sort R
  Timer timer1 = Timer();
  int run_size = int(config.M / (PRIMARY_SIZE + VALUE_SIZE)) - 1;

  // double run_size = 10;
  cout << "run_size: " << run_size << endl;
  string output_file_r = "/tmp/output_r.txt";
  string output_file_s = "/tmp/output_s.txt";
  ReadOptions read_options;
  int num_ways = config.r_tuples / run_size + 1;
  // NOTE: before sorting, R is already sorted
  externalSort(context.db_r, output_file_r, num_ways, run_size, VALUE_SIZE,
               SECONDARY_SIZE);

  auto sort_time = timer1.elapsed();
  cout << "sort_time: " << sort_time << endl;

  int matches = 0;
  ifstream in_r(output_file_r);  // File on which sorting needs to be applied
  string line_r;
  string line_s;
  string temp_r;
  string temp_s;
  int count1 = 0, count2 = 1;
  char* buf1 = new char[4096];
  char* buf2 = new char[4096];
  in_r.rdbuf()->pubsetbuf(buf2, 4096);
  rocksdb::Iterator* it_s = context.db_s->NewIterator(read_options);
  it_s->SeekToFirst();
  if (!it_s->Valid() || !getline(in_r, line_r)) {
    cout << "error" << endl;
  };
  while (it_s->Valid() && line_r != "") {
    std::istringstream iss_r(line_r);  // string stream for line_r
    std::string first_r, second_r, first_s, second_s;
    // cout << "line_r_start: " << line_r << endl;
    if (getline(iss_r, first_r, ',') && getline(iss_r, second_r)) {
      first_s = it_s->key().ToString();
      if (first_r == first_s) {
        temp_r = first_r;
        temp_s = first_s;
        // cout << "matches_before: " << matches << endl;
        count1++;
        while (getline(in_r, line_r)) {  // read next line_r
          std::istringstream temp_iss_r(line_r);
          std::string temp_first_r, temp_second_r;
          if (getline(temp_iss_r, temp_first_r, ',') &&
              getline(temp_iss_r, temp_second_r)) {
            if (temp_first_r == temp_r) {
              cout << "first_r: " << temp_first_r << endl;
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
      if (first_s > first_r) {
        if (!getline(in_r, line_r)) break;
      }
      if (first_s < first_r) {
        it_s->Next();
      }
    } else {
      break;
    }
    // cout << "line_r_end: " << line_r << endl;
  }
  cout << "matches: " << matches << endl;
  auto sort_merge_time = timer1.elapsed();
  cout << "sort_merge_time: " << sort_merge_time << endl;
  delete it_s;

  return make_tuple(matches, sort_time, sort_merge_time);
}

int main(int argc, char* argv[]) {
  parseCommandLine(argc, argv);
  ExpConfig& config = ExpConfig::getInstance();
  ExpContext& context = ExpContext::getInstance();
  context.InitDB();
  uint64_t sum_join_read_io = 0;
  double sum_val_time = 0, sum_get_time = 0, sum_sort_merge_time = 0;

  for (int i = 0; i < config.num_loop; i++) {
    cout << "-------------------------" << endl;
    cout << "loop: " << i << endl;
    cout << "-------------------------" << endl;
    config.this_loop = i;
    vector<uint64_t> R, S, P;
    context.GenerateData(R, S, P);
    context.Ingest(R, S, P, true, true);

    Timer timer1 = Timer();

    auto [matches, val_time, get_time] = ExternalSortMerge(config, context);

    uint64_t join_read_io = get_perf_context()->block_read_count;
    cout << "join read io: " << join_read_io << endl;
    cout << "matches: " << matches << endl;
    cout << "val_time: " << val_time << endl;
    cout << "get_time: " << get_time << endl;
    auto sort_merge_time = timer1.elapsed();
    cout << "sort_merge_time: " << sort_merge_time << endl;

    sum_join_read_io += join_read_io;
    sum_val_time += val_time;
    sum_get_time += get_time;
    sum_sort_merge_time += sort_merge_time;

    R.clear();
    S.clear();
    P.clear();
  }

  cout << "-------------------------" << endl;
  cout << "sum_join_read_io: " << sum_join_read_io << endl;
  cout << "sum_val_time: " << sum_val_time << endl;
  cout << "sum_get_time: " << sum_get_time << endl;
  cout << "sum_sort_merge_time: " << sum_sort_merge_time << endl;
  cout << "-------------------------" << endl;

  context.db_r->Close();
  context.db_s->Close();
  delete context.db_r;
  delete context.db_s;
}
