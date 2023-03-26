// ticker-cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/24.
//

#ifndef TICKER_CXX_TICKER_CORE_HH
#define TICKER_CXX_TICKER_CORE_HH

#include "ticker-def.hh"

#include "ticker-common.hh"
#include "ticker-dbg.hh"
#include "ticker-log.hh"
#include "ticker-pool.hh"

#include "ticker-chrono.hh"

#include "ticker-anchors.hh"
#include "ticker-jobs.hh"

#include <chrono>
#include <ctime>

#include <iomanip>
#include <iostream>
#include <map>
#include <queue>         // for std::priority_queue
#include <string>        // for std::string
#include <tuple>         // for std::tuple
#include <unordered_map> // for std::unordered_map
#include <vector>

#include <algorithm>
#include <functional>

namespace ticker {

  template<typename Derived>
  class base {
  public:
    base(base const &o) {
      __copy(o);
    }
    base(base &&o) {
      __copy(o);
    }
    virtual ~base() {}

    using __D = Derived;
    using _This = base<__D>;
    struct __W : public __D { // timer<Derived, Clock, GMT, ConcreteJob> {
      __W() = default;
      ~__W() {}
    };
    /**
         * @brief get a unique pointer to the instance of __D
         * @param fn_after_constructed a lambda with prototype `[]{ ... }`
         * @return std::unique_ptr&lt;__D>
         */
    static std::unique_ptr<__D> get(std::function<void()> &&fn_after_constructed = nullptr) {
      util::defer<bool> defer_(fn_after_constructed);
      // __W w;
      // return std::move(w);
      return std::make_unique<__W>();
    }
    // static std::shared_ptr<base> create(std::function<void()> &&fn_after_constructed = nullptr) {
    //     util::defer<bool> defer_(fn_after_constructed);
    //     return std::make_shared<__W>();
    // }
    // static timer get(std::function<void()> &&fn_after_constructed = nullptr) {
    //     util::defer<bool> defer_(fn_after_constructed);
    //     return timer{};
    // }

  protected:
    base() {}
    // CLAZZ_NON_MOVEABLE(base);
    void __copy(base const &) {
      // __COPY(_f);
    }
  }; // class base

  /**
     * @brief timer provides the standard Timer interface.
     * @tparam Clock 
     * @details We assume a standard Timer interface will be represented as:
     * 
     * ### A
     *     - in/afetr
     *     - at
     * 
     * 
     */
  template<typename DerivedT = std::nullopt_t,
           typename Clock = Clock,
           bool GMT = false,
           typename ConcreteJob = detail::in_job<Clock, GMT>>
  class timer : public base<typename std::conditional<std::is_same_v<std::nullopt_t, DerivedT>, timer<DerivedT, Clock, GMT, ConcreteJob>, DerivedT>::type> {
    // public:
    //     class posix_ticker {
    //     public:
    //     }; // class posix_ticker

  public:
    using _This = timer<DerivedT, Clock, GMT, ConcreteJob>;
    using super = base<typename std::conditional<std::is_same_v<std::nullopt_t, DerivedT>, _This, DerivedT>::type>;
    using base_t = super;
    using Job = timer_job;
    using _J = std::shared_ptr<Job>;
    using _C = Clock;
    using TP = std::chrono::time_point<_C>;
    using Jobs = std::vector<_J>;
    using TimingWheel = std::map<TP, Jobs>;

  protected:
    timer()
        : base_t{}, _pool(-1) { start(); }
    // CLAZZ_NON_MOVEABLE(timer);
    void __copy(timer const &o) {
      super::__copy(o);
      __COPY(_tp);
      __COPY(_f);
      // __COPY(_tk);
      __COPY(_twl);
      __COPY(_pasts);
      // __COPY(_l_twl);
      // __COPY(_pool);
      // __COPY(_started);
      // __COPY(_ended);
      __COPY(_larger_gap);
      __COPY(_wastage);
    }

  public:
    timer(timer const &o)
        : base_t{}, _pool(-1) {
      __copy(o);
    }
    timer(timer &&o)
        : base_t{}, _pool(-1) {
      __copy(o);
    }
    virtual ~timer() {
      dbg_debug("[timer] dtor...");
      clear();
    }
    void clear() { stop(); }
    void join() { stop(); }

