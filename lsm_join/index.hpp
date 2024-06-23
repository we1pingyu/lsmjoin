#pragma once

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <iostream>
#include <random>
#include <regex>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "exp_utils.hpp"
#include "rocksdb/db.h"
#include "rocksdb/file_system.h"
#include "rocksdb/iostats_context.h"
#include "rocksdb/perf_context.h"
#include "rocksdb/sst_file_reader.h"
#include "rocksdb/statistics.h"

using namespace std;
using namespace ROCKSDB_NAMESPACE;

uint64_t randomNumber(int n = 10) {
  uint64_t max_val = pow(10, n);
  return rand() * 13131 % max_val;
}

void generatePK(uint64_t r, std::vector<uint64_t> &R, int c = 1, int n = 10) {
  std::mt19937 gen;
  uint64_t x;
  int y;
  while (R.size() < r) {
    x = randomNumber(n);
    // y = c;
    for (int j = 0; j < c; ++j) {
      R.push_back(x);
    }
  }
  shuffle(R.begin(), R.end(), gen);
}

void generateData(uint64_t s, uint64_t r, double eps_s, double k_r, double k_s,
                  std::vector<uint64_t> &S, std::vector<uint64_t> &R,
                  bool skew = false, int n = 10) {
  std::mt19937 gen;
  if (!skew) {
    uint64_t x;
    while (S.size() < s) {
      x = randomNumber(n);
      for (int j = 0; j < k_s; ++j) S.push_back(x);
      if (((double)rand() / RAND_MAX) > eps_s) {
        for (int j = 0; j < k_r; ++j) {
          R.push_back(x);
        }
      }
    }
    cout << "S before size: " << S.size() << endl;
    cout << "R before size: " << R.size() << endl;
    while (R.size() < r) {
      x = randomNumber(n);
      for (int j = 0; j < k_r; ++j) {
        R.push_back(x);
      }
    }
  } else {
    uint64_t x, y;
    lognormal_distribution<> distribution(0, k_s);
    uint64_t modulus = static_cast<uint64_t>(std::pow(10, n));
    YCSBGenerator *data_gen = new YCSBGenerator(s, "zipf", k_s);
    for (int i = 0; i < s; ++i) {
      x = data_gen->gen_existing_key();
      // cout << "x: " << x << endl;
      S.push_back(x);
      R.push_back(x);
    }
    cout << "S before size: " << S.size() << endl;
    cout << "R before size: " << R.size() << endl;
    while (R.size() < r) {
      x = static_cast<uint64_t>(distribution(gen) * s) % modulus;
      R.push_back(x);
    }
    // std::unordered_map<uint64_t, int> frequencyMap;
    // for (uint64_t num : S) {
    //   frequencyMap[num]++;
    // }
    // double sum =
    //     std::accumulate(frequencyMap.begin(), frequencyMap.end(), 0.0,
    //                     [](double a, const std::pair<uint64_t, int> &b) {
    //                       return a + b.second;
    //                     });

    // double average = sum / frequencyMap.size();
    // std::cout << "Average number of times each element appears: " << average
    //           << std::endl;
  }
  shuffle(S.begin(), S.end(), gen);
  shuffle(R.begin(), R.end(), gen);
}

void ingest_pk_data(uint64_t tuples, DB *db, const std::vector<uint64_t> &data,
                    int VALUE_SIZE, int SECONDARY_SIZE, int PRIMARY_SIZE) {
  std::string tmp, tmp_key, tmp_value;
  WriteOptions write_options;
  write_options.low_pri = true;
  write_options.disableWAL = true;
  for (uint64_t i = 0; i < tuples; i++) {
    if ((i + 1) % 5000000 == 0) {
      cout << (i + 1) / 1000000 << " million" << endl;
    }
    tmp = to_string(data[i]);
    tmp_key =
        string(PRIMARY_SIZE - min(PRIMARY_SIZE, int(tmp.length())), '0') + tmp;
    // cout << tmp_key << " " << tmp_value << endl;
    db->Put(write_options, tmp_key, string(VALUE_SIZE, '0'));
  }
}

