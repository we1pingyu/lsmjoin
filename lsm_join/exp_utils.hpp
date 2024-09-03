#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <mutex>
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

template <typename Value>
class Generator {
 public:
  virtual Value Next() = 0;
  virtual Value Last() = 0;
  virtual ~Generator() {}
};

class ZipfianGenerator : public Generator<uint64_t> {
 public:
  static const uint64_t kMaxNumItems = (UINT64_MAX >> 24);

  ZipfianGenerator(uint64_t min, uint64_t max, double zipfian_const)
      : num_items_(max - min + 1),
        base_(min),
        theta_(zipfian_const),
        zeta_n_(0),
        n_for_zeta_(0) {
    assert(num_items_ >= 2 && num_items_ < kMaxNumItems);
    zeta_2_ = Zeta(2, theta_);
    alpha_ = 1.0 / (1.0 - theta_);
    RaiseZeta(num_items_);
    eta_ = Eta();

    Next();
  }

  ZipfianGenerator(uint64_t num_items, float zipfian_const)
      : ZipfianGenerator(0, num_items - 1, zipfian_const) {}

  uint64_t Next(uint64_t num_items);

  uint64_t Next() { return Next(num_items_); }

  uint64_t Last();

 private:
  ///
  /// Compute the zeta constant needed for the distribution.
  /// Remember the number of items, so if it is changed, we can recompute zeta.
  ///
  void RaiseZeta(uint64_t num) {
    assert(num >= n_for_zeta_);
    zeta_n_ = Zeta(n_for_zeta_, num, theta_, zeta_n_);
    n_for_zeta_ = num;
  }

  double Eta() {
    return (1 - std::pow(2.0 / num_items_, 1 - theta_)) /
           (1 - zeta_2_ / zeta_n_);
  }

  double RandomDouble(double min = 0.0, double max = 1.0) {
    static std::default_random_engine generator(time(0));
    static std::uniform_real_distribution<double> uniform(min, max);
    return uniform(generator);
  }
  ///
  /// Calculate the zeta constant needed for a distribution.
  /// Do this incrementally from the last_num of items to the cur_num.
  /// Use the zipfian constant as theta. Remember the new number of items
  /// so that, if it is changed, we can recompute zeta.
  ///
  static double Zeta(uint64_t last_num, uint64_t cur_num, double theta,
                     double last_zeta) {
    double zeta = last_zeta;
    for (uint64_t i = last_num + 1; i <= cur_num; ++i) {
      zeta += 1 / std::pow(i, theta);
    }
    return zeta;
  }

  static double Zeta(uint64_t num, double theta) {
    return Zeta(0, num, theta, 0);
  }

  uint64_t num_items_;
  uint64_t base_;  /// Min number of items to generate

  // Computed parameters for generating the distribution
  double theta_, zeta_n_, eta_, alpha_, zeta_2_;
  uint64_t n_for_zeta_;  /// Number of items used to compute zeta_n
  uint64_t last_value_;
  std::mutex mutex_;
};

inline uint64_t ZipfianGenerator::Next(uint64_t num) {
  assert(num >= 2 && num < kMaxNumItems);
  // std::lock_guard<std::mutex> lock(mutex_);

  if (num > n_for_zeta_) {  // Recompute zeta_n and eta
    RaiseZeta(num);
    eta_ = Eta();
  }

  double u = RandomDouble();
  double uz = u * zeta_n_;

  if (uz < 1.0) {
    return last_value_ = 0;
  }

  if (uz < 1.0 + std::pow(0.5, theta_)) {
    return last_value_ = 1;
  }

  return last_value_ = base_ + num * std::pow(eta_ * u - eta_ + 1, alpha_);
}

inline uint64_t ZipfianGenerator::Last() {
  // std::lock_guard<std::mutex> lock(mutex_);
  return last_value_;
}

class ScrambledZipfianGenerator : public Generator<uint64_t> {
 public:
  ScrambledZipfianGenerator(uint64_t min, uint64_t max, double zipfian_const)
      : base_(min),
        num_items_(max - min + 1),
        generator_(min, max, zipfian_const) {}

  ScrambledZipfianGenerator(uint64_t num_items, double zipfian_const)
      : ScrambledZipfianGenerator(0, num_items - 1, zipfian_const) {}

  uint64_t Next();
  uint64_t Last();

 private:
  const uint64_t base_;
  const uint64_t num_items_;
  ZipfianGenerator generator_;

  uint64_t Scramble(uint64_t value) const;
};
const uint64_t kFNVOffsetBasis64 = 0xCBF29CE484222325;
const uint64_t kFNVPrime64 = 1099511628211;
inline uint64_t FNVHash64(uint64_t val) {
  uint64_t hash = kFNVOffsetBasis64;

  for (int i = 0; i < 8; i++) {
    uint64_t octet = val & 0x00ff;
    val = val >> 8;

    hash = hash ^ octet;
    hash = hash * kFNVPrime64;
  }
  return hash;
}
inline uint64_t ScrambledZipfianGenerator::Scramble(uint64_t value) const {
  return base_ + FNVHash64(value) % num_items_;
}

