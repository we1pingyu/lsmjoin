#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>

using namespace std;

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