#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

using namespace std;

bool debug = false;

void DebugPrint(string str) {
  if (debug) {
    cout << str << endl;
  }
}

class Timer {
 public:
  Timer() { clock_gettime(CLOCK_MONOTONIC, &t1); }

  double elapsed() {
    clock_gettime(CLOCK_MONOTONIC, &t2);
    return (t2.tv_sec - t1.tv_sec) + (t2.tv_nsec - t1.tv_nsec) / 1000000000.0;
  }

 private:
  struct timespec t1, t2;
};

struct RunResult {
  int loop;
  uint64_t join_read_io;
  uint64_t matches;
  double index_build_time;
  double join_time;
  // For Hash Join
  double partition_time;
  // For External Sort Merge Join
  double sort_time;
  double cache_hit_rate;
  double false_positive_rate;

  double sync_time;
  double eager_time;
  double update_time;

  double valid_time;
  double get_index_time;
  double get_data_time;
  double post_list_time;
  double sort_cpu_time;
  double sort_io_time;
  double hash_cpu_time;
  double hash_io_time;
  // init
  RunResult(int lp) {
    loop = lp;
    join_read_io = 0;
    matches = 0;
    sync_time = 0;
    eager_time = 0;
    update_time = 0;
    valid_time = 0;
    get_index_time = 0;
    get_data_time = 0;
    post_list_time = 0;
    sort_cpu_time = 0;
    sort_io_time = 0;
    hash_cpu_time = 0;
    hash_io_time = 0;
    index_build_time = 0;
    join_time = 0;
    partition_time = 0;
    sort_time = 0;
    cache_hit_rate = 0;
    false_positive_rate = 0;
  }
};

class ExpResult {
 public:
  static ExpResult &getInstance() {
    static ExpResult instance;
    return instance;
  }

  // forbid copy and move
  ExpResult(ExpResult const &) = delete;
  void operator=(ExpResult const &) = delete;

  void AddRunResult(RunResult run_result) {
    run_results.push_back(run_result);
    sum_join_read_io += run_result.join_read_io;
    sum_sync_time += run_result.sync_time;
    sum_eager_time += run_result.eager_time;
    sum_update_time += run_result.update_time;
    sum_valid_time += run_result.valid_time;
    sum_get_index_time += run_result.get_index_time;
    sum_get_data_time += run_result.get_data_time;
    sum_post_list_time += run_result.post_list_time;
    sum_sort_cpu_time += run_result.sort_cpu_time;
    sum_sort_io_time += run_result.sort_io_time;
    sum_hash_cpu_time += run_result.hash_cpu_time;
    sum_hash_io_time += run_result.hash_io_time;
    sum_index_build_time += run_result.index_build_time;
    sum_join_time += run_result.join_time;
    sum_partition_time += run_result.partition_time;
    sum_sort_time += run_result.sort_time;
  }

  void ShowRunResult(int loop) {
    cout << "join_read_io: " << run_results[loop].join_read_io << " / ";
    cout << "matches: " << run_results[loop].matches << " / ";
    cout << "join_time: " << run_results[loop].join_time << " / ";
    cout << "index_build_time: " << run_results[loop].index_build_time << " / ";
    cout << "partition_time: " << run_results[loop].partition_time << " / ";
    cout << "sort_time: " << run_results[loop].sort_time << endl;
  }

  void ShowExpResult() {
    cout << "-------------------------" << endl;
    cout << "sum_join_read_io: " << sum_join_read_io << " / ";
    cout << "sum_sync_time: " << sum_sync_time << " / ";
    cout << "sum_eager_time: " << sum_eager_time << " / ";
    cout << "sum_update_time: " << sum_update_time << " / ";
    cout << "sum_valid_time: " << sum_valid_time << " / ";
    cout << "sum_get_index_time: " << sum_get_index_time << " / ";
    cout << "sum_get_data_time: " << sum_get_data_time << " / ";
    cout << "sum_post_list_time: " << sum_post_list_time << " / ";
    cout << "sum_sort_cpu_time: " << sum_sort_cpu_time << " / ";
    cout << "sum_sort_io_time: " << sum_sort_io_time << " / ";
    cout << "sum_hash_cpu_time: " << sum_hash_cpu_time << " / ";
    cout << "sum_hash_io_time: " << sum_hash_io_time << " / ";
    cout << "sum_join_time: " << sum_join_time << " / ";
    cout << "sum_index_build_time: " << sum_index_build_time << " / ";
    cout << "sum_partition_time: " << sum_partition_time << " / ";
    cout << "sum_sort_time: " << sum_sort_time << " / ";
    cout << "cache_hit_rate: " << run_results[0].cache_hit_rate << " / ";
    cout << "false_positive_rate: " << run_results[0].false_positive_rate
         << endl;
  }

