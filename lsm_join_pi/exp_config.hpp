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
  double eps;
  double k;
  int c;
  int M;                // memory buffer size
  int B;                // the num of entries in a block
  int T;                // size ratio of LSM-tree
  bool ingestion;       // whether to ingest data
  bool is_public_data;  // whether to use public data
  bool uniform;         // whether to use uniform distribution
  bool use_cache;
  int PRIMARY_SIZE;
  int SECONDARY_SIZE;
  int VALUE_SIZE;
  int num_loop;
  int this_loop;
  int page_size;

  string GetTimeStamp() {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    string str = "";
    str += to_string(1900 + ltm->tm_year);
    str += to_string(1 + ltm->tm_mon);
    str += to_string(ltm->tm_mday);
    str += to_string(ltm->tm_hour);
    str += to_string(ltm->tm_min);
    str += to_string(ltm->tm_sec);
    return str;
  }

  string ToString() {
    string str = "";
    str += "r_index=" + IndexTypeToString(r_index) + " ";
    str += "s_index=" + IndexTypeToString(s_index) + " ";
    str += "join_algorithm=" + JoinAlgorithmToString(join_algorithm) + " ";
    str += "r_tuples=" + to_string(r_tuples) + " ";
    str += "s_tuples=" + to_string(s_tuples) + " ";
    str += "db_r=" + db_r + " ";
    str += "db_s=" + db_s + " ";
    str += "r_index_path=" + r_index_path + " ";
    str += "s_index_path=" + s_index_path + " ";
    str += "eps=" + to_string(eps) + " ";
    str += "k=" + to_string(k) + " ";
    str += "c=" + to_string(c) + " ";
    str += "M=" + to_string(M) + " ";
    str += "B=" + to_string(B) + " ";
    str += "T=" + to_string(T) + " ";
    str += "ingestion=" + to_string(ingestion) + " ";
    str += "is_public_data=" + to_string(is_public_data) + " ";
    str += "unifrom=" + to_string(uniform) + " ";
    str += "public_r=" + public_r + " ";
    str += "public_s=" + public_s + " ";
    str += "num_loop=" + to_string(num_loop) + " ";
    str += "use_cache=" + to_string(use_cache) + " ";
    str += "page_size=" + to_string(page_size) + " ";
    return str;
  };

 private:
  ExpConfig()
      : r_tuples(1e7),
        s_tuples(1e7),
        eps(0.9),
        k(1.0),
        c(1),
        M(64),
        B(128),
        T(5),
        ingestion(false),
        PRIMARY_SIZE(10),
        SECONDARY_SIZE(10),
        is_public_data(false),
        uniform(false),
        num_loop(1),
        use_cache(false),
        page_size(4096),
        this_loop(0),
        r_index_path("/tmp/R_index_" + GetTimeStamp()),
        s_index_path("/tmp/S_index_" + GetTimeStamp()),
        db_r("/tmp/R_DB_" + GetTimeStamp()),
        db_s("/tmp/S_DB_" + GetTimeStamp()),
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
    } else if (sscanf(argv[i], "--epsilon=%lf%c", &m, &junk) == 1) {
      config.eps = m;
    } else if (sscanf(argv[i], "--k=%lf%c", &m, &junk) == 1) {
      config.k = m;
    } else if (sscanf(argv[i], "--c=%lu%c", (unsigned long *)&n, &junk) == 1) {
      config.c = n;
    } else if (sscanf(argv[i], "--M=%lu%c", (unsigned long *)&n, &junk) == 1) {
      config.M = n;
    } else if (sscanf(argv[i], "--B=%lu%c", (unsigned long *)&n, &junk) == 1) {
      config.B = n;
    } else if (sscanf(argv[i], "--T=%lu%c", (unsigned long *)&n, &junk) == 1) {
      config.T = n;
    } else if (strcmp(argv[i], "--ingestion") == 0) {
      config.ingestion = true;
    } else if (strcmp(argv[i], "--public_data") == 0) {
      config.is_public_data = true;
    } else if (strcmp(argv[i], "--uniform") == 0) {
      config.uniform = true;
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
    } else if (strcmp(argv[i], "--use_cache") == 0) {
      config.use_cache = true;
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
  cout << "r_index: " << IndexTypeToString(config.r_index) << endl;
  cout << "s_index: " << IndexTypeToString(config.s_index) << endl;
  cout << "join_algorithm: " << JoinAlgorithmToString(config.join_algorithm)
       << endl;
  cout << "r_tuples: " << config.r_tuples << endl;
  cout << "s_tuples: " << config.s_tuples << endl;
  cout << "eps: " << config.eps << endl;
  cout << "k: " << config.k << endl;
  cout << "c: " << config.c << endl;
  cout << "M: " << config.M << endl;
  cout << "B: " << config.B << endl;
  cout << "T: " << config.T << endl;
  cout << "ingestion: " << config.ingestion << endl;
  cout << "public_data: " << config.is_public_data << endl;
  cout << "uniform: " << config.uniform << endl;
  cout << "public_r: " << config.public_r << endl;
  cout << "public_s: " << config.public_s << endl;
  cout << "db_r: " << config.db_r << endl;
  cout << "db_s: " << config.db_s << endl;
  cout << "r_index_path: " << config.r_index_path << endl;
  cout << "s_index_path: " << config.s_index_path << endl;
  cout << "num_loop: " << config.num_loop << endl;
  cout << "output_file: " << config.output_file << endl;
  cout << "use_cache: " << config.use_cache << endl;
  cout << "page_size: " << config.page_size << endl;

  config.M <<= 20;
  config.VALUE_SIZE = 4096 / config.B - config.PRIMARY_SIZE;
  config.r_tuples /= config.num_loop;
  config.s_tuples /= config.num_loop;
}