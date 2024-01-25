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
#include "external_hash_join.hpp"
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

bool is_regular_ingest_r = true;
bool is_regular_ingest_s = true;
bool is_covering = false;

void ProcessSettings(ExpConfig& config);
void Join(ExpConfig& config, ExpContext& context, RunResult& run_result);

// Driver code
int main(int argc, char* argv[]) {
  parseCommandLine(argc, argv);
  ExpConfig& config = ExpConfig::getInstance();
  ProcessSettings(config);
  ExpContext& context = ExpContext::getInstance();
  ExpResult& result = ExpResult::getInstance();
  context.InitDB();

  for (int i = 0; i < config.num_loop; i++) {
    cout << "-------------------------" << endl;
    cout << "loop: " << i << endl;
    cout << "-------------------------" << endl;
    config.this_loop = i;
    RunResult run_result = RunResult(i);

    vector<uint64_t> R, S, P;
    context.GenerateData(R, S, P);

    context.Ingest(R, S, P, is_regular_ingest_r, is_regular_ingest_s);
    run_result.index_build_time = context.BuildIndex(R, P, is_covering);
    Timer timer1 = Timer();

    Join(config, context, run_result);

    run_result.join_time = timer1.elapsed();
    run_result.join_read_io = get_perf_context()->block_read_count;

    result.AddRunResult(run_result);
    result.ShowRunResult(i);

    R.clear();
    S.clear();
    P.clear();
  }

  result.ShowExpResult();
  result.WriteResultToFile(config.output_file, config.ToString());

  context.db_r->Close();
  context.db_s->Close();
  delete context.db_r;
  delete context.db_s;
  // if index_r is not null_ptr
  if (context.index_r != nullptr) {
    context.index_r->Close();
    delete context.index_r;
  }
}

void Join(ExpConfig& config, ExpContext& context, RunResult& run_result) {
  if (config.join_algorithm == "INTJ") {
    NestedLoop(config, context, run_result);
  } else if (config.join_algorithm == "SJ") {
    if (config.r_index == "Regular") {
      ExternalSortMerge(config, context, run_result);
    } else if (config.r_index == "Eager" || config.r_index == "Lazy" ||
               config.r_index == "Comp") {
      SortMerge(config, context, run_result, false);
    } else if (config.r_index == "CEager" || config.r_index == "CLazy" ||
               config.r_index == "CComp") {
      SortMerge(config, context, run_result, true);
    }
  } else if (config.join_algorithm == "HJ") {
    HashJoin(config, context, run_result);
  }
}

void ProcessSettings(ExpConfig& config) {
  if (config.r_index == "CEager" || config.r_index == "CLazy" ||
      config.r_index == "CComp") {
    is_regular_ingest_r = false;
    is_covering = true;
  }
}