void ingest_data(uint64_t tuples, DB *db, const std::vector<uint64_t> &pk,
                 const std::vector<uint64_t> &data, int VALUE_SIZE,
                 int SECONDARY_SIZE, int PRIMARY_SIZE) {
  std::string tmp, tmp_key, tmp_value;

  cout << "pk size: " << pk.size() << endl;
  bool use_pk = (pk.size() != 0);
  // write to file
  ofstream outfile;
  WriteOptions write_options;
  write_options.low_pri = true;
  write_options.disableWAL = true;
  for (uint64_t i = 0; i < tuples; i++) {
    if ((i + 1) % 5000000 == 0) {
      cout << (i + 1) / 1000000 << " million" << endl;
    }
    tmp = use_pk ? std::to_string(pk[i]) : std::to_string(i);
    tmp_key =
        string(PRIMARY_SIZE - min(PRIMARY_SIZE, int(tmp.length())), '0') + tmp;
    tmp = to_string(data[i]);
    tmp_value =
        string(SECONDARY_SIZE - min(SECONDARY_SIZE, int(tmp.length())), '0') +
        tmp + string(VALUE_SIZE - SECONDARY_SIZE, '0');
    // cout << tmp_key << " " << tmp_value << endl;
    db->Put(write_options, tmp_key, tmp_value);
  }
}

void build_composite_index(DB *db, DB *index, uint64_t *data,
                           const std::vector<uint64_t> &pk, uint64_t tuples,
                           int TOTAL_VALUE_SIZE, int SECONDARY_SIZE,
                           int PRIMARY_SIZE, double &update_time) {
  cout << "building composite index..." << endl;
  string secondary_key, value, tmp_secondary, tmp_primary, tmp_value;
  WriteOptions write_options;
  write_options.low_pri = true;
  write_options.disableWAL = true;
  ReadOptions read_options;
  Status s;
  bool use_pk = (pk.size() != 0);
  for (uint64_t i = 0; i < tuples; i++) {
    if ((i + 1) % 5000000 == 0) {
      cout << (i + 1) / 1000000 << " million" << endl;
    }
    tmp_secondary = std::to_string(data[i]);
    tmp_primary = use_pk ? std::to_string(pk[i]) : std::to_string(i);
    secondary_key =
        string(
            SECONDARY_SIZE - min(SECONDARY_SIZE, int(tmp_secondary.length())),
            '0') +
        tmp_secondary +
        string(PRIMARY_SIZE - min(PRIMARY_SIZE, int(tmp_primary.length())),
               '0') +
        tmp_primary;
    Timer timer = Timer();
    index->Put(write_options, secondary_key, NULL);
    update_time += timer.elapsed();
  }
}

