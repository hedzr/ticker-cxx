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

#include "ticker_cxx/ticker.hh"
#include "ticker_cxx/ticker-x-class.hh"
#include "ticker_cxx/ticker-x-test.hh"

#include <chrono>
#include <iostream>
#include <thread>

ticker::debug::X x_global_var;

namespace test {
    template<typename T>
    constexpr auto ZFN() {
        constexpr auto function = std::string_view{__FUNCTION_NAME__};
        constexpr auto prefix = std::string_view{"auto ZFN() [T = "};
        constexpr auto suffix = std::string_view{"]"};

        constexpr auto start = function.find(prefix) + prefix.size();
        constexpr auto end = function.rfind(suffix);
        std::string_view name = function.substr(start, (end - start));
        return name.substr(0, (end - start));
        // return function;
    }

    template<typename T>
    constexpr auto ZFS() {
        constexpr auto function = std::string_view{__FUNCTION_NAME__};
        constexpr auto prefix = std::string_view{"auto ZFT() [T = "};
        // constexpr auto suffix = std::string_view{"]"};

        constexpr auto start = function.find(prefix) + prefix.size();
        // constexpr auto end = function.rfind(suffix);
        return (unsigned long long) start; // function.substr(start, (end - start));
        // return function;
    }

    template<typename T>
    constexpr auto ZFT() {
        constexpr auto function = std::string_view{__FUNCTION_NAME__};
        // constexpr auto prefix = std::string_view{"auto ZFT() [T = "};
        constexpr auto suffix = std::string_view{"]"};

        // constexpr auto start = function.find(prefix) + prefix.size();
        constexpr auto end = function.rfind(suffix);
        return (unsigned long long) end; // function.substr(start, (end - start));
        // return function;
    }

    template<typename T>
    constexpr auto ZFZ() {
        // auto ZFZ() [T = std::__1::basic_string<char>]
        constexpr auto function = std::string_view{__FUNCTION_NAME__};
        return function;
    }
} // namespace test

void test_type_name() {
    printf(">>Z '%s'\n", test::ZFZ<std::string>().data());
    printf(">>Z '%s'\n", test::ZFN<std::string>().data());
    printf(">>Z %llu, %llu\n", test::ZFS<std::string>(), test::ZFT<std::string>());

#ifndef _WIN32
    // printf(">>2 '%s'\n", ticker::debug::type_name_holder<std::string>::value.data());
    //
    // printf(">>1 '%s'\n", ticker::debug::type_name_1<ticker::pool::conditional_wait_for_int>().data());
    // printf(">>> '%s'\n", ticker::debug::type_name<ticker::pool::conditional_wait_for_int>().data());
#endif

    auto fn = ticker::debug::type_name<std::string>();
    std::string str{fn};
    printf(">>> '%s'\n", str.c_str());

    std::cout << ticker::debug::type_name<std::string>() << '\n';
    std::cout << std::string(ticker::debug::type_name<std::string>()) << '\n';
    printf(">>> %s\n", std::string(ticker::debug::type_name<std::string>()).c_str());
}

void foo() {
    std::cout << x_global_var.c_str() << '\n';
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void test_thread_basics() {
    printf("> %s\n", __FUNCTION_NAME__);
    {
        std::thread t;
        std::cout << "- before starting, joinable: " << std::boolalpha << t.joinable()
                  << '\n';

        t = std::thread(foo);
        std::cout << "- after starting, joinable: " << t.joinable()
                  << '\n';

        t.join();
        std::cout << "- after joining, joinable: " << t.joinable()
                  << '\n';
    }
    {
        unsigned int n = std::thread::hardware_concurrency();
        std::cout << "- " << n << " concurrent _threads are supported.\n";
    }
}

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
                ticker::pool::cw_setter cws(count2);
                printf("  - interval [%02d]: %s\n", count2.val(), ticker::chrono::format_time_point().c_str());
            })
            .build();

    count2.wait();
    printf("end of %s\n", __FUNCTION_NAME__);
}

void test_alarmer() {
    using namespace std::literals::chrono_literals;
    ticker::debug::X x_local_var;
    ticker::pool::conditional_wait_for_int count2{4};
    ticker::alarmer<>::super::get()
            ->every_month(3)
            .on([&count2] {
                auto now = ticker::chrono::now();
                ticker::pool::cw_setter cws(count2);
                printf("  - alarmer [%02d]: %s\n", count2.val(), ticker::chrono::format_time_point(now).c_str());
            })
            .build();
    printf("end of %s\n", __FUNCTION_NAME__);
}

int main() {
    // test_thread();

    TICKER_TEST_FOR(test_periodical_job);

#if 1
    TICKER_TEST_FOR(test_type_name);
    TICKER_TEST_FOR(test_thread_basics);

    TICKER_TEST_FOR(test_timer);

    TICKER_TEST_FOR(test_ticker);
    TICKER_TEST_FOR(test_ticker_interval);

    TICKER_TEST_FOR(test_alarmer);

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
#endif
    return 0;
}