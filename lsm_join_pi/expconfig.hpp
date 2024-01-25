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
  string index_type;  // index type
  uint64_t r_tuples;  // number of tuples in R, R is left relation
  uint64_t s_tuples;  // number of tuples in S, S is right relation
  string db_r;
  string db_s;

  // distribution parameters
  double eps;
  int k;
  int c;
  int M;                // memory buffer size
  int B;                // the num of entries in a block
  bool ingestion;       // whether to ingest data
  bool is_public_data;  // whether to use public data
  int PRIMARY_SIZE;
  int SECONDARY_SIZE;
  int VALUE_SIZE;
  int num_loop;
  int this_loop;

 private:
  ExpConfig()
      : r_tuples(2e7),
        s_tuples(1e7),
        eps(0.9),
        k(2),
        c(1),
        M(64),
        B(32),
        ingestion(false),
        PRIMARY_SIZE(10),
        SECONDARY_SIZE(10),
        is_public_data(false),
        num_loop(1),
        this_loop(0),
        VALUE_SIZE(4096 / B - PRIMARY_SIZE) {}
};

void parseCommandLine(int argc, char **argv) {
  ExpConfig &config = ExpConfig::getInstance();
  char junk;
  double m;
  uint64_t n;

  for (int i = 1; i < argc; i++) {
    if (strncmp(argv[i], "--index=", 8) == 0) {
      config.index_type = argv[i] + 8;
    } else if (sscanf(argv[i], "--s_tuples=%lf%c", &m, &junk) == 1) {
      config.s_tuples = m;
    } else if (sscanf(argv[i], "--r_tuples=%lf%c", &m, &junk) == 1) {
      config.r_tuples = m;
    } else if (sscanf(argv[i], "--epsilon=%lf%c", &m, &junk) == 1) {
      config.eps = m;
    } else if (sscanf(argv[i], "--k=%lu%c", (unsigned long *)&n, &junk) == 1) {
      config.k = n;
    } else if (sscanf(argv[i], "--c=%lu%c", (unsigned long *)&n, &junk) == 1) {
      config.c = n;
    } else if (sscanf(argv[i], "--M=%lu%c", (unsigned long *)&n, &junk) == 1) {
      config.M = n;
    } else if (sscanf(argv[i], "--B=%lu%c", (unsigned long *)&n, &junk) == 1) {
      config.B = n;
    } else if (strcmp(argv[i], "--ingestion") == 0) {
      config.ingestion = true;
    } else if (strcmp(argv[i], "--public_data") == 0) {
      config.is_public_data = true;
    } else if (strncmp(argv[i], "--db_r=", 7) == 0) {
      config.db_r = argv[i] + 7;
    } else if (strncmp(argv[i], "--db_s=", 7) == 0) {
      config.db_s = argv[i] + 7;
    } else if (sscanf(argv[i], "--num_loop=%lu%c", (unsigned long *)&n,
                      &junk) == 1) {
      config.num_loop = n;
    } else {
      cout << "Unrecognized command line argument " << argv[i] << endl;
      exit(1);
    }
  }
  config.VALUE_SIZE = 4096 / config.B - config.PRIMARY_SIZE;
  // output all config parameters
  cout << "index_type: " << config.index_type << endl;
  cout << "r_tuples: " << config.r_tuples << endl;
  cout << "s_tuples: " << config.s_tuples << endl;
  cout << "eps: " << config.eps << endl;
  cout << "k: " << config.k << endl;
  cout << "c: " << config.c << endl;
  cout << "M: " << config.M << endl;
  cout << "B: " << config.B << endl;
  cout << "ingestion: " << config.ingestion << endl;
  cout << "public_data: " << config.is_public_data << endl;
  cout << "db_r: " << config.db_r << endl;
  cout << "db_s: " << config.db_s << endl;
  cout << "num_loop: " << config.num_loop << endl;

  config.M <<= 20;
  config.r_tuples /= config.num_loop;
  config.s_tuples /= config.num_loop;
}