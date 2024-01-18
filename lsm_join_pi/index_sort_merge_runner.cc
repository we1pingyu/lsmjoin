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
  context.Ingest();
  context.BuildIndex();

  Timer timer1 = Timer();

  auto [matches, val_time, get_time] = SortMerge(config, context, false);

  cout << "join read io: " << get_perf_context()->block_read_count << endl;
  cout << "matches: " << matches << endl;
  cout << "val_time: " << val_time << endl;
  cout << "get_time: " << get_time << endl;

  auto sort_merge_time = timer1.elapsed();
  cout << "sort_merge_time: " << sort_merge_time << endl;
}