double build_covering_composite_index(DB *db, DB *index, uint64_t *data,
                                      const std::vector<uint64_t> &pk,
                                      uint64_t tuples, int TOTAL_VALUE_SIZE,
                                      int SECONDARY_SIZE, int PRIMARY_SIZE,
                                      double &sync_time, double &update_time,
                                      bool non_covering) {
  cout << "building covering composite index..." << endl;
  string secondary_key, value, tmp_secondary, tmp_primary, tmp_value, tmp,
      tmp_key;
  WriteOptions write_options;
  write_options.low_pri = true;
  write_options.disableWAL = true;
  ReadOptions read_options;
  Status s;
  struct timespec t1, t2;
  double db_ingest_time = 0.0;
  bool use_pk = (pk.size() != 0);
  for (uint64_t i = 0; i < tuples; i++) {
    if ((i + 1) % 5000000 == 0) {
      cout << (i + 1) / 1000000 << " million" << endl;
    }
    tmp = use_pk ? std::to_string(pk[i]) : std::to_string(i);
    tmp_key =
        string(PRIMARY_SIZE - min(PRIMARY_SIZE, int(tmp.length())), '0') + tmp;
    tmp = to_string(data[i]);
    tmp_value =
        string(SECONDARY_SIZE - min(SECONDARY_SIZE, int(tmp.length())), '0') +
        tmp + string(TOTAL_VALUE_SIZE - SECONDARY_SIZE, '0');
    Timer timer1 = Timer();
    s = db->Get(read_options, tmp_key, &tmp_secondary);
    sync_time += timer1.elapsed();
    Timer timer2 = Timer();
    if (s.ok()) {
      index->SingleDelete(write_options,
                          tmp_secondary.substr(0, SECONDARY_SIZE) + tmp_key);
    }
    if (non_covering)
      index->Put(write_options, tmp_value.substr(0, SECONDARY_SIZE) + tmp_key,
                 NULL);
    else
      index->Put(write_options, tmp_value.substr(0, SECONDARY_SIZE) + tmp_key,
                 string(TOTAL_VALUE_SIZE - SECONDARY_SIZE, '0'));
    update_time += timer2.elapsed();
    // TODO  Index Time is calculated twice.
    Timer timer3 = Timer();
    db->Put(write_options, tmp_key, tmp_value);
    db_ingest_time += timer3.elapsed();
  }
  return db_ingest_time;
}

double build_covering_lazy_index(DB *db, DB *index, uint64_t *data,
                                 const std::vector<uint64_t> &pk,
                                 uint64_t tuples, int TOTAL_VALUE_SIZE,
                                 int SECONDARY_SIZE, int PRIMARY_SIZE,
                                 double &sync_time, double &update_time,
                                 double &post_list_time, bool non_covering) {
  cout << "building covering lazy index..." << endl;
  string secondary_key, value, tmp_secondary, tmp_primary, tmp, tmp_key,
      tmp_value;
  std::vector<std::string> value_split;
  WriteOptions write_options;
  write_options.low_pri = true;
  write_options.disableWAL = true;
  ReadOptions read_options;
  Status s;
  bool use_pk = (pk.size() != 0);
  struct timespec t1, t2;
  double db_ingest_time = 0.0;
  for (uint64_t i = 0; i < tuples; i++) {
    if ((i + 1) % 5000000 == 0) {
      cout << (i + 1) / 1000000 << " million" << endl;
    }
    tmp = use_pk ? std::to_string(pk[i]) : std::to_string(i);
    tmp_key =
        string(PRIMARY_SIZE - min(PRIMARY_SIZE, int(tmp.length())), '0') + tmp;
    tmp = to_string(data[i]);
    tmp_value =
        string(SECONDARY_SIZE - min(SECONDARY_SIZE, int(tmp.length())), '0') +
        tmp + string(TOTAL_VALUE_SIZE - SECONDARY_SIZE, '0');
    Timer timer1 = Timer();
    s = db->Get(read_options, tmp_key, &tmp_secondary);
    sync_time += timer1.elapsed();
    if (s.ok()) {
      Timer timer2 = Timer();
      s = index->Get(read_options, tmp_secondary.substr(0, SECONDARY_SIZE),
                     &tmp);
      sync_time += timer2.elapsed();
      if (s.ok()) {
        Timer timer3 = Timer();
        boost::split(value_split, tmp, boost::is_any_of(":"));
        value_split.erase(
            std::remove_if(value_split.begin(), value_split.end(),
                           [PRIMARY_SIZE, &tmp_key](const std::string &item) {
                             return item.substr(0, PRIMARY_SIZE) == tmp_key;
                           }),
            value_split.end());
        post_list_time += timer3.elapsed();
        if (value_split.empty()) {
          Timer timer4 = Timer();
          index->Delete(write_options, tmp_secondary.substr(0, SECONDARY_SIZE));
          update_time += timer4.elapsed();
        } else {
          Timer timer5 = Timer();
          std::ostringstream oss;
          auto it = value_split.begin();
          oss << *it;
          for (++it; it != value_split.end(); ++it) {
            oss << ':' << *it;
          }
          post_list_time += timer5.elapsed();
          Timer timer6 = Timer();
          index->Put(write_options, tmp_secondary.substr(0, SECONDARY_SIZE),
                     oss.str());
          update_time += timer6.elapsed();
        }
      }
    }
    Timer timer7 = Timer();
    if (non_covering)
      index->Merge(write_options, tmp_value.substr(0, SECONDARY_SIZE), tmp_key);
    else
      index->Merge(write_options, tmp_value.substr(0, SECONDARY_SIZE),
                   tmp_key + string(TOTAL_VALUE_SIZE - SECONDARY_SIZE,
                                    '0'));  // Lazy Index
    update_time += timer7.elapsed();
    Timer timer3 = Timer();
    db->Put(write_options, tmp_key, tmp_value);
    db_ingest_time += timer3.elapsed();
  }
  return db_ingest_time;
}

