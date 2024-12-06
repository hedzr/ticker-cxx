// ticker_cxx -- C++17 Command Line Arguments Parser
//
// @file big-decimal.cc
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
// Created by Hedzr Yeh on 2023/8/30.
//

#include <cstddef>
#include <cstdint> // uint64_t
#include <cstring>
#include <vector>

// #include <decimal>

namespace math {

  namespace detail {
    template<int E, int M, int C = 1, typename NumT = uint64_t>
    class decimal_base_t {
    public:
      using num_t = NumT;
      struct s_parts {
        bool sign : 1;    // sign
        int exponent : E; // exponent
        int mantissa : M; // mantissa
      };

    public:
    protected:
    private:
      union {
        s_parts _p;
        num_t _v[C];
      };
    };
  } // namespace detail

  using decimal256_t = detail::decimal_base_t<19, 237, 4>;         // approximately 71 decimal digits
  using decimal128_t = detail::decimal_base_t<17, 110, 2>;         // 34 decimal digits of significand and an exponent range of −6143 to +6144
  using decimal64_t = detail::decimal_base_t<13, 50>;              // 16 decimal digits of significand and an exponent range of −383 to +384
  using decimal32_t = detail::decimal_base_t<11, 20, 1, uint32_t>; // 7 decimal digits of significand and an exponent range of −95 to +96

  using Octuple = decimal256_t;
  using Quadruple = decimal128_t;
  using Double = decimal64_t;
  using Single = decimal32_t;

  class decimal_t {
  public:
    using num_t = int8_t;

  public:
  protected:
    bool get_s() const { return _s; }

  private:
    bool _s{false};
    std::vector<num_t> _i{};
    std::vector<num_t> _f{};
  };

} // namespace math

int main() {
  //   {
  //     std::decimal::decimal32 dn(.3), dn2(.099), dn3(1000), dn4(201);
  //     dn -= dn2;
  //     dn *= dn3;
  //     cout << "decimal32 = " << (dn == dn4) << " : " << decimal32_to_double(dn) << endl;
  //   }

  //   {
  //     std::decimal::decimal64 dn(.3), dn2(.099), dn3(1000), dn4(201);
  //     dn -= dn2;
  //     dn *= dn3;
  //     cout << "decimal64 = " << (dn == dn4) << " : " << decimal64_to_double(dn) << endl;
  //   }

  //   {
  //     std::decimal::decimal128 dn(.3), dn2(.099), dn3(1000), dn4(201);
  //     dn -= dn2;
  //     dn *= dn3;
  //     cout << "decimal128 = " << (dn == dn4) << " : " << decimal128_to_double(dn) << endl;
  //   }

  return 0;
}