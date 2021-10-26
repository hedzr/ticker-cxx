// ticker_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/26.
//

#ifndef TICKER_CXX_TICKER_TIMER_JOB_HH
#define TICKER_CXX_TICKER_TIMER_JOB_HH

#include "ticker-chrono.hh"
#include "ticker-pool.hh"

#include <chrono>
#include <memory>

namespace ticker {

    using Clock = std::chrono::system_clock;

    class timer_job {
    public:
        explicit timer_job(std::function<void()> &&f, bool recur = false, bool interval = false)
            : _recur(recur)
            , _interval(interval)
            , _f(std::move(f))
            , _hit(0) {}
        virtual ~timer_job() {}
        Clock::time_point next_time_point() const { return next_time_point(Clock::now()); }
        virtual Clock::time_point next_time_point(Clock::time_point const now) const = 0;

        void launch_to(pool::thread_pool &p, std::function<void(timer_job *tj)> const &post_job = nullptr) {
            launch_fn_to_pool(_f, p, post_job);
        }

        std::size_t hits() const { return _hit; }
        void operator()() { _f(); }

    private:
        void launch_fn_to_pool(std::function<void()> const &fn, pool::thread_pool &pool, std::function<void(timer_job *tj)> const &post_job = nullptr) {
            pool.queue_task([=]() {
                fn();
                if (post_job)
                    post_job(this);
            });
#if defined(_DEBUG) || TICKER_CXX_TEST_THREAD_POOL_DBGOUT
            if ((_hit % 10) == 0)
                pool_debug("job launched, %p (_recur=%d, _interval=%d, hit=%u)", (void *) this, _recur, _interval, _hit);
#endif
            ++_hit;
            std::this_thread::yield();
        }

    public:
        bool _recur;
        bool _interval;

    protected:
        std::function<void()> _f;
        std::size_t _hit;
    };

} // namespace ticker

#endif //TICKER_CXX_TICKER_TIMER_JOB_HH