    /**
         * @brief run task in (one minute, five seconds, ...)
         * @tparam _Callable 
         * @tparam _Args 
         * @param time 
         * @param f 
         * @param args 
         * @return 
         */
    typename super::__D &in(const typename Clock::time_point time) {
      _tp = time;
      return static_cast<typename super::__D &>(*this);
    }
    typename super::__D &in(const typename Clock::duration time) {
      _tp = Clock::now() + time;
      return static_cast<typename super::__D &>(*this);
    }
    typename super::__D &after(const typename Clock::time_point time) { return in(time); }
    typename super::__D &after(const typename Clock::duration time) { return in(time); }
    typename super::__D &at(const typename Clock::time_point time) { return in(time); }
    typename super::__D &at(const std::string &time) {
      std::tm tm;
      // auto time_now = Clock::to_time_t(Clock::now());
      // std::tm tm = *std::localtime(&time_now);
      // our final time as a time_point
      typename Clock::time_point tp;
      if (ticker::chrono::try_parse_by(tm, time, "%H:%M:%S", "%Y-%m-%d %H:%M:%S", "%Y/%m/%d %H:%M:%S")) {
        tp = ticker::chrono::tm_2_time_point(&tm);
        // if we've already passed this time, the user will mean next day, so add a day.
        if (Clock::now() >= tp)
          tp += std::chrono::hours(24);
      } else {
        // could not parse time
        throw std::runtime_error("Cannot parse time string: " + time);
      }

      return in(tp);
    }

    template<typename _Callable, typename... _Args>
    typename super::__D &on(_Callable &&f, _Args &&...args) {
      _f = std::bind(std::forward<_Callable>(f), std::forward<_Args>(args)...);
      return static_cast<typename super::__D &>(*this);
    }

    // template<typename = std::enable_if_t<std::is_same<typename super::__D, _This>::value,int> =0>
    void build() {
      std::shared_ptr<Job> t = std::make_shared<ConcreteJob>(std::move(_f));
      // auto next_time = t->next_time_point();
      // dbg_debug("next_time: %s", format_time_point(next_time).c_str());
      add_task(_tp, std::move(t));
    }

  private:
    void stop() {
      dbg_debug("[runner] stopping...");
      _t.detach();
      _tk.kill();
      // if (_t.joinable()) _t.join();
      _ended.wait();
      dbg_debug("[runner] stopped.");
    }
    void start() {
      {
        std::unique_lock<std::mutex> l(_l_twl);
        dbg_trace("[runner] starting...");
      }
      _t = std::thread(runner, this);
      _started.wait();
      // t.detach();
      dbg_trace("[runner] started.");
    }

