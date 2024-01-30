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
  double val_time;
  double get_time;
  double index_build_time;
  double join_time;
  // For Hash Join
  double partition_time;
  // For External Sort Merge Join
  double sort_time;
  // init
  RunResult(int lp) {
    loop = lp;
    join_read_io = 0;
    matches = 0;
    val_time = 0;
    get_time = 0;
    index_build_time = 0;
    join_time = 0;
    partition_time = 0;
    sort_time = 0;
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
    sum_val_time += run_result.val_time;
    sum_get_time += run_result.get_time;
    sum_join_time += run_result.join_time;
    sum_index_build_time += run_result.index_build_time;
    sum_partition_time += run_result.partition_time;
    sum_sort_time += run_result.sort_time;
  }

  void ShowRunResult(int loop) {
    cout << "join_read_io: " << run_results[loop].join_read_io << endl;
    cout << "matches: " << run_results[loop].matches << endl;
    cout << "val_time: " << run_results[loop].val_time << endl;
    cout << "get_time: " << run_results[loop].get_time << endl;
    cout << "join_time: " << run_results[loop].join_time << endl;
    cout << "index_build_time: " << run_results[loop].index_build_time << endl;
    cout << "partition_time: " << run_results[loop].partition_time << endl;
    cout << "sort_time: " << run_results[loop].sort_time << endl;
  }

  void ShowExpResult() {
    cout << "-------------------------" << endl;
    cout << "sum_join_read_io: " << sum_join_read_io << endl;
    cout << "sum_val_time: " << sum_val_time << endl;
    cout << "sum_get_time: " << sum_get_time << endl;
    cout << "sum_join_time: " << sum_join_time << endl;
    cout << "sum_index_build_time: " << sum_index_build_time << endl;
    cout << "sum_partition_time: " << sum_partition_time << endl;
    cout << "sum_sort_time: " << sum_sort_time << endl;
    cout << "-------------------------" << endl;
  }

  void WriteResultToFile(string file_name, string config_info) {
    // Write to file
    ofstream outfile;
    // if file does not exist, Create new file
    // else append to existing file
    outfile.open(file_name, ios::out | ios::app);
    outfile << "-------------------------" << endl;
    outfile << config_info << " ";
    outfile << "sum_join_time=" << sum_join_time << " ";
    outfile << "sum_index_build_time=" << sum_index_build_time << endl;
    outfile << "-------------------------" << endl;
    outfile.close();
  }

 private:
  ExpResult() {}
  uint64_t sum_join_read_io = 0;
  double sum_val_time = 0, sum_get_time = 0, sum_join_time = 0,
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
      return "Priamry";
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