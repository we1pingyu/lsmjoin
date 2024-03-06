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

void SortMergeForEagerLazy(ExpConfig& config, ExpContext& context,
                           RunResult& run_result, rocksdb::Iterator* it_r,
                           rocksdb::Iterator* it_s, int& matches) {
  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE;
  string temp_r_key, temp_r_value, temp_s_key, temp_s_value, value_r, value_s;
  std::vector<std::string> value_split;
  Status s;
  string tmp;
  int count1 = 0, count2 = 0;
  Timer timer = Timer();
  double data_time = 0.0, index_time = 0.0;
  while (it_r->Valid() && it_s->Valid()) {
    Timer timer2 = Timer();
    temp_r_key = it_r->key().ToString();
    temp_s_key = it_s->key().ToString();
    temp_r_value = it_r->value().ToString();
    temp_s_value = it_s->value().ToString();

    if (temp_r_key == temp_s_key) {
      timer2 = Timer();
      value_split =
          boost::split(value_split, temp_r_value, boost::is_any_of(":"));
      if (IsCoveringIndex(config.r_index) || !IsIndex(config.r_index)) {
        count1 += value_split.size();
      } else {
        std::sort(value_split.begin(), value_split.end());
        auto last = std::unique(value_split.begin(), value_split.end());
        value_split.erase(last, value_split.end());
        for (auto x : value_split) {
          timer = Timer();
          s = context.db_r->Get(ReadOptions(), x.substr(0, PRIMARY_SIZE), &tmp);
          data_time += timer.elapsed();
          if (s.ok() && tmp.substr(0, SECONDARY_SIZE) == temp_r_key) count1++;
        }
      }

      value_split =
          boost::split(value_split, temp_s_value, boost::is_any_of(":"));
      if (IsCoveringIndex(config.s_index) || !IsIndex(config.s_index)) {
        // if covering or no index, no need to validate
        count2 += value_split.size();
      } else {
        std::sort(value_split.begin(), value_split.end());
        auto last = std::unique(value_split.begin(), value_split.end());
        value_split.erase(last, value_split.end());

        for (auto x : value_split) {
          timer = Timer();
          s = context.db_s->Get(ReadOptions(), x.substr(0, PRIMARY_SIZE), &tmp);
          data_time += timer.elapsed();
          if (s.ok() && tmp.substr(0, SECONDARY_SIZE) == temp_s_key) count2++;
        }
      }

      matches += count1 * count2;
      count1 = 0;
      count2 = 0;
      timer = Timer();
      it_r->Next();
      it_s->Next();
      index_time += timer.elapsed();
    } else if (temp_r_key < temp_s_key) {
      timer = Timer();
      it_r->Next();
      index_time += timer.elapsed();
    } else if (temp_r_key > temp_s_key) {
      timer = Timer();
      it_s->Next();
      index_time += timer.elapsed();
    }
  }
  run_result.get_data_time += data_time;
  run_result.get_index_time += index_time;
}