void build_lazy_index(DB *db, DB *index, uint64_t *data,
                      const std::vector<uint64_t> &pk, uint64_t tuples,
                      int TOTAL_VALUE_SIZE, int SECONDARY_SIZE,
                      int PRIMARY_SIZE, double &update_time) {
  cout << "building lazy index..." << endl;
  string secondary_key, value, tmp_secondary, tmp_primary;
  WriteOptions write_options;
  write_options.low_pri = true;
  write_options.disableWAL = true;
  bool use_pk = (pk.size() != 0);
  for (uint64_t i = 0; i < tuples; i++) {
    if ((i + 1) % 5000000 == 0) {
      cout << (i + 1) / 1000000 << " million" << endl;
    }
    tmp_secondary = to_string(data[i]);
    tmp_primary = use_pk ? std::to_string(pk[i]) : std::to_string(i);
    secondary_key = string(SECONDARY_SIZE -
                               min(SECONDARY_SIZE, int(tmp_secondary.length())),
                           '0') +
                    tmp_secondary;
    value = string(PRIMARY_SIZE - min(PRIMARY_SIZE, int(tmp_primary.length())),
                   '0') +
            tmp_primary;
    Timer timer = Timer();
    index->Merge(write_options, secondary_key, value);
    update_time += timer.elapsed();
  }
}

