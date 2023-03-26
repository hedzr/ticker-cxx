// ticker_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/24.
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

void test_ticker() {
  using namespace std::literals::chrono_literals;
  ticker::debug::X x_local_var;

  ticker::pool::conditional_wait_for_int count{16};
  auto t = ticker::ticker<>::get();
#if !TICKER_CXX_ENABLE_THREAD_POOL_READY_SIGNAL
  std::this_thread::sleep_for(300ms);
#endif

  dbg_print("  - start at: %s", ticker::chrono::format_time_point().c_str());
  t->every(1us)
      .on([&count]() {
        // auto now = ticker::chrono::now();
        ticker::pool::cw_setter cws(count);
        printf("  - every [%02d]: %s\n", count.val(), ticker::chrono::format_time_point().c_str());
      })
      .build();

  count.wait();
  // t.clear();
  printf("end of %s\n", __FUNCTION_NAME__);
}

void test_ticker_interval() {
  using namespace std::literals::chrono_literals;
  ticker::debug::X x_local_var;

  ticker::pool::conditional_wait_for_int count2{4};
  auto t = ticker::ticker<>::get([] { dbg_print("  - start at: %s", ticker::chrono::format_time_point().c_str()); });
  t->interval(200ms)
      .on([&count2] {
        ticker::pool::cw_setter cws(count2); // set conditional_wait object at destructed time.
        printf("  - interval [%02d]: %s\n", count2.val(), ticker::chrono::format_time_point().c_str());
      })
      .build();

  count2.wait();
  printf("end of %s\n", __FUNCTION_NAME__);
}

int main() {

  TICKER_TEST_FOR(test_ticker);
  TICKER_TEST_FOR(test_ticker_interval);

  // TICKER_TEST_FOR(test_alarm);

  // using namespace std::literals::chrono_literals;
  // std::this_thread::sleep_for(200ns);
  //
  // {
  //     std::time_t ct = std::time(0);
  //     char *cc = ctime(&ct);
  //     printf(". now: %s\n", cc);
  // }
  // {
  //     auto t = std::time(nullptr);
  //     auto tm = *std::localtime(&t);
  //     std::cout << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << '\n';
  // }

  return 0;
}