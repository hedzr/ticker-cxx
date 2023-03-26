// ticker_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/26.
//

#include "ticker_cxx/ticker-periodical-job.hh"
#include "ticker_cxx/ticker-x-class.hh"
#include "ticker_cxx/ticker-x-test.hh"

#include <chrono>
#include <iostream>
#include <thread>

ticker::debug::X x_global_var;

void foo1() {
  dbg_print("foo1 hit.");
}

void test_periodical_job() {
  // namespace chr = ticker::chrono;
  // namespace dtl = ticker::chrono::detail;
  using clock = std::chrono::system_clock;
  using time_point = clock::time_point;
  using namespace std::literals::chrono_literals;
  using ahr = ticker::anchors;

  struct testcase {
    const char *desc;
    ahr anchor;
    int offset;
    int ordinal;
    int times;
    time_point now, expected;
  };

#define CASE(desc, anchor, ofs) \
  testcase { desc, anchor, ofs, 1, -1, time_point(), time_point() }
#define CASE1(desc, anchor, ofs, ord) \
  testcase { desc, anchor, ofs, ord, -1, time_point(), time_point() }
#define CASE2(desc, anchor, ofs, ord, times) \
  testcase { desc, anchor, ofs, ord, times, time_point(), time_point() }
#define NOW_CASE(now_str, expected_str, desc, anchor, ofs) \
  testcase { desc, anchor, ofs, 1, -1, ticker::chrono::parse_datetime(now_str), ticker::chrono::parse_datetime(expected_str) }

  for (auto const &t : {
           // Month
           NOW_CASE("2021-08-05", "2021-09-03", "day 3 every month", ahr::Month, 3),
           NOW_CASE("2021-08-05", "2021-08-23", "day 23 every month", ahr::Month, 23),
           NOW_CASE("2021-08-05", "2021-08-29", "day -3 every month", ahr::Month, -3),
           NOW_CASE("2021-08-05", "2021-08-17", "day -15 every month", ahr::Month, -15),
           NOW_CASE("2021-08-17", "2021-09-17", "day -15 every month", ahr::Month, -15),

           // Year
           NOW_CASE("2021-08-05", "2022-08-03", "day 3 (this month) every year", ahr::Year, 3),
           NOW_CASE("2021-08-05", "2021-08-23", "day 23 (this month) every year", ahr::Year, 23),
           NOW_CASE("2021-08-05", "2021-12-29", "day -3 (this month) every year", ahr::Year, -3),
           NOW_CASE("2021-08-05", "2021-12-17", "day -15 (this month) every year", ahr::Year, -15),
           NOW_CASE("2021-08-18", "2021-12-17", "day -15 (this month) every year", ahr::Year, -15),

           // FirstThirdOfMonth ...
           NOW_CASE("2021-08-05", "2021-09-03", "day 3 every first third of month", ahr::FirstThirdOfMonth, 3),
           NOW_CASE("2021-08-05", "2021-08-08", "day 8 every first third of month", ahr::FirstThirdOfMonth, 8),
           NOW_CASE("2021-08-05", "2021-08-08", "day -3 every first third of month", ahr::FirstThirdOfMonth, -3),
           NOW_CASE("2021-08-05", "2021-09-04", "day -7 every first third of month", ahr::FirstThirdOfMonth, -7),

           // MiddleThirdOfMonth ...
           NOW_CASE("2021-08-15", "2021-09-13", "day 3 every mid third of month", ahr::MiddleThirdOfMonth, 3),
           NOW_CASE("2021-08-15", "2021-08-18", "day 8 every mid third of month", ahr::MiddleThirdOfMonth, 8),
           NOW_CASE("2021-08-15", "2021-08-18", "day -3 every mid third of month", ahr::MiddleThirdOfMonth, -3),
           NOW_CASE("2021-08-15", "2021-09-14", "day -7 every mid third of month", ahr::MiddleThirdOfMonth, -7),

           // LastThirdOfMonth ...
           NOW_CASE("2021-08-25", "2021-09-23", "day 3 every last third of month", ahr::LastThirdOfMonth, 3),
           NOW_CASE("2021-08-25", "2021-08-28", "day 8 every last third of month", ahr::LastThirdOfMonth, 8),
           NOW_CASE("2021-08-25", "2021-08-29", "day -3 every last third of month", ahr::LastThirdOfMonth, -3),
           NOW_CASE("2021-08-25", "2021-09-23", "day -9 every last third of month", ahr::LastThirdOfMonth, -9),
           NOW_CASE("2021-08-22", "2021-08-23", "day -9 every last third of month", ahr::LastThirdOfMonth, -9),

       }) {
    ticker::detail::periodical_job pj(t.anchor, t.ordinal, t.offset, t.times, foo1);
    auto now = t.now;
    if (ticker::chrono::duration_is_zero(now))
      now = ticker::chrono::now();
    else
      pj.last_pt = now;
    auto pt = pj.next_time_point(now);
    dbg_print("%40s: %s -> %s", t.desc, ticker::chrono::format_time_point_to_local(now).c_str(), ticker::chrono::format_time_point_to_local(pt).c_str());

    auto tmp = t.expected;
    if (!ticker::chrono::duration_is_zero(tmp)) {
      if (ticker::chrono::compare_date_part(pt, tmp) != 0) {
        dbg_print("%40s: ERROR: expecting %s but got %s", " ", ticker::chrono::format_time_point_to_local(tmp).c_str(), ticker::chrono::format_time_point_to_local(pt).c_str());
        exit(-1);
      }
    }
  }

#undef NOW_CASE
}

int main() {
  // test_thread();

  TICKER_TEST_FOR(test_periodical_job);
}