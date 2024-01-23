#include <algorithm>
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

tuple<int, double, double> NestedLoop(ExpConfig &config, ExpContext &context) {
  Timer timer = Timer();
  ReadOptions read_options;
  auto memory_budget = 1000000;
  cout << "joining ... " << endl;
  context.rocksdb_opt.statistics->Reset();
  rocksdb::get_iostats_context()->Reset();
  rocksdb::get_perf_context()->Reset();
  rocksdb::Iterator *it_r = context.db_r->NewIterator(read_options);
  rocksdb::Iterator *it_s = context.db_s->NewIterator(read_options);
  Slice upper_bound_slice;
  uint64_t matches = 0;
  Status status;
  double valid_time = 0.0;
  int valid_count = 0;
  int total_io = 0;
  vector<int> avg_io;
  string tmp_r, value;

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
  for (it_r->SeekToFirst(); it_r->Valid(); it_r->Next()) {
    tmp_r = it_r->value().ToString().substr(0, config.SECONDARY_SIZE);
    // cout << tmp_r << endl;
    status = context.db_s->Get(read_options, tmp_r, &value);
    if (status.ok()) matches++;
  }

  cout << "matches: " << matches << endl;
  delete it_r;
  delete it_s;
  auto join_time = timer.elapsed();
  cout << "join read io: " << get_perf_context()->block_read_count << endl;
  cout << "join_time: " << join_time << endl;
  // cout << "MTPS: " << double(tuples * 2) / join_time / 1000000.0 << endl;
  return make_tuple(matches, join_time, valid_time);
}

int main(int argc, char *argv[]) {
  parseCommandLine(argc, argv);
  ExpConfig &config = ExpConfig::getInstance();
  ExpContext &context = ExpContext::getInstance();
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

    auto [matches, val_time, get_time] = NestedLoop(config, context);

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