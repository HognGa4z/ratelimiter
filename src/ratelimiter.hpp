//@author hankinzhang

#pragma once

#include <chrono>
#include <limits>
#include <mutex>
#include <ratio>
#include <shared_mutex>
#include <atomic>
#include <string>
#include <thread>
#include <algorithm>

#include "utils.hpp"

using LLONG = long long;
using TIME_DURATION = long long;
using TIME_POINT = std::chrono::steady_clock::time_point;
using NANO_UNIT = std::chrono::nanoseconds;

template <typename T = std::chrono::seconds>
class RateLimiter {
 private:
  std::mutex mu_;

  LLONG limit_;
  int burst_;
  std::atomic<LLONG> token_;

  TIME_DURATION const_token_supply_unit;

  TIME_POINT last_;

 public:
  explicit RateLimiter(LLONG limit, int burst);
  ~RateLimiter(){};

  // Block
  void Wait(TIME_POINT timeout);
  void WaitN(TIME_POINT timeout, int n);

  // Return
  bool Allow();
  bool AllowN(TIME_POINT now, int n);

  void SetLimit(LLONG limit);
  void SetBurst(int n);

 private:
  void supplyTokens(TIME_POINT now);
  TIME_DURATION timeDuration(TIME_POINT a, TIME_POINT b);
};

template <typename T>
RateLimiter<T>::RateLimiter(LLONG limit, int burst):
  limit_(limit), burst_(burst), token_(burst){
    T unit(limit_);
    const_token_supply_unit = NANO_UNIT(unit).count();
}

template <typename T>
void RateLimiter<T>::WaitN(TIME_POINT timeout, int n) {
  bool getToken = false;
  while (1) {
    auto now = std::chrono::steady_clock::now();
    if (now >= timeout) {
      return;
    }
    getToken = AllowN(now, n);
    if (getToken) {
      return;
    }
    std::this_thread::yield();
  }
}

template <typename T>
void RateLimiter<T>::Wait(TIME_POINT timeout) {
  WaitN(timeout, 1);
}

template <typename T>
bool RateLimiter<T>::AllowN(TIME_POINT now, int n) {
  auto start_time = std::chrono::steady_clock::now();
  //defer(dry::utils::TimeConsume(start_time););

  supplyTokens(now);

  // std::lock_guard<std::mutex> guard(mu_);
  auto token_left = token_.load();
  if (token_left < n) {
    return false;
  }

  LLONG token_left_new;
  do {
    token_left_new = token_left - n;
  } while(!token_.compare_exchange_weak(token_left, token_left_new));
  return true;
}

template <typename T>
bool RateLimiter<T>::Allow() {
  return AllowN(std::chrono::steady_clock::now(), 1);
}

template <typename T>
void RateLimiter<T>::SetLimit(LLONG limit) {
}

template <typename T>
void RateLimiter<T>::SetBurst(int n) {
}

template <typename T>
void RateLimiter<T>::supplyTokens(TIME_POINT now) {
  if (timeDuration(now, last_) < const_token_supply_unit) {
    return;
  }

  // std::lock_guard<std::mutex> guard(mu_);
  LLONG new_tokens = timeDuration(now, last_) / const_token_supply_unit;
  if (new_tokens <= 0) {
    return;
  }

  last_ += NANO_UNIT(new_tokens * const_token_supply_unit);
  LLONG token_left_old = token_.load();

  LLONG token_free = burst_ - token_left_old;
  if (new_tokens > token_free || new_tokens > burst_) {
    new_tokens = std::min<LLONG>(burst_, token_free);
  }

  LLONG token_left_new;
  do {
    token_left_new = token_left_old + new_tokens;
  } while(!token_.compare_exchange_weak(token_left_old, token_left_new));
}

template <typename T>
TIME_DURATION RateLimiter<T>::timeDuration(TIME_POINT a, TIME_POINT b) {
  return std::chrono::duration_cast<NANO_UNIT>(a - b).count();
}
