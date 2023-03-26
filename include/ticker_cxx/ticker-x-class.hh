//
// Created by Hedzr Yeh on 2021/7/16.
//

#ifndef TICKER_CXX_HZ_X_CLASS_HH
#define TICKER_CXX_HZ_X_CLASS_HH

#pragma once

#include <stdlib.h>
#include <string>

namespace ticker::debug {

  class X {
    std::string _str;

    void _ct(const char *leading) {
      printf("  - %s: X[ptr=%p].str: %p, '%s'\n", leading, (void *) this, (void *) _str.c_str(), _str.c_str());
    }

  public:
    X() {
      _ct("ctor()");
    }
    ~X() {
      _ct("dtor");
    }
    X(std::string &&s)
        : _str(std::move(s)) {
      _ct("ctor(s)");
    }
    X(std::string const &s)
        : _str(s) {
      _ct("ctor(s(const&))");
    }
    X &operator=(std::string &&s) {
      _str = std::move(s);
      _ct("operator=(&&s)");
      return (*this);
    }
    X &operator=(std::string const &s) {
      _str = s;
      _ct("operator=(const&s)");
      return (*this);
    }

    const char *c_str() const { return _str.c_str(); }
    operator const char *() const { return _str.c_str(); }
  };

} // namespace ticker::debug

#endif //TICKER_CXX_HZ_X_CLASS_HH
