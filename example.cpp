#include <chrono>
#include <iostream>
#include <limits>
#include <mutex>
#include <numeric>
#include <thread>

#include "ratelimiter.hpp"

int main() {
  using namespace std;
  using namespace std::chrono;

  using namespace std::chrono_literals;

  RateLimiter<std::chrono::milliseconds> limiter(100, 100);
  std::mutex mu;
  auto f = [&](int n) {
    limiter.Wait(steady_clock::now() + seconds(100));
  };

  auto start = std::chrono::steady_clock::now();
  constexpr LLONG LOOP_COUNT = 10000;
  // thread t[LOOP_COUNT];
  // for (int i = 0; i < LOOP_COUNT; i++) {
  //     t[i] = thread(f, i);
  // }
  // for (int i = 0; i < LOOP_COUNT; i++) {
  //     t[i].join();
  // }
  for (int i = 0; i < LOOP_COUNT; i++) {
    // limiter.Wait(steady_clock::now() + seconds(3));
    limiter.Allow();
  }
  // this_thread::sleep_for(2s);
  // for (int i = 0; i < LOOP_COUNT; i++) {
  //   // limiter.Wait(steady_clock::now() + seconds(3));
  //   cout << limiter.Allow() << endl;
  // }
  auto end = std::chrono::steady_clock::now();
  // cout << duration_cast<milliseconds>(end - start).count() << endl;
  return 0;
}