    static void runner(timer *_this) { _this->runner_loop(); }
    void runner_loop() {
      bool ret;
      using namespace std::literals::chrono_literals;
      const auto starting_gap = 10ns;
      std::chrono::nanoseconds d = starting_gap;
#if defined(_DEBUG) || TICKER_CXX_TEST_THREAD_POOL_DBGOUT
      std::size_t hit{0}, loop{0};
#endif
      _started.set();
      dbg_trace("[runner] ready...");
      while ((ret = _tk.wait_for(d)) != _tk.ConditionMatched) {
        // std::this_thread::sleep_for(d);
        dbg_debug("[runner] waked up. (_tk.terminated() == %d, ret=%d)", _tk.terminated(), ret);
        d = _larger_gap;

        TP picked;
        Jobs jobs, recurred_jobs;
        {
          auto [itp, itn, found] = find_next();
          if (!found) {
            dbg_debug("[runner] find_next() returned not found");
            continue;
          }

          dbg_debug("[runner] found a time-point");
          // got a picked point
          std::unique_lock<std::mutex> l(_l_twl);
          (*itp).second.swap(jobs);
          picked = (*itp).first;

          // erase all expired jobs
          _twl.erase(_twl.begin(), itp);

          if (itn != _twl.end()) {
            auto next_tp = (*itn).first;
            d = (next_tp - picked);
            if (d > _wastage)
              d -= _wastage;
#if defined(_DEBUG) || TICKER_CXX_TEST_THREAD_POOL_DBGOUT
            if ((hit % 10) == 0)
              pool_debug("[runner] [size: %u, hit: %u, loop: %u] picked = %s, next_tp = %s, duration = %s",
                         _twl.size(), hit, loop,
                         chrono::format_time_point(picked).c_str(),
                         chrono::format_time_point(next_tp).c_str(),
                         chrono::format_duration(d).c_str());
            hit++;
#endif
          }
        }

        // launch the jobs
        for (auto it = jobs.begin(); it != jobs.end(); ++it) {
          std::shared_ptr<Job> &j = (*it);
          if (j->_interval) {
            // pool_debug("[runner] job starting, _interval");
            j->launch_to(_pool, [&](timer_job *tj) {
              add_task(tj->next_time_point(), std::move(j));
            });
          } else if (j->_recur) {
            // pool_debug("[runner] job starting, _recur");
            j->launch_to(_pool);
            recurred_jobs.emplace_back(std::move(j));
          } else {
            // pool_debug("[runner] job starting");
            j->launch_to(_pool);
          }
        }

        // hold all past jobs to avoid heap-use-after-free sanitization
        _pasts.emplace(picked, std::move(jobs));

        for (auto &j : recurred_jobs) {
          auto tp = j->next_time_point();
#if defined(_DEBUG) || TICKER_CXX_TEST_THREAD_POOL_DBGOUT
          auto dur = ((detail::every_job<Clock, GMT> *) j.get())->dur;
          auto size = add_task(tp, std::move(j));
          if ((loop % 10) == 0)
            pool_debug("[runner] [size: %u, hit: %u, loop: %u] _recur job/%d added: %s, dur = %s, d = %s",
                       size, hit, loop, recurred_jobs.size(),
                       chrono::format_time_point(tp).c_str(),
                       chrono::format_duration(dur).c_str(),
                       chrono::format_duration(d).c_str());
          UNUSED(size, dur);
          loop++;
#else
          add_task(tp, std::move(j));
#endif
          d = tp - Clock::now();
          if (d > _wastage)
            d -= _wastage;
        }
      }
      dbg_debug("[runner] timer::runner ended (_tk.terminated() == %d, ret = %d).", _tk.terminated(), ret);
      _ended.set();
    }
    std::tuple<typename TimingWheel::iterator, typename TimingWheel::iterator, bool>
    find_next() {
      auto time_now = Clock::to_time_t(Clock::now());
      bool found{};

      std::unique_lock<std::mutex> l(_l_twl);

      typename TimingWheel::iterator itn = _twl.end();
      typename TimingWheel::iterator itp = itn;

      for (auto it = _twl.begin(); it != itn; ++it) {
        auto &tp = (*it).first;
        auto tmp = Clock::to_time_t(tp);
        if (tmp > time_now) break;
        itp = it, found = true;
        continue;
      }

      if (found) {
        itn = itp;
        itn++;
      }
      return {itp, itn, found};
    }

  protected:
    std::size_t add_task(TP const &tp, std::shared_ptr<Job> &&task) {
      std::size_t size;
      {
        std::unique_lock<std::mutex> l(_l_twl);
        auto it = _twl.find(tp);
        if (it == _twl.end()) {
          Jobs coll;
          coll.emplace_back(std::move(task));
          _twl.emplace(tp, std::move(coll));
          pool_debug("add_task, tp not found. pool.size=%lu", _twl.size());
        } else {
          auto &coll = (*it).second;
          coll.emplace_back(std::move(task));
          pool_debug("add_task, tp found. pool.size=%lu", _twl.size());
        }
        size = _twl.size();
      }
      std::this_thread::yield();
      return size;
    }
    std::size_t remove_task(TP const &tp, std::shared_ptr<Job> const &task) {
      std::size_t size;
      {
        std::unique_lock<std::mutex> l(_l_twl);

        auto it = _twl.find(tp);
        if (it != _twl.end()) {
          auto &coll = (*it).second;
          coll.erase(std::remove(coll.begin(), coll.end(), task), coll.end());
        }
        size = _twl.size();
      }
      std::this_thread::yield();
      return size;
    }

  protected:
    typename Clock::time_point _tp{};
    std::function<void()> _f{nullptr};

  private:
    std::thread _t;
    pool::timer_killer _tk{}; // to shut down the sleep+loop in `runner` thread gracefully
    TimingWheel _twl{};
    TimingWheel _pasts{};
    std::mutex _l_twl{};
    pool::thread_pool _pool;
    pool::conditional_wait_for_bool _started{}, _ended{};                   // runner thread terminated.
    std::chrono::nanoseconds _larger_gap = std::chrono::milliseconds(3000); // = 3s
    std::chrono::nanoseconds _wastage = std::chrono::milliseconds(0);
  }; // class timer