void SortMergeForComp(ExpConfig& config, ExpContext& context,
                      RunResult& run_result, rocksdb::Iterator* it_r,
                      rocksdb::Iterator* it_s, int& matches) {
  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE;
  string temp_r_key, temp_r_value, temp_s_key, temp_s_value, value_r, value_s;
  std::vector<std::string> value_split;
  Status status;
  string tmp;
  int count1 = 0, count2 = 0;
  double data_time = 0.0, index_time = 0.0;
  Timer timer1 = Timer();
  while (it_r->Valid() && it_s->Valid()) {
    temp_r_key = it_r->key().ToString().substr(0, SECONDARY_SIZE);
    temp_s_key = it_s->key().ToString().substr(0, SECONDARY_SIZE);
    temp_r_value = it_r->key().ToString().substr(SECONDARY_SIZE, PRIMARY_SIZE);
    temp_s_value = it_s->key().ToString().substr(SECONDARY_SIZE, PRIMARY_SIZE);
    if (temp_r_key == temp_s_key) {
      if (IsCoveringIndex(config.r_index) || !IsIndex(config.r_index)) {
        count1++;  // number of keys in R
      } else {
        timer1 = Timer();
        status = context.db_r->Get(ReadOptions(), temp_r_value, &value_r);
        data_time += timer1.elapsed();
        if (status.ok() && value_r.substr(0, SECONDARY_SIZE) == temp_r_key)
          count1++;  // number of keys in R
      }

      if (IsCoveringIndex(config.s_index) || !IsIndex(config.s_index)) {
        count2++;  // number of keys in S
      } else {
        // get s
        timer1 = Timer();
        status = context.db_s->Get(ReadOptions(), temp_s_value, &value_s);
        data_time += timer1.elapsed();
        if (status.ok() && value_s.substr(0, SECONDARY_SIZE) == temp_s_key)
          count2++;  // number of keys in S
      }

      if (IsIndex(config.r_index)) {
        tmp = temp_r_key;
        while (it_r->Valid()) {
          timer1 = Timer();
          it_r->Next();
          index_time += timer1.elapsed();
          if (!it_r->Valid()) break;
          temp_r_key = it_r->key().ToString().substr(0, SECONDARY_SIZE);
          temp_r_value =
              it_r->key().ToString().substr(SECONDARY_SIZE, PRIMARY_SIZE);
          if (temp_r_key == tmp) {
            if (IsCoveringIndex(config.r_index)) {
              count1++;
            } else {
              timer1 = Timer();
              status = context.db_r->Get(ReadOptions(), temp_r_value, &value_r);
              data_time += timer1.elapsed();
              if (status.ok() &&
                  value_r.substr(0, SECONDARY_SIZE) == temp_r_key)
                count1++;
            }
          } else
            break;
        }
      }

      if (IsIndex(config.s_index)) {
        tmp = temp_s_key;
        while (it_s->Valid()) {
          timer1 = Timer();
          it_s->Next();
          index_time += timer1.elapsed();
          if (!it_s->Valid()) break;
          temp_s_key = it_s->key().ToString().substr(0, SECONDARY_SIZE);
          temp_s_value =
              it_s->key().ToString().substr(SECONDARY_SIZE, PRIMARY_SIZE);
          if (temp_s_key == tmp) {
            if (IsCoveringIndex(config.s_index)) {
              count2++;
            } else {
              timer1 = Timer();
              status = context.db_s->Get(ReadOptions(), temp_s_value, &value_s);
              data_time += timer1.elapsed();
              if (status.ok() &&
                  value_s.substr(0, SECONDARY_SIZE) == temp_s_key)
                count2++;
            }
          } else
            break;
        }
      }

      matches += count1 * count2;
      count1 = 0;
      count2 = 0;
    } else if (temp_r_key < temp_s_key) {
      timer1 = Timer();
      it_r->Next();
      index_time += timer1.elapsed();
    } else if (temp_r_key > temp_s_key) {
      timer1 = Timer();
      it_s->Next();
      index_time += timer1.elapsed();
    }
  }
}

void SortMerge(ExpConfig& config, ExpContext& context, RunResult& run_result,
               rocksdb::Iterator* it_r, rocksdb::Iterator* it_s) {
  Timer timer1 = Timer();
  // double run_size = 10;

  int matches = 0, count1 = 0, count2 = 0;
  std::vector<std::string> value_split;
  Status status;
  rocksdb::get_perf_context()->Reset();
  it_r->SeekToFirst();
  it_s->SeekToFirst();
  double val_time = 0.0, get_time = 0.0;
  if (IsLazyIndex(config.r_index) || IsEagerIndex(config.r_index) ||
      IsLazyIndex(config.s_index) || IsEagerIndex(config.s_index)) {
    SortMergeForEagerLazy(config, context, run_result, it_r, it_s, matches);
  } else if (IsCompIndex(config.r_index) || IsCompIndex(config.s_index)) {
    SortMergeForComp(config, context, run_result, it_r, it_s, matches);
  }

  run_result.matches = matches;
  delete it_r;
  delete it_s;

  return;
}

