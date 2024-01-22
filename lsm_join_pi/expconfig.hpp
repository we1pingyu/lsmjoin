#pragma once

#include <algorithm>
#include <boost/program_options.hpp>
#include <fstream>
#include <iostream>
#include <random>
#include <regex>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

using namespace std;
namespace po = boost::program_options;

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

  // Define the supported options.
  po::options_description desc("Allowed options");
  desc.add_options()("index", po::value<string>(&config.index_type),
                     "Index type")(
      "s_tuples", po::value<uint64_t>(&config.s_tuples), "S Tuples")(
      "r_tuples", po::value<uint64_t>(&config.r_tuples), "R Tuples")(
      "epsilon", po::value<double>(&config.eps), "Eps value")(
      "k", po::value<int>(&config.k), "K value")("c", po::value<int>(&config.c),
                                                 "C value")(
      "M", po::value<int>(&config.M), "M value")("B", po::value<int>(&config.B),
                                                 "B value")(
      "ingestion", po::bool_switch(&config.ingestion), "Ingestion flag");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  // output all config parameters
  cout << "index_type: " << config.index_type << endl;
  cout << "r_tuples: " << config.r_tuples << endl;
  cout << "s_tuples: " << config.s_tuples << endl;
  cout << "eps: " << config.eps << endl;
  cout << "k: " << config.k << endl;
  cout << "c: " << config.c << endl;
  cout << "M: " << config.M << endl;
  cout << "B: " << config.B << endl;
  cout << "ingestion: " << (config.ingestion ? "true" : "false") << endl;

  config.M <<= 20;
}