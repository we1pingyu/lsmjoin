#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "exp_config.hpp"
#include "exp_context.hpp"
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

void Join(ExpConfig& config, ExpContext& context, RunResult& run_result);

void print_db_status(rocksdb::DB* db) {
  rocksdb::ColumnFamilyMetaData cf_meta;
  db->GetColumnFamilyMetaData(&cf_meta);

  std::vector<std::string> file_names;
  int level_idx = 1;
  for (auto& level : cf_meta.levels) {
    std::string level_str = "";
    for (auto& file : level.files) {
      level_str += file.name + ", ";
    }
    level_str =
        level_str == "" ? "EMPTY" : level_str.substr(0, level_str.size() - 2);
    std::cout << "Level " << level_idx << " : " << level.files.size()
              << " Files : " << level_str << std::endl;
    level_idx++;
  }
}

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
    RunResult run_result = RunResult(i);

    vector<uint64_t> R, S, P, SP;

    context.GenerateData(R, S, P, SP);

    if (config.ingestion) {
      context.Ingest(R, S, P, SP);
    }

    if (IsIndex(config.r_index) || IsIndex(config.s_index)) {
      run_result.index_build_time = context.BuildIndex(R, S, P, SP);
    }

    // // r.value = s.value
    // // show all r
    // ReadOptions read_options;
    // rocksdb::Iterator* it_r = context.db_r->NewIterator(read_options);
    // for (it_r->SeekToFirst(); it_r->Valid(); it_r->Next()) {
    //   cout << "key: " << it_r->key().ToString()
    //        << ", value: " << it_r->value().ToString() << endl;
    // }

    // show all s
    // rocksdb::Iterator* it_ss = context.db_s->NewIterator(ReadOptions());
    // for (it_ss->SeekToFirst(); it_ss->Valid(); it_ss->Next()) {
    //   cout << "key: " << it_ss->key().ToString()
    //        << ", value: " << it_ss->value().ToString() << endl;
    // }

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
  if (context.ptr_index_r != nullptr) {
    print_db_status(context.ptr_index_r);
    context.ptr_index_r->Close();
    delete context.ptr_index_r;
  }
  // if index_s is not null_ptr
  if (context.ptr_index_s != nullptr) {
    print_db_status(context.ptr_index_s);
    context.ptr_index_s->Close();
    delete context.ptr_index_s;
  }
}

void Join(ExpConfig& config, ExpContext& context, RunResult& run_result) {
  if (config.join_algorithm == JoinAlgorithm::INLJ) {
    if (config.s_index == IndexType::Primary) {
      NestedLoop(config, context, run_result);
    } else {
      IndexNestedLoop(config, context, run_result,
                      IsCoveringIndex(config.s_index));
    }

  } else if (config.join_algorithm == JoinAlgorithm::SJ) {
    if (config.r_index == IndexType::Regular) {
      if (config.s_index == IndexType::Regular) {
        NonIndexExternalSortMerge(config, context, run_result);
      } else {
        rocksdb::Iterator* it_s;
        if (IsIndex(config.s_index)) {
          DebugPrint("S is indexed");
          it_s = context.ptr_index_s->NewIterator(ReadOptions());
        } else {
          DebugPrint("S is not indexed");
          it_s = context.db_s->NewIterator(ReadOptions());
        }
        SingleIndexExternalSortMerge(config, context, run_result, it_s);
      }
    } else {
      rocksdb::Iterator *it_r, *it_s;
      if (IsIndex(config.r_index)) {
        DebugPrint("R is indexed");
        it_r = context.ptr_index_r->NewIterator(ReadOptions());
      } else {
        DebugPrint("R is not indexed");
        it_r = context.db_r->NewIterator(ReadOptions());
      }
      if (IsIndex(config.s_index)) {
        DebugPrint("S is indexed");
        it_s = context.ptr_index_s->NewIterator(ReadOptions());
      } else {
        DebugPrint("S is not indexed");
        it_s = context.db_s->NewIterator(ReadOptions());
      }
      SortMerge(config, context, run_result, it_r, it_s);
    }
  } else if (config.join_algorithm == JoinAlgorithm::HJ) {
    HashJoin(config, context, run_result);
  }
}