# ticker-cxx

![CMake Build Matrix](https://github.com/hedzr/ticker-cxx/workflows/CMake%20Build%20Matrix/badge.svg) <!-- 
![CMake Build Matrix](https://github.com/hedzr/ticker-cxx/workflows/CMake%20Build%20Matrix/badge.svg?event=release) 
--> [![GitHub tag (latest SemVer)](https://img.shields.io/github/tag/hedzr/ticker-cxx.svg?label=release)](https://github.com/hedzr/ticker-cxx/releases)

`ticker-cxx` is a C++17 library to provide a set of timing ticker functions, it schedules to trigger your customized tasks.

**WIP**

## Features

We provide these template classes for scheduling your works mainly:

- timer
- ticker
- alarm

The typical time and period representations, such as `1us`, from `std::literals::chrono_literals`, are valid for the timer setting.


## History

- v0.2.3: little improvements for building compatibilities
- v0.2.1: first public release
- ? : the plan started

## Guide

### Uses timer

the one-time timer after a duration (for instance `1us`):

```cpp
void test_timer() {
    using namespace std::literals::chrono_literals;

    ticker::pool::conditional_wait_for_int count{1};
    auto t = ticker::timer<>::get();
    t->after(1us)
            .on([&count] {
                auto now = ticker::chrono::now();
                ticker::pool::cw_setter cws(count);
                printf("  - after [%02d]: %s\n", count.val(), ticker::chrono::format_time_point(now).c_str());
            })
            .build();

    printf("count.wait()\n");
    count.wait_for(); // wait_for(2s) or wait()
    // t.clear();
    printf("end of %s\n", __FUNCTION_NAME__);
}
```

`at()`, `in()` are the synonyms of `after(...)`.


### Uses ticker

runs a ticker after 1us, and stop it once 16 times tick repeated:

```cpp
void test_timer() {
    using namespace std::literals::chrono_literals;

    ticker::pool::conditional_wait_for_int count{16};
    auto t = ticker::ticker<>::get();
    t->every(1us)
            .on([&count]() {
                // auto now = ticker::chrono::now();
                ticker::pool::cw_setter cws(count);
                printf("  - every [%02d]: %s\n", count.val(), ticker::chrono::format_time_point().c_str());
            })
            .build();

    count.wait(); // wait_for(2s) or wait()
}
```

`interval()` could be used instead of `every()`.

### alarm

`class ticker::alarm` provides the periodical job running mechanism. It could be used in a GTD app perfectly.

A simple demo is:

```cpp
void test_alarm() {
    using namespace std::literals::chrono_literals;
    
    ticker::pool::conditional_wait_for_int count2{4};
    ticker::alarm<>::super::get()
            ->every_month(3)
            .on([&count2] {
                auto now = ticker::chrono::now();
                ticker::pool::cw_setter cws(count2);
                printf("  - alarm [%02d]: %s\n", count2.val(), ticker::chrono::format_time_point(now).c_str());
            })
            .build();
    printf("end of %s\n", __FUNCTION_NAME__);
}
```

`ticker::alarm::loop_for(anchors, day_offset, how_many_times, repeat_count)` enables the full functions with Anchors enumerated variables:

```cpp
    enum class anchors {
        Nothing,

        Month,       // 'offset' (1..31, -1..-31) day every month; with 'ordinal' times
        TwoMonth,    // 'offset' day every two month
        Quarter,     // 'offset' quarter day every quarter
        FourMonth,   //
        FiveMonth,   //
        SixMonth,    // 'offset' day every half a year
        SevenMonth,  //
        EightMonth,  // eight
        NineMonth,   //
        TenMonth,    // ten
        ElevenMonth, //
        Year,        // 'offset' day every year, this month; with 'ordinal' times

        FirstThirdOfMonth,  // 'offset' (0..9, -1..-10) day 在每一个上旬; // with 'ordinal' times
        MiddleThirdOfMonth, // 'offset' (0..9, -1..-10) day 在每一个中旬; // with 'ordinal' times
        LastThirdOfMonth,   // 'offset' (0..9, -1..-10) day 在每一个下旬; // with 'ordinal' times

        DayInYear,   // 'offset' (0-365) day every one year
        WeekInMonth, // 'offset' (0..5)  week every one month
        WeekInYear,  // 'offset' (0..51) week every one year
        Week,        // 'offset' (0..6)  day every one week

        // ...
    };
```

## Build Options

### Build with CMake

> 1. gcc 10+: passed
> 2. clang 12+: passed
> 3. msvc build tool 16.7.2, 16.8.5 (VS2019 or Build Tool) passed

ninja is optional for faster building.

```bash
# configure
cmake -S . -B build/ -G Ninja
# build
cmake --build build/
# install
cmake --install build/
# Or:cmake --build build/ --target install
#
# Sometimes sudo it:
#   sudo cmake --build build/ --target install
# Or manually:
#   cmake --install build/ --prefix ./install --strip
#   sudo cp -R ./install/include/* /usr/local/include/
#   sudo cp -R ./install/lib/cmake/undo_cxx /usr/local/lib/cmake/
```


### Other CMake Options

1. `TICKER_CXX_BUILD_TESTS_EXAMPLES`=OFF
2. `TICKER_CXX_BUILD_DOCS`=OFF
3. ...
   

### CMake Options with C++ Macros

1. `TICKER_CXX_ENABLE_ASSERTIONS`
2. `TICKER_CXX_ENABLE_PRECONDITION_CHECKS`=ON
3. `TICKER_CXX_ENABLE_THREAD_POOL_READY_SIGNAL` (always be 1)
4. `TICKER_CXX_TEST_THREAD_POOL_DBGOUT`
5. `TICKER_CXX_UNIT_TEST`
6. `USE_DEBUG`, `USE_DEBUG_MALLOC`
7. ...

### Macros after include `ticker-def.hh`

1. `_DEBUG`, `DEBUG`
2. ...

## Thanks to JODL

Thanks to [JetBrains](https://www.jetbrains.com/?from=ticker-cxx) for donating product licenses to help develop **ticker-cxx** [![jetbrains](https://gist.githubusercontent.com/hedzr/447849cb44138885e75fe46f1e35b4a0/raw/bedfe6923510405ade4c034c5c5085487532dee4/jetbrains-variant-4.svg)](https://www.jetbrains.com/?from=hedzr/ticker-cxx)


## LICENSE

Apache 2.0

