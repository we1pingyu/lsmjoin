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
  std::string index_type;  // index type
  uint64_t r_tuples;       // number of tuples in R, R is left relation
  uint64_t s_tuples;       // number of tuples in S, S is right relation
  // distribution parameters
  double eps;
  int k;
  int c;
  int M;           // memory buffer size
  int B;           // the num of entries in a block
  bool ingestion;  // whether to ingest data
  int PRIMARY_SIZE;
  int SECONDARY_SIZE;
  int VALUE_SIZE;

 private:
  ExpConfig()
      : index_type(""),
        r_tuples(1e7),
        s_tuples(2e7),
        eps(0.9),
        k(2),
        c(1),
        M(32 << 20),
        B(32),
        ingestion(false),
        PRIMARY_SIZE(10),
        SECONDARY_SIZE(10),
        VALUE_SIZE(4096 / B - SECONDARY_SIZE) {}
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
    }
  }

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

  config.M <<= 20;
}