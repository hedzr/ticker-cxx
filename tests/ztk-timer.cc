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

void test_timer() {
  using namespace std::literals::chrono_literals;
  ticker::debug::X x_local_var;

  ticker::pool::conditional_wait_for_int count{1};
  auto t = ticker::timer<>::get();
#if !TICKER_CXX_ENABLE_THREAD_POOL_READY_SIGNAL
  std::this_thread::sleep_for(300ms);
#endif

  dbg_print("  - start at: %s", ticker::chrono::format_time_point().c_str());
  t->after(1us)
      .on([&count] {
        auto now = ticker::chrono::now();
        ticker::pool::cw_setter cws(count);
        // std::time_t ct = std::time(0);
        // char *cc = ctime(&ct);
        printf("  - after [%02d]: %s\n", count.val(), ticker::chrono::format_time_point(now).c_str());
      })
      .build();

  printf("count.wait()\n");
  count.wait_for();
  // t.clear();
  printf("end of %s\n", __FUNCTION_NAME__);
}

int main() {
  TICKER_TEST_FOR(test_timer);
}