void NonIndexExternalSortMerge(ExpConfig& config, ExpContext& context,
                               RunResult& run_result) {
  cout << "external sort merge" << endl;
  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE, VALUE_SIZE = config.VALUE_SIZE;

  cout << "Serializing data" << endl;
  // Sort R
  Timer timer1 = Timer();
  int run_size =
      int((config.M - 3 * 4096) / (PRIMARY_SIZE + VALUE_SIZE) / 2) - 1;
  string prefix_r = config.db_r + "_sj_output";
  // double run_size = 10;
  string output_file_r = prefix_r + ".txt";
  string prefix_s = config.db_s + "_sj_output";
  string output_file_s = prefix_s + ".txt";
  ReadOptions read_options;
  std::vector<std::string> value_split;
  Status status;
  // sort R
  cout << "Sort R" << endl;
  int num_ways_r = config.r_tuples * (config.this_loop + 1) / run_size + 1;
  MERGE::externalSort(context.db_r, output_file_r, num_ways_r, run_size,
                      VALUE_SIZE, SECONDARY_SIZE, run_result, prefix_r);
  // sort S
  cout << "Sort S" << endl;
  int num_ways_s = config.s_tuples * (config.this_loop + 1) / run_size + 1;
  MERGE::externalSort(context.db_s, output_file_s, num_ways_s, run_size,
                      VALUE_SIZE, SECONDARY_SIZE, run_result, prefix_s);

  int matches = 0;
  ifstream in_r(output_file_r);  // File on which sorting needs to be applied
  ifstream in_s(output_file_s);  // File on which sorting needs to be applied
  if (!in_r.is_open()) {
    cout << "Unable to open file R" << endl;
    exit(1);
  };
  if (!in_s.is_open()) {
    cout << "Unable to open file S" << endl;
    exit(1);
  };

  string line_r;
  string line_s;
  string temp_r_key, temp_r_value, temp_s_key, temp_s_value;
  string tmp;
  int count1 = 1, count2 = 1;
  char* buf1 = new char[4096];
  char* buf2 = new char[4096];
  in_r.rdbuf()->pubsetbuf(buf1, 4096);
  in_s.rdbuf()->pubsetbuf(buf2, 4096);
  Timer timer2 = Timer();
  if (!getline(in_s, line_s) || !getline(in_r, line_r)) {
    DebugPrint("!getline(in_r, line_r): " + to_string(!getline(in_r, line_r)));
    DebugPrint("!getline(in_s, line_s): " + to_string(!getline(in_s, line_s)));
  };

  if (in_s.eof() || in_r.eof()) {
    DebugPrint("in_s.eof(): " + to_string(in_s.eof()));
    DebugPrint("in_r.eof(): " + to_string(in_r.eof()));
    exit(1);
  };

  while (line_s != "" && line_r != "") {
    std::istringstream iss_r(line_r);  // string stream for line_r
    std::istringstream iss_s(line_s);  // string stream for line_s
    if (getline(iss_r, temp_r_key, ',') && getline(iss_r, temp_r_value) &&
        getline(iss_s, temp_s_key, ',') && getline(iss_s, temp_s_value)) {
      if (temp_r_key == temp_s_key) {
        while (getline(in_r, line_r)) {  // read next line_r
          std::istringstream temp_iss_r(line_r);
          std::string temp_first_r, temp_second_r;
          if (getline(temp_iss_r, temp_first_r, ',') &&
              getline(temp_iss_r, temp_second_r)) {
            if (temp_first_r == temp_r_key) {
              count1++;
              continue;
            } else {
              break;
            }
          }
        }
        while (getline(in_s, line_s)) {  // read next line_s
          std::istringstream temp_iss_s(line_s);
          std::string temp_first_s, temp_second_s;
          if (getline(temp_iss_s, temp_first_s, ',') &&
              getline(temp_iss_s, temp_second_s)) {
            if (temp_first_s == temp_s_key) {
              count2++;
              continue;
            } else {
              break;
            }
          }
        }

        matches += count1 * count2;
        // cout << "matches_after: " << matches << endl;
        count1 = 1;
        count2 = 1;
      } else if (temp_r_key < temp_s_key) {
        if (!getline(in_r, line_r)) break;
      } else if (temp_r_key > temp_s_key) {
        if (!getline(in_s, line_s)) break;
      }
    } else {
      break;
    }
  }
  run_result.sort_io_time += timer2.elapsed();
  run_result.matches = matches;
  return;
}

