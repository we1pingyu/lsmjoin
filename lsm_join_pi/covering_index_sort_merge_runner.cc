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
  ExpResult& result = ExpResult::getInstance();
  context.InitDB();

  for (int i = 0; i < config.num_loop; i++) {
    cout << "-------------------------" << endl;
    cout << "loop: " << i << endl;
    cout << "-------------------------" << endl;
    config.this_loop = i;
    vector<uint64_t> R, S, P;
    context.GenerateData(R, S, P);
    context.Ingest(R, S, P, false, true);
    RunResult run_result = RunResult(i);
    run_result.index_build_time = context.BuildIndex(R, P, true);
    Timer timer1 = Timer();

    SortMerge(config, context, run_result, true);

    run_result.join_time = timer1.elapsed();
    run_result.join_read_io = get_perf_context()->block_read_count;

    result.AddRunResult(run_result);
    result.ShowRunResult(i);

    R.clear();
    S.clear();
    P.clear();
  }

  result.ShowExpResult();

  context.db_r->Close();
  context.db_s->Close();
  context.index_r->Close();
  delete context.db_r;
  delete context.db_s;
  delete context.index_r;
}