  /**
     * @brief a posix timer wrapper class
     * 
     * @par inspired by [https://github.com/Bosma/Scheduler](https://github.com/Bosma/Scheduler) and <https://github.com/jmettraux/rufus-scheduler>.
     */
  template<typename DerivedT = std::nullopt_t,
           typename Clock = Clock,
           bool GMT = false,
           typename ConcreteJob = detail::every_job<Clock, GMT>>
  class ticker : public timer<typename std::conditional<std::is_same_v<std::nullopt_t, DerivedT>, ticker<DerivedT, Clock, GMT, ConcreteJob>, DerivedT>::type, Clock, GMT, ConcreteJob> {
  public:
    ticker(ticker const &o) {
      __copy(o);
    }
    ticker(ticker &&o) {
      __copy(o);
    }
    ~ticker() {}
    using _This = ticker<DerivedT, Clock, GMT, ConcreteJob>;
    using super = timer<typename std::conditional<std::is_same_v<std::nullopt_t, DerivedT>, _This, DerivedT>::type, Clock, GMT, ConcreteJob>;
    using base_t = typename super::base_t;
    // struct __W : public ticker<Clock, GMT, ConcreteJob> {
    //     __W() = default;
    // };
    // static std::shared_ptr<ticker> get(std::function<void()> &&f = nullptr) {
    //     util::defer<bool> defer_(f);
    //     return std::make_shared<__W>();
    // }
    // static ticker get(std::function<void()> &&f = nullptr) {
    //     util::defer<bool> defer_(f);
    //     return ticker{};
    // }

    typename base_t::__D &every(const typename Clock::duration time) {
      _dur = time, _interval = false;
      return static_cast<typename base_t::__D &>(*this);
    }
    typename base_t::__D &interval(const typename Clock::duration time) {
      _dur = time, _interval = true;
      return static_cast<typename base_t::__D &>(*this);
    }

    void build() {
      auto copy_fn = super::_f;
      std::shared_ptr<typename super::Job> t = std::make_shared<ConcreteJob>(_dur, std::move(copy_fn));
      auto next_time = t->next_time_point();
      dbg_debug("next_time: %s", chrono::format_time_point(next_time).c_str());
      if (_interval)
        super::add_task(Clock::now(), std::move(t));
      else
        super::add_task(next_time, std::move(t));
    }

    // template<typename _Callable, typename... _Args>
    // ticker &cron(const typename Clock::time_point time, _Callable &&f, _Args &&...args) {
    //     UNUSED(time, f);
    //     return (*this);
    // }

  protected:
    ticker() {}
    // CLAZZ_NON_MOVEABLE(ticker);
    void __copy(ticker const &o) {
      super::__copy(o);
      __COPY(_dur);
      __COPY(_interval);
    }

    typename Clock::duration _dur;
    bool _interval{false};
  }; // class ticker

  template<typename DerivedT = std::nullopt_t, typename Clock = Clock, bool GMT = false, typename ConcreteJob = detail::periodical_job<Clock, GMT>>
  class alarm : public ticker<typename std::conditional<std::is_same_v<std::nullopt_t, DerivedT>, alarm<DerivedT, Clock, GMT, ConcreteJob>, DerivedT>::type, Clock, GMT, ConcreteJob> {
  public:
    alarm(alarm const &o) {
      __copy(o);
    }
    alarm(alarm &&o) {
      __copy(o);
    }
    ~alarm() {}
    using _This = alarm<DerivedT, Clock, GMT, ConcreteJob>;
    using super = ticker<typename std::conditional<std::is_same_v<std::nullopt_t, DerivedT>, _This, DerivedT>::type, Clock, GMT, ConcreteJob>;
    using base_t = typename super::base_t;

    typename base_t::__D &every_month(int day_offset = 1, int how_many = 1, int repeat_times = 0) {
      return loop_for(anchors::Month, day_offset, how_many, repeat_times);
    }
    typename base_t::__D &every_year(int day_offset = 1, int how_many = 1, int repeat_times = 0) {
      return loop_for(anchors::Year, day_offset, how_many, repeat_times);
    }
    typename base_t::__D &loop_for(anchors anchor = anchors::Month, int day_offset = 1, int how_many = 1, int repeat_times = 0) {
      _anchor = anchor;
      _ordinal = how_many, _offset = day_offset, _times = repeat_times;
      return static_cast<typename base_t::__D &>(*this);
    }

