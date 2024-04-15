#include "exp_utils.hpp"

int main(int argc, char* argv[]) {
  // 1 million
  int num = 4797969;
  Timer timer = Timer();
  for (int i = 0; i < num; i++) {
    Timer timer_test = Timer();
    timer_test.elapsed();
  }

  cout << "Time taken: " << timer.elapsed() << " seconds" << endl;
}
