// C++ program to implement
// external sorting using
// merge sort
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "expconfig.hpp"
#include "expcontext.hpp"
#include "index.hpp"
#include "join_algorithms.hpp"
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

// Driver code
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
    context.Ingest(R, S, P);
    context.BuildIndex(R, P);

    Timer timer1 = Timer();

    auto [matches, val_time, get_time] = SortMerge(config, context, false);

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
  return 0;
}