    void build() {
      std::shared_ptr<typename super::Job> t = std::make_shared<ConcreteJob>(_anchor, _ordinal, _offset, _times, std::move(super::_f));
      auto next_time = t->next_time_point();
      dbg_debug("anchor: %d, count: %d, next_time: %s", _anchor, _ordinal, chrono::format_time_point(next_time).c_str());
      super::add_task(next_time, std::move(t));
    }

  protected:
    alarm() {}
    // CLAZZ_NON_MOVABLE(alarm);
    void __copy(alarm const &o) {
      super::__copy(o);
      __COPY(_anchor);
      __COPY(_ordinal);
      __COPY(_offset);
      __COPY(_times);
    }

    anchors _anchor = anchors::Nothing;
    int _ordinal{0};
    int _offset{0};
    int _times{0};
  }; // class alarm

} // namespace ticker

namespace ticker::test {

  inline std::ostream &build_time(std::ostream &os) {
    std::tm t{};
    std::istringstream tsi(__TIMESTAMP__);
    // tsi.imbue(std::locale("de_DE.utf-8"));
    tsi >> std::get_time(&t, "%a %b %d %H:%M:%S %Y");
    // std::get_time(&t, "%Y-%m-%dT%H:%M:%S");
    // std::ostringstream ts;
    // ts << std::put_time(&t, "%Y-%m-%dT%H:%M:%S");
    return os << std::put_time(&t, "%FT%T%z");
  }
  inline std::string build_time() {
    std::ostringstream ts;
    build_time(ts);
    return ts.str();
  }

  inline void test_for_macros() {
    std::cout << '\n';

    std::cout << "TICKER_CXX_ENABLE_ASSERTIONS               : " << TICKER_CXX_ENABLE_ASSERTIONS << '\n';
    std::cout << "TICKER_CXX_ENABLE_PRECONDITION_CHECKS      : " << TICKER_CXX_ENABLE_PRECONDITION_CHECKS << '\n';
    std::cout << "TICKER_CXX_ENABLE_THREAD_POOL_READY_SIGNAL : " << TICKER_CXX_ENABLE_THREAD_POOL_READY_SIGNAL << '\n';
    std::cout << "TICKER_CXX_ENABLE_VERBOSE_LOG              : " << TICKER_CXX_ENABLE_VERBOSE_LOG << '\n';
    std::cout << "TICKER_CXX_TEST_THREAD_POOL_DBGOUT         : " << TICKER_CXX_TEST_THREAD_POOL_DBGOUT << '\n';
    std::cout << "TICKER_CXX_UNIT_TEST                       : " << TICKER_CXX_UNIT_TEST << '\n';

    std::cout << '\n'
              << TICKER_CXX_PROJECT_NAME << " v" << TICKER_CXX_VERSION_STRING << '\n'
              << TICKER_CXX_ARCHIVE_NAME << ": " << TICKER_CXX_DESCRIPTION << '\n'
              << "         version: " << TICKER_CXX_VERSION_STR << '\n'
              << "          branch: " << TICKER_CXX_GIT_BRANCH << '\n'
              << "             tag: " << TICKER_CXX_GIT_TAG << " (" << TICKER_CXX_GIT_TAG_LONG << ")" << '\n'
              << "            hash: " << TICKER_CXX_GIT_REV << " (" << TICKER_CXX_GIT_COMMIT_HASH << ")" << '\n'
              << "             cpu: " << TICKER_CXX_CPU << '\n'
              << "            arch: " << TICKER_CXX_CPU_ARCH << '\n'
              << "       arch-name: " << TICKER_CXX_CPU_ARCH_NAME << '\n'
              << "      build-name: " << TICKER_CXX_BUILD_NAME << '\n'
              << "      build-time: " << build_time() << '\n'
              << "       timestamp: " << chrono::format_time_point() << '\n'
              << '\n';

    dbg_debug("debug mode log enabled.");
    dbg_verbose_debug("verbose log (trace mode) enabled.");
  }

} // namespace ticker::test

#endif //TICKER_CXX_TICKER_CORE_HH
