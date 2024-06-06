#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <regex>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

#include "exp_utils.hpp"

using namespace std;

class ExpConfig {
 public:
  static ExpConfig &getInstance() {
    static ExpConfig instance;
    return instance;
  }

  // forbid copy and move
  ExpConfig(ExpConfig const &) = delete;
  void operator=(ExpConfig const &) = delete;

  // config parameters
  IndexType r_index;  // index type
  IndexType s_index;
  JoinAlgorithm join_algorithm;
  uint64_t r_tuples;  // number of tuples in R, R is left relation
  uint64_t s_tuples;  // number of tuples in S, S is right relation
  string db_r;
  string db_s;
  string public_r;
  string public_s;
  string r_index_path;
  string s_index_path;
  string output_file;

  // distribution parameters
  double eps_s;
  double k_r;
  double k_s;
  int c_r;
  int c_s;
  int M;                // memory buffer size
  int B;                // the num of entries in a block
  int T;                // size ratio of LSM-tree
  int K;                // number of runs in a level
  bool skip_ingestion;  // whether to skeip ingestion
  bool is_public_data;  // whether to use public data
  bool skew;            // whether to use skew distribution
  bool theory;          // whether to use theoretical model
  bool skip_join;       // whether to skip join
  bool direct_io;       // whether to use direct IO
  bool noncovering;     // whether to use non-covering index
  int bpk;              // bits per key
  int cache_size;
  int PRIMARY_SIZE;
  int SECONDARY_SIZE;
  int VALUE_SIZE;
  int num_loop;
  int this_loop;
  int page_size;

  string ToString() {
    string str = "";
    str += "r_index=" + IndexTypeToString(r_index) + " ";
    str += "s_index=" + IndexTypeToString(s_index) + " ";
    str += "join_algorithm=" + JoinAlgorithmToString(join_algorithm) + " ";
    str += "r_tuples=" + to_string(r_tuples * num_loop) + " ";
    str += "s_tuples=" + to_string(s_tuples * num_loop) + " ";
    str += "db_r=" + db_r + " ";
    str += "db_s=" + db_s + " ";
    str += "r_index_path=" + r_index_path + " ";
    str += "s_index_path=" + s_index_path + " ";
    str += "eps_s=" + to_string(eps_s) + " ";
    str += "k_r=" + to_string(k_r) + " ";
    str += "k_s=" + to_string(k_s) + " ";
    str += "c_r=" + to_string(c_r) + " ";
    str += "c_s=" + to_string(c_s) + " ";
    str += "M=" + to_string(M) + " ";
    str += "B=" + to_string(B) + " ";
    str += "T=" + to_string(T) + " ";
    str += "K=" + to_string(K) + " ";
    str += "skip_ingestion=" + to_string(skip_ingestion) + " ";
    str += "is_public_data=" + to_string(is_public_data) + " ";
    str += "theory=" + to_string(theory) + " ";
    str += "skew=" + to_string(skew) + " ";
    str += "public_r=" + public_r + " ";
    str += "public_s=" + public_s + " ";
    str += "num_loop=" + to_string(num_loop) + " ";
    str += "cache_size=" + to_string(cache_size) + " ";
    str += "bpk=" + to_string(bpk) + " ";
    str += "page_size=" + to_string(page_size) + " ";
    str += "skip_join=" + to_string(skip_join) + " ";
    str += "direct_io=" + to_string(direct_io) + " ";
    str += "noncovering=" + to_string(noncovering) + " ";
    return str;
  };

