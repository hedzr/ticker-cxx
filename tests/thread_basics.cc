// ticker_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/26.
//

#include "ticker_cxx/ticker-pool.hh"
#include "ticker_cxx/ticker-x-class.hh"
#include "ticker_cxx/ticker-x-test.hh"

#include <chrono>
#include <iostream>
#include <thread>

ticker::debug::X x_global_var;

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

int main() {
    TICKER_TEST_FOR(test_thread_basics);
}