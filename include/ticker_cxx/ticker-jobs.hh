// ticker_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/26.
//

#ifndef TICKER_CXX_TICKER_JOBS_HH
#define TICKER_CXX_TICKER_JOBS_HH

#include "ticker-anchors.hh"
#include "ticker-timer-job.hh"
#include "ticker-periodical-job.hh"

namespace ticker::detail {
    
    template<typename Clock = Clock, bool GMT = false>
    class in_job : public timer_job {
    public:
        explicit in_job(std::function<void()> &&f)
            : timer_job(std::move(f)) {}
        virtual ~in_job() {}
        
        using time_point = typename Clock::time_point;
        // dummy time_point because it's not used
        typename Clock::time_point next_time_point(time_point const) const override { return time_point{typename Clock::duration(0)}; }
    };

    template<typename Clock = Clock, bool GMT = false>
    class every_job : public timer_job {
    public:
        explicit every_job(typename Clock::duration d, std::function<void()> &&f, bool interval = false)
            : timer_job(std::move(f), true, interval)
            , dur(d) {}
        virtual ~every_job() {}

        typename Clock::time_point next_time_point(typename Clock::time_point const now) const override {
#if defined(_DEBUG) || TICKER_CXX_TEST_THREAD_POOL_DBGOUT
            auto nxt = now + dur;
            pool_debug("         %s -> %s", chrono::format_time_point(now).c_str(), chrono::format_time_point(nxt).c_str());
            return nxt;
#else
            return now + dur;
#endif
        };

        typename Clock::duration dur;
    };

}

#endif //TICKER_CXX_TICKER_JOBS_HH
