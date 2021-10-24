//
// Created by Hedzr Yeh on 2021/8/6.
//

#ifndef TICKER_CXX_HZ_X_TEST_HH
#define TICKER_CXX_HZ_X_TEST_HH

#if defined(TICKER_CXX_UNIT_TEST) && TICKER_CXX_UNIT_TEST == 1

#include <algorithm>
#include <ctime>
#include <functional>
#include <memory>

#include "ticker-chrono.hh"
#include "ticker-dbg.hh"


namespace ticker::test {

    /**
     * @brief wrapper will wrap a test function to add pre-/post-process on it.
     * @tparam _Callable 
     * @tparam _Args 
     * @see TICKER_TEST_FOR
     * @note To use wrapper class, see also TICKER_TEST_FOR(func) macro.
     * In your test app, it'll be quoted as:
     * @code{c++}
     * void test_func_1(){ ... }
     * int main(){
     *   TICKER_TEST_FOR(test_func_1);
     * }
     * @endcode
     */
    template<typename _Callable, typename... _Args>
    class wrapper {
    public:
        explicit wrapper(const char *_fname_, _Callable &&f, _Args &&...args) {
            // auto filename = debug::type_name<decltype(f)>();
            auto bound = std::bind(std::forward<_Callable>(f), std::forward<_Args>(args)...);
            chrono::high_res_duration hrd;
            before(_fname_);
            try {
                bound();
            } catch (...) {
                after(_fname_);
                throw;
            }
            after(_fname_);
        }
        ~wrapper() = default;

    private:
        void before(const char *fname) {
            printf("\n--- BEGIN OF %-40s ----------------------\n", fname);
        }
        void after(const char *fname) {
            printf("--- END OF %-42s ----------------------\n\n", fname);
        }
    }; // class wrapper

    template<typename _Callable, typename... _Args>
    inline auto bind(const char *funcname, _Callable &&f, _Args &&...args) {
        wrapper w{funcname, f, args...};
        return w;
    }

    /**
     * @brief TICKER_TEST_FOR will wrap a test function to add pre-/post-process on it.
     * @details In your test app, it'll be quoted as:
     * @code{c++}
     * void test_func_1(){ ... }
     * int main(){
     *   TICKER_TEST_FOR(test_func_1);
     * }
     * @endcode
     */
#define TICKER_TEST_FOR(f) ticker::test::bind(#f, f)

    namespace detail {
        inline void third_party(int n, std::function<void(int)> f) {
            f(n);
        }

        /**
     * @brief 
     * 
     * foo f;
     * f.invoke(1, 2);
     * 
     */
        struct foo {
            template<typename... Args>
            void invoke(int n, Args &&...args) {
                auto bound = std::bind(&foo::invoke_impl<Args &...>, this,
                                       std::placeholders::_1, std::forward<Args>(args)...);

                third_party(n, bound);
            }

            template<typename... Args>
            void invoke_impl(int, Args &&...) {
            }
        };
    } // namespace detail

} // namespace ticker::test
#endif

#endif //TICKER_CXX_HZ_X_TEST_HH