inline uint64_t ScrambledZipfianGenerator::Next() {
  return Scramble(generator_.Next());
}

inline uint64_t ScrambledZipfianGenerator::Last() {
  return Scramble(generator_.Last());
}

class YCSBGenerator {
 public:
  YCSBGenerator(int num_keys, std::string mode, double zipfian_skewness) {
    std::vector<int> v(num_keys);
    for (int i = 0; i < num_keys; ++i) {
      v[i] = i;
    }
    existing_keys = v;
    keys = v;
    key_gen = existing_keys.begin();

    dist_existing = new ScrambledZipfianGenerator(num_keys, zipfian_skewness);
    dist_new = new ScrambledZipfianGenerator(num_keys, zipfian_skewness);
  }

  ~YCSBGenerator() {
    delete dist_new;
    delete dist_existing;
  }

  std::string gen_key() {
    std::string key = std::to_string(*key_gen++);
    if (key_gen == existing_keys.end()) key_gen = existing_keys.begin();
    return key;
  }

  std::string gen_new_dup_key() {
    std::string key = std::to_string(existing_keys[dist_existing->Next() - 1]);
    int last_char = key.back() - '0';
    return key.substr(0, key.length() - 1) +
           static_cast<char>('A' + last_char + 17);
  }

  int gen_existing_key() {
    return existing_keys[dist_existing->Next() - 1];
  }

  std::string gen_val(size_t value_size) {
    return std::string(value_size, 'a');
  }

 private:
  std::string mode;
  std::mt19937 engine;
  std::vector<int>::iterator key_gen;
  std::vector<int> keys;
  std::vector<int> existing_keys;
  Generator<uint64_t> *dist_new;
  Generator<uint64_t> *dist_existing;
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
  double cpu_time;
  // double string_process_time;
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
    cpu_time = 0;
    index_build_time = 0;
    join_time = 0;
    partition_time = 0;
    sort_time = 0;
    cache_hit_rate = 0;
    false_positive_rate = 0;
    // string_process_time = 0;
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
    double cpu_time = run_result.join_time + run_result.index_build_time -
                      run_result.sync_time - run_result.eager_time -
                      run_result.update_time - run_result.get_data_time -
                      run_result.get_index_time - run_result.sort_io_time -
                      run_result.hash_io_time;
    sum_cpu_time += cpu_time;
    // sum_string_process_time += run_result.string_process_time;
    sum_other_cpu_time += (sum_cpu_time - sum_post_list_time -
                           sum_sort_cpu_time - sum_hash_cpu_time);
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
    cout << "sum_cpu_time: " << sum_cpu_time << " / ";
    cout << "cache_hit_rate: " << run_results[0].cache_hit_rate << " / ";
    // cout << "string_process_time: " << sum_string_process_time << " / ";
    cout << "sum_other_cpu_time: " << sum_other_cpu_time << " / ";
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
    outfile << "sum_cpu_time=" << sum_cpu_time << " ";
    outfile << "cache_hit_rate=" << run_results[0].cache_hit_rate << " ";
    outfile << "false_positive_rate=" << run_results[0].false_positive_rate
            << " ";
    // outfile << "string_process_time=" << sum_string_process_time << endl;
    outfile << "sum_other_cpu_time=" << sum_other_cpu_time << " ";
    std::string join_time_list = "[";
    std::string index_build_time_list = "[";
    for (auto &run_result : run_results) {
      join_time_list += std::to_string(run_result.join_time) + ",";
      index_build_time_list +=
          std::to_string(run_result.index_build_time) + ",";
    }
    join_time_list = join_time_list.substr(0, join_time_list.size() - 2) + "] ";
    outfile << "join_time_list=" << join_time_list;
    index_build_time_list =
        index_build_time_list.substr(0, index_build_time_list.size() - 2) +
        "] ";
    outfile << "index_build_time_list=" << index_build_time_list << endl;
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
         sum_index_build_time = 0, sum_partition_time = 0, sum_sort_time = 0,
         sum_cpu_time = 0, sum_other_cpu_time = 0;
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

enum JoinAlgorithm { INLJ, SJ, HJ, RJ };

string JoinAlgorithmToString(JoinAlgorithm join_algorithm) {
  switch (join_algorithm) {
    case JoinAlgorithm::INLJ:
      return "INLJ";
    case JoinAlgorithm::SJ:
      return "SJ";
    case JoinAlgorithm::HJ:
      return "HJ";
    case JoinAlgorithm::RJ:
      return "RJ";
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