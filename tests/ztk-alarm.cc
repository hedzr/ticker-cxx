// ticker_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/26.
//

// #define TICKER_CXX_TEST_THREAD_POOL_DBGOUT 1

// #define TICKER_CXX_ENABLE_THREAD_POOL_READY_SIGNAL 1

#include "ticker_cxx/ticker-core.hh"
#include "ticker_cxx/ticker-x-class.hh"
#include "ticker_cxx/ticker-x-test.hh"

#include <chrono>
#include <iostream>
#include <thread>

ticker::debug::X x_global_var;

void test_alarm() {
  using namespace std::literals::chrono_literals;
  ticker::debug::X x_local_var;
  ticker::pool::conditional_wait_for_int count2{4};
  ticker::alarm_t<>::super::get()
      ->every_month(3)
      .on([&count2] {
        auto now = ticker::chrono::now();
        ticker::pool::cw_setter cws(count2);
        printf("  - alarm [%02d]: %s\n", count2.val(),
               ticker::chrono::format_time_point(now).c_str());
      })
      .build();
  printf("end of %s\n", __FUNCTION_NAME__);
}

int main() {
  TICKER_TEST_FOR(test_alarm);
}