double build_covering_eager_index(DB *db, DB *index, uint64_t *data,
                                  const std::vector<uint64_t> &pk,
                                  uint64_t tuples, int TOTAL_VALUE_SIZE,
                                  int SECONDARY_SIZE, int PRIMARY_SIZE,
                                  double &sync_time, double &update_time,
                                  double &post_list_time, double &eager_time,
                                  bool non_covering) {
  cout << "building covering eager index..." << endl;
  string secondary_key, value, tmp_secondary, tmp_primary, tmp, tmp_key,
      tmp_value;
  std::vector<std::string> value_split;
  WriteOptions write_options;
  write_options.low_pri = true;
  write_options.disableWAL = true;
  ReadOptions read_options;
  Status s;
  bool use_pk = (pk.size() != 0);
  struct timespec t1, t2;
  double db_ingest_time = 0.0;
  std::stringstream ss;
  string rest = string(TOTAL_VALUE_SIZE - SECONDARY_SIZE, '0');
  for (uint64_t i = 0; i < tuples; i++) {
    if ((i + 1) % 5000000 == 0) {
      cout << (i + 1) / 1000000 << " million" << endl;
    }
    tmp = use_pk ? std::to_string(pk[i]) : std::to_string(i);
    tmp_key =
        string(PRIMARY_SIZE - min(PRIMARY_SIZE, int(tmp.length())), '0') + tmp;
    tmp = to_string(data[i]);
    tmp_value =
        string(SECONDARY_SIZE - min(SECONDARY_SIZE, int(tmp.length())), '0') +
        tmp + string(TOTAL_VALUE_SIZE - SECONDARY_SIZE, '0');
    Timer timer1 = Timer();
    s = db->Get(read_options, tmp_key, &tmp_secondary);
    sync_time += timer1.elapsed();
    if (s.ok()) {
      Timer timer2 = Timer();
      s = index->Get(read_options, tmp_secondary.substr(0, SECONDARY_SIZE),
                     &tmp);
      sync_time += timer2.elapsed();
      if (s.ok()) {
        Timer timer3 = Timer();
        boost::split(value_split, tmp, boost::is_any_of(":"));
        value_split.erase(
            std::remove_if(value_split.begin(), value_split.end(),
                           [PRIMARY_SIZE, &tmp_key](const std::string &item) {
                             return item.substr(0, PRIMARY_SIZE) == tmp_key;
                           }),
            value_split.end());
        post_list_time += timer3.elapsed();
        if (value_split.size() == 0) {
          Timer timer4 = Timer();
          index->Delete(write_options, tmp_secondary.substr(0, SECONDARY_SIZE));
          update_time += timer4.elapsed();
        } else {
          Timer timer5 = Timer();
          std::ostringstream oss;
          auto it = value_split.begin();
          oss << *it;
          for (++it; it != value_split.end(); ++it) {
            oss << ':' << *it;
          }
          post_list_time += timer5.elapsed();
          Timer timer6 = Timer();
          index->Put(write_options, tmp_secondary.substr(0, SECONDARY_SIZE),
                     oss.str());
          update_time += timer6.elapsed();
        }
      }
    }
    secondary_key = tmp_value.substr(0, SECONDARY_SIZE);
    Timer timer7 = Timer();
    s = index->Get(read_options, secondary_key, &tmp_primary);
    // cout << "secondary_key: " << secondary_key << endl;
    eager_time += timer7.elapsed();
    Timer timer8 = Timer();
    if (s.ok()) {
      if (!non_covering)
        index->Put(write_options, secondary_key,
                   tmp_key + rest + ":" + tmp_primary);
      else
        index->Put(write_options, secondary_key, tmp_key + ":" + tmp_primary);
    } else {
      if (!non_covering)
        index->Put(write_options, secondary_key, tmp_key + rest);
      else
        index->Put(write_options, secondary_key, tmp_key);
    }
    update_time += timer8.elapsed();
    Timer timer3 = Timer();
    db->Put(write_options, tmp_key, tmp_value);
    db_ingest_time += timer3.elapsed();
  }
  return db_ingest_time;
}

void build_eager_index(DB *db, DB *index, uint64_t *data,
                       const std::vector<uint64_t> &pk, uint64_t tuples,
                       int TOTAL_VALUE_SIZE, int SECONDARY_SIZE,
                       int PRIMARY_SIZE, double &update_time,
                       double &eager_time) {
  cout << "building eager index..." << endl;
  string secondary_key, value, tmp_secondary, tmp_primary, tmp_value, tmp;
  ReadOptions read_options;
  WriteOptions write_options;
  write_options.low_pri = true;
  write_options.disableWAL = true;
  Status s;
  vector<string> value_split;
  bool use_pk = (pk.size() != 0);
  for (uint64_t i = 0; i < tuples; i++) {
    if ((i + 1) % 5000000 == 0) {
      cout << (i + 1) / 1000000 << " " << endl;
    }
    tmp_secondary = to_string(data[i]);
    tmp_primary = use_pk ? std::to_string(pk[i]) : std::to_string(i);
    secondary_key = string(SECONDARY_SIZE -
                               min(SECONDARY_SIZE, int(tmp_secondary.length())),
                           '0') +
                    tmp_secondary;
    value = string(PRIMARY_SIZE - min(PRIMARY_SIZE, int(tmp_primary.length())),
                   '0') +
            tmp_primary;
    Timer timer1 = Timer();
    s = index->Get(read_options, secondary_key, &tmp_primary);
    eager_time += timer1.elapsed();
    Timer timer2 = Timer();
    if (s.ok()) {
      tmp_primary = value + ":" + tmp_primary;
      index->Put(write_options, secondary_key, tmp_primary);
    } else {
      index->Put(write_options, secondary_key, value);
    }
    update_time += timer2.elapsed();
  }
}