// ticker_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/26.
//

#include "ticker_cxx/ticker-dbg.hh"
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

int main() {
  TICKER_TEST_FOR(test_type_name);
}