void SingleIndexExternalSortMerge(ExpConfig& config, ExpContext& context,
                                  RunResult& run_result,
                                  rocksdb::Iterator* it_s) {
  cout << "external sort merge" << endl;

  int PRIMARY_SIZE = config.PRIMARY_SIZE,
      SECONDARY_SIZE = config.SECONDARY_SIZE, VALUE_SIZE = config.VALUE_SIZE;

  cout << "Serializing data" << endl;
  // Sort R
  Timer timer1 = Timer();
  int run_size =
      int((config.M - 3 * 4096) / (PRIMARY_SIZE + VALUE_SIZE) / 2) - 1;
  double data_time = 0.0, index_time = 0.0, sort_time = 0.0;
  Timer timer = Timer();
  // double run_size = 10;
  string prefix_r = config.db_r + "_sj_output";
  string output_file_r = prefix_r + ".txt";
  string prefix_s = config.db_s + "_sj_output";
  string output_file_s = prefix_s + ".txt";
  ReadOptions read_options;
  std::vector<std::string> value_split;
  Status s;
  int num_ways_r = config.r_tuples * (config.this_loop + 1) / run_size + 1;
  // S is indexed, already sorted
  MERGE::externalSort(context.db_r, output_file_r, num_ways_r, run_size,
                      VALUE_SIZE, SECONDARY_SIZE, run_result, prefix_r);

  int matches = 0;
  ifstream in_r(output_file_r);  // File on which sorting needs to be applied
  if (!in_r.is_open()) {
    cerr << "Unable to open file" << endl;
  };
  string line_r;
  string line_s;
  string temp_r_key, temp_r_value, temp_s_key, temp_s_value;
  string tmp;
  int count1 = 1, count2 = 0;
  char* buf1 = new char[4096];
  char* buf2 = new char[4096];
  in_r.rdbuf()->pubsetbuf(buf2, 4096);
  it_s->SeekToFirst();
  if (!it_s->Valid() || !getline(in_r, line_r)) {
    DebugPrint("!it_s->Valid(): " + to_string(!it_s->Valid()));
    DebugPrint("!getline(in_r, line_r): " + to_string(!getline(in_r, line_r)));
  };

  while (it_s->Valid() && line_r != "") {
    std::istringstream iss_r(line_r);  // string stream for line_r
    timer = Timer();
    if (getline(iss_r, temp_r_key, ',') && getline(iss_r, temp_r_value)) {
      sort_time += timer.elapsed();
      if (IsCompIndex(config.s_index)) {
        temp_s_key = it_s->key().ToString().substr(0, SECONDARY_SIZE);
        temp_s_value =
            it_s->key().ToString().substr(SECONDARY_SIZE, PRIMARY_SIZE);
      } else {
        temp_s_key = it_s->key().ToString();
        temp_s_value = it_s->value().ToString();
      }

      if (temp_r_key == temp_s_key) {
        timer = Timer();
        while (getline(in_r, line_r)) {  // read next line_r
          std::istringstream temp_iss_r(line_r);
          std::string temp_first_r, temp_second_r;
          if (getline(temp_iss_r, temp_first_r, ',') &&
              getline(temp_iss_r, temp_second_r)) {
            if (temp_first_r == temp_r_key) {
              count1++;
              continue;
            } else {
              break;
            }
          }
        }
        sort_time += timer.elapsed();
        value_split =
            boost::split(value_split, temp_s_value, boost::is_any_of(":"));

        if (IsCoveringIndex(config.s_index) || !IsIndex(config.s_index)) {
          count2 += value_split.size();
          if (config.s_index == IndexType::CComp) {
            tmp = temp_s_key;
            timer = Timer();
            while (it_s->Valid()) {
              it_s->Next();
              if (!it_s->Valid()) break;
              temp_s_key = it_s->key().ToString().substr(0, SECONDARY_SIZE);
              temp_s_value =
                  it_s->key().ToString().substr(SECONDARY_SIZE, PRIMARY_SIZE);
              if (temp_s_key == tmp) {
                count2++;
              } else
                break;
            }
            index_time += timer.elapsed();
          }
        } else {
          if (IsEagerIndex(config.s_index) || IsLazyIndex(config.s_index)) {
            for (auto x : value_split) {
              timer = Timer();
              s = context.db_s->Get(ReadOptions(), x.substr(0, PRIMARY_SIZE),
                                    &tmp);
              data_time += timer.elapsed();
              if (s.ok() && tmp.substr(0, SECONDARY_SIZE) == temp_s_key)
                count2++;
            }
          } else {
            timer = Timer();
            string value_s;
            s = context.db_s->Get(ReadOptions(), temp_s_value, &value_s);
            data_time += timer.elapsed();
            if (s.ok() && value_s.substr(0, SECONDARY_SIZE) == temp_s_key)
              count2++;
            data_time += timer.elapsed();
            tmp = temp_s_key;
            while (it_s->Valid()) {
              timer = Timer();
              it_s->Next();
              index_time += timer.elapsed();
              if (!it_s->Valid()) break;
              temp_s_key = it_s->key().ToString().substr(0, SECONDARY_SIZE);
              temp_s_value =
                  it_s->key().ToString().substr(SECONDARY_SIZE, PRIMARY_SIZE);
              if (temp_s_key == tmp) {
                timer = Timer();
                s = context.db_s->Get(ReadOptions(), temp_s_value, &value_s);
                data_time += timer.elapsed();
                if (s.ok() && value_s.substr(0, SECONDARY_SIZE) == temp_s_key)
                  count2++;
              } else
                break;
            }
          }
        }

        matches += count1 * count2;
        // cout << "matches_after: " << matches << endl;
        count1 = 1;
        count2 = 0;
      } else if (temp_r_key < temp_s_key) {
        timer = Timer();
        if (!getline(in_r, line_r)) break;
        sort_time += timer.elapsed();
      } else if (temp_r_key > temp_s_key) {
        timer = Timer();
        it_s->Next();
        index_time += timer.elapsed();
      }
    } else {
      break;
    }
    // cout << "line_r_end: " << line_r << endl;
  }
  run_result.matches = matches;
  run_result.get_data_time += data_time;
  run_result.get_index_time += index_time;
  run_result.sort_io_time += sort_time;
  delete it_s;
  return;
}

