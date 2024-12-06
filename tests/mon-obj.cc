// ticker_cxx -- C++17 Command Line Arguments Parser
//
// @file mon-obj.cc
// @brief Utilities for string operations.
//
// @copy Copyright © 2016 - 2023 Hedzr Yeh.
//
// This file is part of ticker-cxx (ticker_cxx for C++17 version).
//
// ticker_cxx is free software: you can redistribute it and/or modify
// it under the terms of the MIT License.
//
// ticker_cxx is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the MIT License
// along with ticker_cxx.  If not, see <https://github.com/hedzr/cmdr-cxx/blob/master/LICENSE>.

//
// Created by Hedzr Yeh on 2023/7/30.
//

#include <iostream>

#include <algorithm>
#include <condition_variable>
#include <functional>
#include <iterator>
#include <mutex>
#include <queue>
#include <random>
#include <thread>
#include <vector>

#if 0
namespace RangeBasedLoop {
  void test() {
    for(auto& it: cont) {
      std::cout << (i++) << ": " << (*it) << '\n';
    }
  }
}
#endif

namespace HF {
  class double_t {
  public:
    // explicit double_t(double const& dbl) : _m(dbl) {}
    double_t(double dbl = 0.0) : _m(dbl) {}
    double_t(double_t const &o) : _m(o._m) {}
    explicit double_t(double_t &&o) : _m(o._m) {}

    friend double_t operator+(double_t const &lhs, double_t const &rhs) {
      double_t r(lhs);
      r._m += rhs._m;
      return r;
    }
    // friend double_t &operator+=(double_t &lhs, double_t const &rhs) {
    //   lhs._m += rhs._m;
    //   return lhs;
    // }
    friend double_t &operator+=(double_t &lhs, double_t const &rhs);

    double_t &operator-=(double_t const &rhs) {
      _m -= rhs._m;
      return (*this);
    }
    double_t operator-(double_t const &rhs) {
      double_t ret(*this);
      ret._m -= rhs._m;
      return ret;
    }

    friend std::ostream &operator<<(std::ostream &os, double_t const &o) { // (3)
      return os << o._m << " _m";
    }

    double_t &operator=(double_t const &o) {
      _m = o._m;
      return (*this);
    }

  private:
    double _m;
  };

  inline double_t &operator+=(double_t &lhs, double_t const &rhs) {
    lhs._m += rhs._m;
    return lhs;
  }

  template<class V>
  struct Cat {
    V value_;
  };

  template<class V>
  struct Dog {
    V value_;

    friend bool operator<(Dog const &a, Dog const &b) {
      return a.value_ < b.value_;
    }
  };

  namespace {

    template<class T>
    void sort_in_place(std::vector<T> const &vt) {
      std::vector<std::reference_wrapper<const T>> vr(vt.begin(), vt.end());
      std::sort(vr.begin(), vr.end());
      std::transform(vr.begin(), vr.end(),
                     std::ostream_iterator<int>(std::cout), std::mem_fn(&T::value_));
    }

    template<class V>
    inline bool operator<(Cat<V> const &a, Cat<V> const &b) {
      return a.value_ < b.value_;
    }

    // template<class V>
    // inline bool operator<(Cat<V> const &a, Cat<V> const &b) {
    //   return a.value_ < b.value_;
    // }

    // template<class V>
    // inline bool operator<(std::reference_wrapper<Cat<V>> const a, std::reference_wrapper<Cat<V>> const b) {
    //   return a.get().value_ < b.get().value_;
    // }

    void test1() {
      std::cout << '\n';

      double_t a(5.5);
      double_t const b = a;

      a += b;
      std::cout << "double_t(5.5) + double_t(5.5): " << a + b << '\n'; // (4)
      a -= b;
      std::cout << "double_t(5.5) - double_t(5.5): " << a - b << '\n'; // (5)
      std::cout << '\n';

      std::vector<Dog<int>> const vec{{7}, {9}, {5}, {1}, {3}};
      sort_in_place(vec);
    }

  } // namespace

} // namespace HF

namespace M {
  template<class T>
  class monitor_t {
  public:
    template<typename... Args>
    monitor_t(Args &&...args) : _obj(std::forward<Args>(args)...) {}

    struct monitor_helper {
      monitor_helper(monitor_t *mon) : _mon(mon), _ulh(mon->_lock) {}
      T *operator->() { return &_mon->_obj; }
      monitor_t *_mon;
      std::unique_lock<std::mutex> _ulh; // unique_lock helper
    };

    monitor_helper operator->() { return monitor_helper(this); }
    monitor_helper get_locker() { return monitor_helper(this); }
    T &unsafe_get() { return _obj; }
    T const &unsafe_get() const { return _obj; }

  private:
    T _obj;
    std::mutex _lock;
  };
} // namespace M

namespace A {
  class monitor_t {
  public:
    void lock() const { _monit_mutex.lock(); }
    void unlock() const { _monit_mutex.unlock(); }
    void notify_one() const noexcept { _monit_cond.notify_one(); }
    void notify_all() const noexcept { _monit_cond.notify_all(); }

#if (__cplusplus < 202002L) // c++20a, c++17 and lower

    template<typename Predicate>
    void wait(Predicate pred) const { // (10)
      std::unique_lock<std::mutex> monit_guard(_monit_mutex);
      _monit_cond.wait(monit_guard, pred);
    }

#else // c++20 or higher
    template<std::predicate Predicate>
    void wait(Predicate pred) const {
      std::unique_lock<std::mutex> monit_guard(_monit_mutex);
      _monit_cond.wait(monit_guard, pred);
    }
#endif
#if __cplusplus > 201703L
    // C++20a, and C++20 and higher
#endif

  private:
    mutable std::mutex _monit_mutex;
    mutable std::condition_variable _monit_cond;
  };

  template<typename T> // (1)
  class thread_safe_queue_t : public monitor_t {
  public:
    void add(T val) {
      lock();
      _my_queue.push(val); // (6)
      unlock();
      notify_one();
    }

    T get() {
      wait([this] { return !_my_queue.empty(); }); // (2)
      lock();
      auto val = _my_queue.front(); // (4)
      _my_queue.pop();              // (5)
      unlock();
      return val;
    }

  private:
    std::queue<T> _my_queue; // (3)
  };

  class dice_t {
  public:
    int operator()() { return _rand(); }

  private:
    std::function<int()> _rand = std::bind(std::uniform_int_distribution<>(1, 6),
                                           std::default_random_engine());
  };
} // namespace A

namespace {

  void test_namespace_a() {
    std::cout << '\n';

    constexpr auto NumberThreads = 100;

    A::thread_safe_queue_t<int> safe_queue; // (7)

    auto add_lambda = [&safe_queue](int val) {
      safe_queue.add(val); // (8)
      std::cout << val << " " << std::this_thread::get_id() << "; ";
    };
    auto get_lambda = [&safe_queue] { safe_queue.get(); }; // (9)

    std::vector<std::thread> add_threads(NumberThreads);
    A::dice_t dice;
    for (auto &thr : add_threads) thr = std::thread(add_lambda, dice());

    std::vector<std::thread> get_threads(NumberThreads);
    for (auto &thr : get_threads) thr = std::thread(get_lambda);

    for (auto &thr : add_threads) thr.join();
    for (auto &thr : get_threads) thr.join();

    std::cout << "\n\n";
  }

} // namespace

int main() {
  std::cout << "Hello, World!" << '\n';
  test_namespace_a();

  HF::test1();
}