  void WriteResultToFile(string file_name, string config_info) {
    // Write to file
    ofstream outfile;
    // if file does not exist, Create new file
    // else append to existing file
    outfile.open(file_name, ios::out | ios::app);
    outfile << "-------------------------" << endl;
    outfile << config_info << " ";
    outfile << "matches=" << run_results.back().matches << " ";
    outfile << "sum_join_read_io=" << sum_join_read_io << " ";
    outfile << "sum_sync_time=" << sum_sync_time << " ";
    outfile << "sum_eager_time=" << sum_eager_time << " ";
    outfile << "sum_update_time=" << sum_update_time << " ";
    outfile << "sum_valid_time=" << sum_valid_time << " ";
    outfile << "sum_get_index_time=" << sum_get_index_time << " ";
    outfile << "sum_get_data_time=" << sum_get_data_time << " ";
    outfile << "sum_post_list_time=" << sum_post_list_time << " ";
    outfile << "sum_sort_cpu_time=" << sum_sort_cpu_time << " ";
    outfile << "sum_sort_io_time=" << sum_sort_io_time << " ";
    outfile << "sum_hash_cpu_time=" << sum_hash_cpu_time << " ";
    outfile << "sum_hash_io_time=" << sum_hash_io_time << " ";
    outfile << "sum_join_time=" << sum_join_time << " ";
    outfile << "sum_index_build_time=" << sum_index_build_time << " ";
    outfile << "cache_hit_rate=" << run_results[0].cache_hit_rate << " ";
    outfile << "false_positive_rate=" << run_results[0].false_positive_rate
            << endl;
    outfile << "-------------------------" << endl;
    outfile.close();
  }

 private:
  ExpResult() {}
  uint64_t sum_join_read_io = 0;
  double sum_sync_time = 0, sum_eager_time = 0, sum_update_time = 0,
         sum_valid_time = 0, sum_get_index_time = 0, sum_get_data_time = 0,
         sum_post_list_time = 0, sum_sort_cpu_time = 0, sum_sort_io_time = 0,
         sum_hash_cpu_time = 0, sum_hash_io_time = 0, sum_join_time = 0,
         sum_index_build_time = 0, sum_partition_time = 0, sum_sort_time = 0;
  // RunResult
  vector<RunResult> run_results;
};

enum class IndexType {
  Eager,
  Lazy,
  Comp,
  CEager,
  CLazy,
  CComp,
  Primary,
  Regular,
  Embedded
};

bool IsCoveringIndex(IndexType index_type) {
  switch (index_type) {
    case IndexType::CEager:
    case IndexType::CLazy:
    case IndexType::CComp:
      return true;
    default:
      return false;
  }
}

bool IsNonCoveringIndex(IndexType index_type) {
  switch (index_type) {
    case IndexType::Eager:
    case IndexType::Lazy:
    case IndexType::Comp:
      return true;
    default:
      return false;
  }
}

bool IsIndex(IndexType index_type) {
  return IsCoveringIndex(index_type) || IsNonCoveringIndex(index_type);
}

bool IsEagerIndex(IndexType index_type) {
  switch (index_type) {
    case IndexType::Eager:
    case IndexType::CEager:
      return true;
    default:
      return false;
  }
}

bool IsLazyIndex(IndexType index_type) {
  switch (index_type) {
    case IndexType::Lazy:
    case IndexType::CLazy:
      return true;
    default:
      return false;
  }
}

bool IsCompIndex(IndexType index_type) {
  switch (index_type) {
    case IndexType::Comp:
    case IndexType::CComp:
      return true;
    default:
      return false;
  }
}

string IndexTypeToString(IndexType index_type) {
  switch (index_type) {
    case IndexType::Eager:
      return "Eager";
    case IndexType::Lazy:
      return "Lazy";
    case IndexType::Comp:
      return "Comp";
    case IndexType::CEager:
      return "CEager";
    case IndexType::CLazy:
      return "CLazy";
    case IndexType::CComp:
      return "CComp";
    case IndexType::Primary:
      return "Primary";
    case IndexType::Regular:
      return "Regular";
    case IndexType::Embedded:
      return "Embedded";
    default:
      return "Unknown";
  }
}

IndexType StringToIndexType(string index_type) {
  if (index_type == "Eager") {
    return IndexType::Eager;
  } else if (index_type == "Lazy") {
    return IndexType::Lazy;
  } else if (index_type == "Comp") {
    return IndexType::Comp;
  } else if (index_type == "CEager") {
    return IndexType::CEager;
  } else if (index_type == "CLazy") {
    return IndexType::CLazy;
  } else if (index_type == "CComp") {
    return IndexType::CComp;
  } else if (index_type == "Primary") {
    return IndexType::Primary;
  } else if (index_type == "Regular") {
    return IndexType::Regular;
  } else if (index_type == "Embedded") {
    return IndexType::Embedded;
  } else {
    cout << "Unknown index type: " << index_type << endl;
    exit(1);
  }
}

enum JoinAlgorithm { INLJ, SJ, HJ };

string JoinAlgorithmToString(JoinAlgorithm join_algorithm) {
  switch (join_algorithm) {
    case JoinAlgorithm::INLJ:
      return "INLJ";
    case JoinAlgorithm::SJ:
      return "SJ";
    case JoinAlgorithm::HJ:
      return "HJ";
    default:
      return "Unknown";
  }
}

JoinAlgorithm StringToJoinAlgorithm(string join_algorithm) {
  if (join_algorithm == "INLJ") {
    return JoinAlgorithm::INLJ;
  } else if (join_algorithm == "SJ") {
    return JoinAlgorithm::SJ;
  } else if (join_algorithm == "HJ") {
    return JoinAlgorithm::HJ;
  } else {
    cout << "Unknown join algorithm: " << join_algorithm << endl;
    exit(1);
  }
}