 private:
  ExpConfig()
      : r_tuples(1e7),
        s_tuples(1e7),
        eps_s(0.4),
        k_r(4.0),
        k_s(4.0),
        c_r(1),
        c_s(1),
        M(16),
        B(64),
        T(5),
        K(2),
        skip_ingestion(false),
        PRIMARY_SIZE(10),
        SECONDARY_SIZE(10),
        is_public_data(false),
        direct_io(false),
        skew(false),
        num_loop(1),
        cache_size(0),
        bpk(10.0),
        page_size(4096),
        this_loop(0),
        theory(false),
        skip_join(false),
        noncovering(false),
        r_index_path("/tmp/R_index"),
        s_index_path("/tmp/S_index"),
        db_r("/tmp/R_DB"),
        db_s("/tmp/S_DB"),
        output_file("output.txt"),
        VALUE_SIZE() {}
};

void parseCommandLine(int argc, char **argv) {
  ExpConfig &config = ExpConfig::getInstance();
  char junk;
  double m;
  uint64_t n;
  string r_index, s_index, join_algorithm;

  for (int i = 1; i < argc; i++) {
    if (strncmp(argv[i], "--r_index=", 10) == 0) {
      r_index = argv[i] + 10;
      config.r_index = StringToIndexType(r_index);
    } else if (strncmp(argv[i], "--s_index=", 10) == 0) {
      s_index = argv[i] + 10;
      config.s_index = StringToIndexType(s_index);
    } else if (strncmp(argv[i], "--J=", 4) == 0) {
      join_algorithm = argv[i] + 4;
      config.join_algorithm = StringToJoinAlgorithm(join_algorithm);
    } else if (sscanf(argv[i], "--s_tuples=%lf%c", &m, &junk) == 1) {
      config.s_tuples = m;
    } else if (sscanf(argv[i], "--r_tuples=%lf%c", &m, &junk) == 1) {
      config.r_tuples = m;
    } else if (sscanf(argv[i], "--eps_s=%lf%c", &m, &junk) == 1) {
      config.eps_s = m;
    } else if (sscanf(argv[i], "--k_r=%lf%c", &m, &junk) == 1) {
      config.k_r = m;
    } else if (sscanf(argv[i], "--k_s=%lf%c", &m, &junk) == 1) {
      config.k_s = m;
    } else if (sscanf(argv[i], "--c_r=%lu%c", (unsigned long *)&n, &junk) ==
               1) {
      config.c_r = n;
    } else if (sscanf(argv[i], "--c_s=%lu%c", (unsigned long *)&n, &junk) ==
               1) {
      config.c_s = n;
    } else if (sscanf(argv[i], "--M=%lu%c", (unsigned long *)&n, &junk) == 1) {
      config.M = n;
    } else if (sscanf(argv[i], "--B=%lu%c", (unsigned long *)&n, &junk) == 1) {
      config.B = n;
    } else if (sscanf(argv[i], "--T=%lu%c", (unsigned long *)&n, &junk) == 1) {
      config.T = n;
    } else if (sscanf(argv[i], "--K=%lu%c", (unsigned long *)&n, &junk) == 1) {
      config.K = n;
    } else if (sscanf(argv[i], "--bpk=%lu%c", (unsigned long *)&n, &junk) ==
               1) {
      config.bpk = n;
    } else if (strcmp(argv[i], "--skip_ingestion") == 0) {
      config.skip_ingestion = true;
    } else if (strcmp(argv[i], "--skip_join") == 0) {
      config.skip_join = true;
    } else if (strcmp(argv[i], "--direct_io") == 0) {
      config.direct_io = true;
    } else if (strcmp(argv[i], "--public_data") == 0) {
      config.is_public_data = true;
    } else if (strcmp(argv[i], "--skew") == 0) {
      config.skew = true;
    } else if (strcmp(argv[i], "--theory") == 0) {
      config.theory = true;
    } else if (strcmp(argv[i], "--noncovering") == 0) {
      config.noncovering = true;
    } else if (strncmp(argv[i], "--public_r=", 11) == 0) {
      config.public_r = argv[i] + 11;
    } else if (strncmp(argv[i], "--public_s=", 11) == 0) {
      config.public_s = argv[i] + 11;
    } else if (strncmp(argv[i], "--r_index_path=", 15) == 0) {
      config.r_index_path = argv[i] + 15;
    } else if (strncmp(argv[i], "--s_index_path=", 15) == 0) {
      config.s_index_path = argv[i] + 15;
    } else if (strncmp(argv[i], "--db_r=", 7) == 0) {
      config.db_r = argv[i] + 7;
    } else if (strncmp(argv[i], "--db_s=", 7) == 0) {
      config.db_s = argv[i] + 7;
    } else if (sscanf(argv[i], "--num_loop=%lu%c", (unsigned long *)&n,
                      &junk) == 1) {
      config.num_loop = n;
    } else if (sscanf(argv[i], "--cache_size=%lu%c", (unsigned long *)&n,
                      &junk) == 1) {
      config.cache_size = n;
    } else if (strncmp(argv[i], "--output_file=", 14) == 0) {
      config.output_file = argv[i] + 14;
    } else if (sscanf(argv[i], "--page_size=%lu%c", (unsigned long *)&n,
                      &junk) == 1) {
      config.page_size = n;
    } else {
      cout << "Unrecognized command line argument " << argv[i] << endl;
      exit(1);
    }
  }

  // check if all essential arguments are set
  string essential_args[] = {"r_index", "s_index", "J"};
  for (auto arg : essential_args) {
    if (arg == "r_index" && r_index == "") {
      cout << "r_index is not set" << endl;
      exit(1);
    } else if (arg == "s_index" && s_index == "") {
      cout << "s_index is not set" << endl;
      exit(1);
    } else if (arg == "J" && join_algorithm == "") {
      cout << "Join Algorithm is not set" << endl;
      exit(1);
    }
  }

  // output all config parameters
  cout << "r_index: " << IndexTypeToString(config.r_index) << " / ";
  cout << "s_index: " << IndexTypeToString(config.s_index) << " / ";
  cout << "join_algorithm: " << JoinAlgorithmToString(config.join_algorithm)
       << " / ";
  cout << "r_tuples: " << config.r_tuples * config.num_loop << " / ";
  cout << "s_tuples: " << config.s_tuples * config.num_loop << " / ";
  cout << "eps_s: " << config.eps_s << " / ";
  cout << "k_r: " << config.k_r << " / ";
  cout << "k_s: " << config.k_s << " / ";
  cout << "c_r: " << config.c_r << " / ";
  cout << "c_s: " << config.c_r << " / ";
  cout << "M: " << config.M << " / ";
  cout << "B: " << config.B << " / ";
  cout << "T: " << config.T << " / ";
  cout << "K: " << config.K << " / ";
  cout << "skip_ingestion: " << config.skip_ingestion << " / ";
  cout << "direct_io: " << config.direct_io << " / ";
  cout << "skip_join: " << config.skip_join << " / ";
  cout << "public_data: " << config.is_public_data << " / ";
  cout << "noncovering: " << config.noncovering << " / ";
  cout << "skew: " << config.skew << " / ";
  cout << "public_r: " << config.public_r << " / ";
  cout << "public_s: " << config.public_s << " / ";
  cout << "db_r: " << config.db_r << " / ";
  cout << "db_s: " << config.db_s << " / ";
  cout << "r_index_path: " << config.r_index_path << " / ";
  cout << "s_index_path: " << config.s_index_path << " / ";
  cout << "num_loop: " << config.num_loop << " / ";
  cout << "output_file: " << config.output_file << " / ";
  cout << "cache_size: " << config.cache_size << " / ";
  cout << "bpk: " << config.bpk << " / ";
  cout << "page_size: " << config.page_size << endl;

  config.M <<= 20;
  config.VALUE_SIZE = 4096 / config.B - config.PRIMARY_SIZE;
  config.VALUE_SIZE = int(0.95 * config.VALUE_SIZE);  // make room for metadata
  config.cache_size <<= 20;
  config.r_tuples /= config.num_loop;
  config.s_tuples /= config.num_loop;
}