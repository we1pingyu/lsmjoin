#include <sys/resource.h>

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
#include "external_radix_join.hpp"
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
  cout << "=========================" << endl;
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
              << " Files : " << level_str << " Size : " << level.size
              << std::endl;
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
    config.this_loop = i;
    RunResult run_result = RunResult(i);

    vector<uint64_t> R, S, P, SP;

    context.GenerateData(R, S, P, SP);

    if (!config.skip_ingestion) {
      context.Ingest(R, S, P, SP);
    }
    double sync_time = 0.0, eager_time = 0.0, update_time = 0.0,
           post_time = 0.0;
    if (IsIndex(config.r_index) || IsIndex(config.s_index)) {
      run_result.index_build_time = context.BuildIndex(
          R, S, P, SP, sync_time, eager_time, update_time, post_time);
    }

    Timer timer1 = Timer();
    std::map<std::string, uint64_t> stats;
    context.rocksdb_opt.statistics->Reset();
    rocksdb::get_iostats_context()->Reset();
    rocksdb::get_perf_context()->Reset();
    if (!config.skip_join) Join(config, context, run_result);
    context.rocksdb_opt.statistics->getTickerMap(&stats);
    double cache_hit_rate = stats["rocksdb.block.cache.hit"] == 0
                                ? 0
                                : double(stats["rocksdb.block.cache.hit"]) /
                                      double(stats["rocksdb.block.cache.hit"] +
                                             stats["rocksdb.block.cache.miss"]);
    run_result.cache_hit_rate = cache_hit_rate;
    double false_positive_rate = 0.0;
    if (IsCompIndex(config.r_index) || IsCompIndex(config.s_index)) {
      double false_positive =
          double(stats["rocksdb.last.level.seek.filter.match"] -
                 stats["rocksdb.last.level.seek.data.useful.filter.match"] +
                 stats["rocksdb.non.last.level.seek.filter.match"] -
                 stats["rocksdb.non.last.level.seek.data.useful.filter.match"]);
      double true_negative =
          double(stats["rocksdb.last.level.seek.filtered"] +
                 stats["rocksdb.non.last.level.seek.filtered"]);
      false_positive_rate = false_positive / (false_positive + true_negative);
    } else {
      double false_positive =
          double(stats["rocksdb.bloom.filter.full.positive"] -
                 stats["rocksdb.bloom.filter.full.true.positive"]);
      double true_negative = double(stats["rocksdb.bloom.filter.useful"]);
      false_positive_rate = false_positive / (false_positive + true_negative);
    }
    run_result.false_positive_rate = false_positive_rate;
    run_result.join_time = timer1.elapsed();
    cout << "After Join:" << endl;
    cout << "Page Cache:" << get_perf_context()->block_read_count << endl;
    run_result.join_read_io = get_iostats_context()->bytes_read / 4096;

    run_result.sync_time += sync_time;
    run_result.eager_time += eager_time;
    run_result.update_time += update_time;
    run_result.post_list_time += post_time;
    double join_cpu_time = run_result.join_time - (run_result.get_data_time +
                                                   run_result.get_index_time +
                                                   run_result.sort_io_time);
    cout << "Join CPU Time: " << join_cpu_time << endl;

    result.AddRunResult(run_result);
    result.ShowRunResult(i);

    R.clear();
    S.clear();
    P.clear();
  }

  result.ShowExpResult();
  result.WriteResultToFile(config.output_file, config.ToString());
  // print_db_status(context.db_r);
  // print_db_status(context.db_s);
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
  } else if (config.join_algorithm == JoinAlgorithm::RJ) {
    RadixJoin(config, context, run_result);
  }
}