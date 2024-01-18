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
  context.initDB();
  context.ingest();
  context.build_index();

  cout << "Merging" << endl;
  string tmp;
  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE;

  Timer timer1 = Timer();
  // double run_size = 10;

  rocksdb::Iterator* it_r = context.index_r->NewIterator(ReadOptions());
  rocksdb::Iterator* it_s = context.db_s->NewIterator(ReadOptions());
  int matches = 0, count1 = 0, count2 = 0;
  string temp_r_key, temp_r_value, temp_s_key, temp_s_value, value_r, value_s;
  std::vector<std::string> value_split;
  Status status;
  rocksdb::get_perf_context()->Reset();
  it_r->SeekToFirst();
  it_s->SeekToFirst();
  double val_time = 0.0, get_time = 0.0;
  if (config.index_type == "lazy" || config.index_type == "eager") {
    while (it_r->Valid() && it_s->Valid()) {
      Timer timer2 = Timer();
      temp_r_key = it_r->key().ToString();
      temp_s_key = it_s->key().ToString();
      temp_r_value = it_r->value().ToString();
      temp_s_value = it_s->value().ToString();
      get_time += timer2.elapsed();
      if (temp_r_key == temp_s_key) {
        timer2 = Timer();
        count1++;
        value_split =
            boost::split(value_split, temp_r_value, boost::is_any_of(":"));
        // back to db_r to validate
        // TODO: Why is this necessary? Shouldn't the index be correct?
        for (auto x : value_split) {
          status =
              context.db_r->Get(ReadOptions(), x.substr(0, PRIMARY_SIZE), &tmp);
          if (status.ok() && tmp.substr(0, SECONDARY_SIZE) == temp_r_key)
            count2++;
        }
        matches += count1 * count2;
        val_time += timer2.elapsed();
        count1 = 0;
        count2 = 0;
        it_r->Next();
        it_s->Next();
      } else if (temp_r_key < temp_s_key)
        it_r->Next();
      else if (temp_r_key > temp_s_key)
        it_s->Next();
    }
  } else {
    cout << "composite" << endl;
    while (it_r->Valid() && it_s->Valid()) {
      // NOTE: Add timer here
      Timer timer2 = Timer();
      temp_r_key = it_r->key().ToString().substr(0, SECONDARY_SIZE);
      temp_s_key = it_s->key().ToString();
      temp_r_value =
          it_r->key().ToString().substr(SECONDARY_SIZE, PRIMARY_SIZE);
      temp_s_value =
          it_s->key().ToString().substr(SECONDARY_SIZE, PRIMARY_SIZE);
      get_time += timer2.elapsed();
      // cout << temp_r_key << " " << temp_s_key << endl;
      if (temp_r_key == temp_s_key) {
        Timer timer2 = Timer();
        count1++;  // number of keys in R
        // back to db_r to validate
        status = context.db_r->Get(ReadOptions(), temp_r_value, &value_r);
        if (status.ok() && value_r.substr(0, SECONDARY_SIZE) == temp_r_key)
          count2++;  // number of keys in S

        tmp = temp_s_key;
        while (it_r->Valid()) {
          it_r->Next();  // TODO: PeekNext()?
          if (!it_r->Valid()) break;
          temp_r_key = it_r->key().ToString().substr(0, SECONDARY_SIZE);
          temp_r_value =
              it_r->key().ToString().substr(SECONDARY_SIZE, PRIMARY_SIZE);
          if (temp_r_key == tmp) {
            status = context.db_r->Get(ReadOptions(), temp_r_value, &value_r);
            if (status.ok() && value_r.substr(0, SECONDARY_SIZE) == temp_r_key)
              count2++;
          } else
            break;
        }
        matches += count1 * count2;
        val_time += timer2.elapsed();
        count1 = 0;
        count2 = 0;
      } else if (temp_r_key < temp_s_key)
        it_r->Next();
      else if (temp_r_key > temp_s_key)
        it_s->Next();
    }
  }

  cout << "join read io: " << get_perf_context()->block_read_count << endl;
  cout << "matches: " << matches << endl;
  cout << "val_time: " << val_time << endl;
  cout << "get_time: " << get_time << endl;

  auto sort_merge_time = timer1.elapsed();
  cout << "sort_merge_time: " << sort_merge_time << endl;

  delete it_r;
  delete it_s;
  context.db_r->Close();
  context.db_s->Close();
  context.index_r->Close();
  delete context.db_r;
  delete context.db_s;
  delete context.index_r;
}
