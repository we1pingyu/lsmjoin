#pragma once

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "boost/unordered_map.hpp"
#include "exp_config.hpp"
#include "exp_context.hpp"
#include "external_hash_join.hpp"
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

void HashJoin(ExpConfig& config, ExpContext& context, RunResult& run_result) {
  HASHJOIN::HashJoin(config, context, run_result);
}

void SortMerge(ExpConfig& config, ExpContext& context, RunResult& run_result,
               bool is_covering = false) {
  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE;

  cout << "Merging" << endl;

  string tmp;
  Timer timer1 = Timer();
  // double run_size = 10;

  rocksdb::Iterator* it_r = context.ptr_index_r->NewIterator(ReadOptions());
  rocksdb::Iterator* it_s = context.db_s->NewIterator(ReadOptions());
  int matches = 0, count1 = 0, count2 = 0;
  string temp_r_key, temp_r_value, temp_s_key, temp_s_value, value_r, value_s;
  std::vector<std::string> value_split;
  Status status;
  rocksdb::get_perf_context()->Reset();
  it_r->SeekToFirst();
  it_s->SeekToFirst();
  double val_time = 0.0, get_time = 0.0;
  if (IsLazyIndex(config.r_index) || IsEagerIndex(config.r_index)) {
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
        if (is_covering) {
          count2 += value_split.size();
        } else {
          std::sort(value_split.begin(), value_split.end());
          auto last = std::unique(value_split.begin(), value_split.end());
          value_split.erase(last, value_split.end());
          for (auto x : value_split) {
            status = context.db_r->Get(ReadOptions(), x.substr(0, PRIMARY_SIZE),
                                       &tmp);
            if (status.ok() && tmp.substr(0, SECONDARY_SIZE) == temp_r_key)
              count2++;
          }
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
  } else if (config.r_index == IndexType::Comp ||
             config.r_index == IndexType::CComp) {
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

        if (is_covering) {
          count2++;
        } else {
          // back to db_r to validate
          status = context.db_r->Get(ReadOptions(), temp_r_value, &value_r);
          if (status.ok() && value_r.substr(0, SECONDARY_SIZE) == temp_r_key)
            count2++;  // number of keys in S
        }

        tmp = temp_s_key;
        while (it_r->Valid()) {
          it_r->Next();
          if (!it_r->Valid()) break;
          temp_r_key = it_r->key().ToString().substr(0, SECONDARY_SIZE);
          temp_r_value =
              it_r->key().ToString().substr(SECONDARY_SIZE, PRIMARY_SIZE);
          if (temp_r_key == tmp) {
            if (is_covering) {
              count2++;
            } else {
              status = context.db_r->Get(ReadOptions(), temp_r_value, &value_r);
              if (status.ok() &&
                  value_r.substr(0, SECONDARY_SIZE) == temp_r_key)
                count2++;
            }

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

  run_result.matches = matches;
  run_result.val_time = val_time;
  run_result.get_time = get_time;

  delete it_r;
  delete it_s;

  return;
}

void ExternalSortMerge(ExpConfig& config, ExpContext& context,
                       RunResult& run_result) {
  cout << "external sort merge" << endl;

  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE, VALUE_SIZE = config.VALUE_SIZE;

  cout << "Serializing data" << endl;
  // Sort R
  Timer timer1 = Timer();
  int run_size = int(config.M / (PRIMARY_SIZE + VALUE_SIZE)) - 1;

  // double run_size = 10;
  string output_file_r = "/tmp/output_r.txt";
  string output_file_s = "/tmp/output_s.txt";
  ReadOptions read_options;
  int num_ways = config.r_tuples * (config.this_loop + 1) / run_size + 1;
  // NOTE: before sorting, R is already sorted
  MERGE::externalSort(context.db_r, output_file_r, num_ways, run_size,
                      VALUE_SIZE, SECONDARY_SIZE);

  run_result.sort_time = timer1.elapsed();

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
  run_result.matches = matches;

  delete it_s;
  return;
}

void NestedLoop(ExpConfig& config, ExpContext& context, RunResult& result) {
  Timer timer = Timer();
  ReadOptions read_options;
  auto memory_budget = 1000000;
  cout << "joining ... " << endl;
  context.rocksdb_opt.statistics->Reset();
  rocksdb::get_iostats_context()->Reset();
  rocksdb::get_perf_context()->Reset();
  rocksdb::Iterator* it_r = context.db_r->NewIterator(read_options);
  rocksdb::Iterator* it_s = context.db_s->NewIterator(read_options);
  Slice upper_bound_slice;
  uint64_t matches = 0;
  Status status;
  double valid_time = 0.0;
  int valid_count = 0;
  int total_io = 0;
  vector<int> avg_io;
  string tmp_r, value;

  // TODO R left-join S, 遍历R找到S对应的值
  // tmp_s: secondary key, db_r: key-
  for (it_r->SeekToFirst(); it_r->Valid(); it_r->Next()) {
    tmp_r = it_r->value().ToString().substr(0, config.SECONDARY_SIZE);
    // cout << tmp_r << endl;
    status = context.db_s->Get(read_options, tmp_r, &value);
    if (status.ok()) {
      matches++;
    }
  }

  result.matches = matches;

  delete it_r;
  delete it_s;
  return;
}

void IndexNestedLoop(ExpConfig& config, ExpContext& context, RunResult& result,
                     bool covering = true) {
  cout << "index nested loop joining..." << endl;
  string secondary_key_lower, secondary_key_upper, value, tmp_r, tmp_primary;
  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE, VALUE_SIZE = config.VALUE_SIZE;
  ReadOptions read_options;
  rocksdb::Iterator* it_r = context.db_r->NewIterator(read_options);
  rocksdb::Iterator* it_s = context.ptr_index_s->NewIterator(read_options);
  uint64_t matches = 0;  // number of matches
  Status s;
  string tmp;
  std::vector<std::string> value_split;
  struct timespec t1, t2;
  double val_time = 0.0, get_time = 0.0;

  if (IsEagerIndex(config.s_index) || IsLazyIndex(config.s_index)) {
    for (it_r->SeekToFirst(); it_r->Valid(); it_r->Next()) {
      Timer timer1 = Timer();
      tmp_r = it_r->value().ToString().substr(0, SECONDARY_SIZE);
      s = context.ptr_index_s->Get(read_options, tmp_r, &value);
      get_time += timer1.elapsed();
      if (s.ok()) {
        value_split = boost::split(value_split, value, boost::is_any_of(":"));
        std::set<std::string> value_set(value_split.begin(), value_split.end());
        if (covering) {
          for (auto x : value_set) matches++;
        } else {
          Timer timer = Timer();
          for (auto x : value_set) {
            s = context.db_s->Get(read_options, x.substr(0, PRIMARY_SIZE),
                                  &tmp);

            if (s.ok() && tmp.substr(0, SECONDARY_SIZE) == tmp_r) matches++;
          }
          val_time += timer.elapsed();
        }
      }
    }
  } else if (IsCompIndex(config.s_index)) {
    // it_r: primary key, value: secondary key
    for (it_r->SeekToFirst(); it_r->Valid(); it_r->Next()) {
      Timer timer1 = Timer();
      tmp_r = it_r->value().ToString().substr(0, SECONDARY_SIZE);
      get_time += timer1.elapsed();

      secondary_key_lower = tmp_r + string(PRIMARY_SIZE, '0');
      secondary_key_upper = tmp_r + string(PRIMARY_SIZE, '9');
      it_s->Seek(secondary_key_lower);  // TODO why seek?
      // it_s: secondary key+primary key
      for (; it_s->Valid() && it_s->key().ToString() <= secondary_key_upper;
           it_s->Next()) {
        timer1 = Timer();
        string tmp_s = it_s->key().ToString().substr(0, SECONDARY_SIZE);
        get_time += timer1.elapsed();

        if (tmp_s == tmp_r) {
          if (!covering) {
            Timer timer = Timer();
            s = context.db_s->Get(
                read_options,
                it_s->key().ToString().substr(SECONDARY_SIZE, PRIMARY_SIZE),
                &value);
            if (s.ok() && value.substr(0, SECONDARY_SIZE) == tmp_s) matches++;
            val_time += timer.elapsed();
          } else
            matches++;
        }
      }
    }
  }

  result.matches = matches;
  result.val_time = val_time;
  result.get_time = get_time;

  delete it_r;
  delete it_s;
}