void NestedLoop(ExpConfig& config, ExpContext& context, RunResult& result) {
  Timer timer = Timer();
  ReadOptions read_options;
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
  double data_time = 0.0;
  Timer timer = Timer();
  for (it_r->SeekToFirst(); it_r->Valid(); it_r->Next()) {
    tmp_r = it_r->value().ToString().substr(0, config.SECONDARY_SIZE);
    status = context.db_s->Get(read_options, tmp_r, &value);
    if (status.ok()) matches++;
  }
  data_time += timer.elapsed();
  result.get_data_time += data_time;
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
  if (IsCompIndex(config.r_index) || IsCompIndex(config.s_index)) {
    read_options.total_order_seek = false;
    read_options.auto_prefix_mode = false;
  }
  rocksdb::Iterator* it_r = context.db_r->NewIterator(read_options);
  rocksdb::Iterator* it_s = context.ptr_index_s->NewIterator(read_options);
  uint64_t matches = 0;  // number of matches
  Status s;
  string tmp;
  std::vector<std::string> value_split;
  Timer timer = Timer();
  double data_time = 0.0, index_time = 0.0;
  if (IsEagerIndex(config.s_index) || IsLazyIndex(config.s_index)) {
    for (it_r->SeekToFirst(); it_r->Valid();) {
      tmp_r = it_r->value().ToString().substr(0, SECONDARY_SIZE);
      timer = Timer();
      s = context.ptr_index_s->Get(read_options, tmp_r, &value);
      data_time += timer.elapsed();
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
          index_time += timer.elapsed();
        }
      }
      timer = Timer();
      it_r->Next();
      data_time += timer.elapsed();
      if (!it_r->Valid()) break;
    }
  } else if (IsCompIndex(config.s_index)) {
    // it_r: primary key, value: secondary key
    for (it_r->SeekToFirst(); it_r->Valid();) {
      tmp_r = it_r->value().ToString().substr(0, SECONDARY_SIZE);
      secondary_key_lower = tmp_r + string(PRIMARY_SIZE, '0');
      secondary_key_upper = tmp_r + string(PRIMARY_SIZE, '9');
      timer = Timer();
      it_s->Seek(secondary_key_lower);
      index_time += timer.elapsed();
      for (; it_s->Valid() && it_s->key().ToString() <= secondary_key_upper;) {
        string tmp_s = it_s->key().ToString().substr(0, SECONDARY_SIZE);
        if (tmp_s == tmp_r) {
          if (!covering) {
            timer = Timer();
            s = context.db_s->Get(
                read_options,
                it_s->key().ToString().substr(SECONDARY_SIZE, PRIMARY_SIZE),
                &value);
            if (s.ok() && value.substr(0, SECONDARY_SIZE) == tmp_s) matches++;
            data_time += timer.elapsed();
          } else
            matches++;
        }
        timer = Timer();
        it_s->Next();
        index_time += timer.elapsed();
        if (!it_s->Valid() || it_s->key().ToString() > secondary_key_upper)
          break;
      }
      timer = Timer();
      it_r->Next();
      data_time += timer.elapsed();
      if (!it_r->Valid()) break;
    }
  }

  result.matches = matches;
  result.get_data_time += data_time;
  result.get_index_time += index_time;

  delete it_r;
  delete it_s;
}
