// ticker_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/26.
//

#ifndef TICKER_CXX_TICKER_PERIODICAL_JOB_HH
#define TICKER_CXX_TICKER_PERIODICAL_JOB_HH

#include "ticker-anchors.hh"
#include "ticker-timer-job.hh"

namespace ticker::detail {

    template<typename Clock = std::chrono::system_clock, bool GMT = false>
    class periodical_job : public timer_job {
    public:
        explicit periodical_job(anchors anchor_, int ordinal_, int offset_, int times_, std::function<void()> &&f, bool interval = false)
            : timer_job(std::move(f), true, interval)
            , last_pt(Clock::now())
            , anchor(anchor_)
            , ordinal(ordinal_)
            , offset(offset_)
            , times(times_) {}
        virtual ~periodical_job() {}

        typename Clock::time_point next_time_point(typename Clock::time_point const now) const override {
            if (now < last_pt)
                return last_pt;

            typename Clock::time_point pt;
            auto time_now = Clock::to_time_t(now);
            std::tm tm = GMT ? *std::gmtime(&time_now) : *std::localtime(&time_now);

            switch (anchor) {
                case anchors::Nothing:
                    break;

                case anchors::Month:
                case anchors::TwoMonth:
                case anchors::Quarter:
                case anchors::FourMonth:
                case anchors::FiveMonth:
                case anchors::SixMonth:
                case anchors::SevenMonth:
                case anchors::EightMonth:
                case anchors::NineMonth:
                case anchors::TenMonth:
                case anchors::ElevenMonth:
                case anchors::Year: {
                    int delta = ordinal * (int) anchor;
                    if (offset > 0) {
                        if (tm.tm_mday >= offset) {
                            tm.tm_mon += delta;
                        }
                        tm.tm_mday = offset;
                        while (tm.tm_mon > 11) {
                            tm.tm_mon -= 12;
                            tm.tm_year++;
                        }
                    } else if (anchor < anchors::Year) {
                        int ofs = -offset;
                        tm = chrono::last_day_at_this_month<Clock, GMT>(tm, ofs, delta);
                    } else {
                        int ofs = -offset;
                        tm = chrono::last_day_at_this_year<Clock, GMT>(tm, ofs);
                    }
                    pt = chrono::tm_2_time_point<Clock>(&tm);
                    if (pt <= now) {
                        tm.tm_mon++;
                        pt = chrono::tm_2_time_point<Clock>(&tm);
                    }
                } break;

                case anchors::FirstThirdOfMonth: {
                    int delta = 1 * ordinal;
                    int day = offset > 0 ? offset : 11 + offset;
                    if (tm.tm_mday >= day) {
                        tm.tm_mon += delta;
                    }
                    tm.tm_mday = day;
                    pt = chrono::tm_2_time_point<Clock>(&tm);

                } break;
                case anchors::MiddleThirdOfMonth: {
                    int delta = 1 * ordinal;
                    int day = offset > 0 ? 10 + offset : 21 + offset;
                    if (tm.tm_mday >= day) {
                        tm.tm_mon += delta;
                    }
                    tm.tm_mday = day;
                    pt = chrono::tm_2_time_point<Clock>(&tm);
                } break;
                case anchors::LastThirdOfMonth: {
                    int delta = 1 * ordinal;
                    if (offset > 0) {
                        int day = 20 + offset;
                        if (tm.tm_mday >= day) {
                            tm.tm_mon += delta;
                        }
                        tm.tm_mday = day;
                    } else {
                        int ofs = -offset;
                        std::tm tmp = chrono::last_day_at_this_month<Clock, GMT>(tm, ofs, delta - 1);
                        if (tmp.tm_mday >= ofs) {
                            tm = chrono::last_day_at_this_month<Clock, GMT>(tm, ofs, delta);
                        } else {
                            tm = tmp;
                            tm.tm_mday -= (ofs);
                        }
                    }
                    pt = chrono::tm_2_time_point<Clock>(&tm);
                    if (pt < last_pt) {
                        tm.tm_mon++;
                        pt = chrono::tm_2_time_point<Clock>(&tm);
                    }
                } break;

                case anchors::DayInYear: { // 'offset': which day (0..365) in a year
                    typename Clock::time_point t;
                    int ofs;
                    if (offset > 0) {
                        ofs = offset;
                        t = chrono::tm_2_time_point<Clock>(&tm);
                    } else {
                        ofs = -offset;
                        std::tm tmp = chrono::last_day_at_this_year<Clock, GMT>(tm, ofs);
                        t = chrono::tm_2_time_point<Clock>(&tmp);
                        tm = tmp;
                    }

                    if (tm.tm_yday > ofs) {
                        int day_delta = ofs - tm.tm_wday;
                        t += std::chrono::hours(day_delta * 24);
                    } else {
                        int day_delta = ordinal + tm.tm_yday + 1 - tm.tm_wday;
                        t += std::chrono::hours(day_delta * 24);
                    }
                    tm = chrono::time_point_2_tm<Clock, GMT>(t);

                    pt = chrono::tm_2_time_point<Clock>(&tm);
                } break;

                case anchors::WeekInMonth: {
                    // 'offset': which week in a month
                    // 'ordinal': weekday in that week
                    if (offset > 0) {
                        int ofs = offset == 0 ? 1 : offset;
                        // std::tm tmp = chrono::last_day_at_this_month(tm, ofs, delta);
                        std::tm tmp = tm;
                        tmp.tm_mday = 1; // get 1st day in this month
                        auto t = chrono::tm_2_time_point<Clock>(&tmp);
                        tmp = chrono::time_point_2_tm<Clock, GMT>(t);

                        if (tmp.tm_wday < ordinal) {
                            int day_delta = ordinal - tmp.tm_wday;
                            t += std::chrono::hours(day_delta * 24);
                        } else {
                            int day_delta = ordinal + 7 - tmp.tm_wday;
                            t += std::chrono::hours(day_delta * 24);
                            ofs--;
                        }
                        ofs--;
                        if (ofs > 0) {
                            t += std::chrono::hours(ofs * 7 * 24);
                        }

                        tm = chrono::time_point_2_tm<Clock, GMT>(t);
                    } else {
                        int ofs = -offset;
                        std::tm tmp = chrono::last_day_at_this_month<Clock, GMT>(tm, ofs, 1);

                        typename Clock::time_point t;
                        if (tmp.tm_wday < ordinal) {
                            int day_delta = ordinal - tmp.tm_wday;
                            t -= std::chrono::hours(day_delta * 24);
                        } else {
                            int day_delta = ordinal + 7 - tmp.tm_wday;
                            t -= std::chrono::hours(day_delta * 24);
                            ofs--;
                        }
                        ofs--;
                        if (ofs > 0) {
                            t -= std::chrono::hours(ofs * 7 * 24);
                        }

                        tm = chrono::time_point_2_tm<Clock, GMT>(t);
                    }
                    pt = chrono::tm_2_time_point<Clock>(&tm);
                } break;

                case anchors::WeekInYear: {
                    // 'offset': which week in a year
                    // 'ordinal': weekday in that week
                    if (offset > 0) {
                        int ofs = offset == 0 ? 1 : offset;
                        std::tm tmp = tm;
                        tmp.tm_mday = 1; // get 1st day in this year
                        tmp.tm_mon = 0;
                        auto t = chrono::tm_2_time_point<Clock>(&tmp);
                        tmp = chrono::time_point_2_tm<Clock, GMT>(t);

                        if (tmp.tm_wday < ordinal) {
                            int day_delta = ordinal - tmp.tm_wday;
                            t += std::chrono::hours(day_delta * 24);
                        } else {
                            int day_delta = ordinal + 7 - tmp.tm_wday;
                            t += std::chrono::hours(day_delta * 24);
                            ofs--;
                        }
                        ofs--;
                        if (ofs > 0) {
                            t += std::chrono::hours(ofs * 7 * 24);
                        }

                        tm = chrono::time_point_2_tm<Clock, GMT>(t);
                    } else {
                        int ofs = -offset;
                        std::tm tmp = chrono::last_day_at_this_year<Clock, GMT>(tm, ofs);

                        typename Clock::time_point t;
                        if (tmp.tm_wday < ordinal) {
                            int day_delta = ordinal - tmp.tm_wday;
                            t -= std::chrono::hours(day_delta * 24);
                        } else {
                            int day_delta = ordinal + 7 - tmp.tm_wday;
                            t -= std::chrono::hours(day_delta * 24);
                            ofs--;
                        }
                        ofs--;
                        if (ofs > 0) {
                            t -= std::chrono::hours(ofs * 7 * 24);
                        }

                        tm = chrono::time_point_2_tm<Clock, GMT>(t);
                    }
                    pt = chrono::tm_2_time_point<Clock>(&tm);
                } break;

                case anchors::Week: {
                    int day_delta;
                    if (offset > 0) {
                        day_delta = tm.tm_wday > offset ? tm.tm_wday - offset : offset - tm.tm_wday + 7;
                    } else {
                        int ofs = 7 + offset;
                        day_delta = tm.tm_wday > ofs ? tm.tm_wday - ofs : ofs - tm.tm_wday + 7;
                    }
                    pt = now + std::chrono::hours(day_delta * 24);
                } break;

                case anchors::Dummy00000:
                    break;
                    // case anchors::Hour:
                    //     pt = chrono::tm_2_time_point<Clock ,GMT>(&tm);
                    //     break;
                    // case anchors::Minute:
                    //     pt = chrono::tm_2_time_point<Clock ,GMT>(&tm);
                    //     break;
                    // case anchors::Second:
                    //     pt = chrono::tm_2_time_point<Clock ,GMT>(&tm);
                    //     break;
                    // case anchors::Millisecond:
                    //     pt = chrono::tm_2_time_point<Clock ,GMT>(&tm);
                    //     break;
                    // case anchors::Microsecond:
                    //     pt = chrono::tm_2_time_point<Clock ,GMT>(&tm);
                    //     break;
                    // case anchors::Nanosecond:
                    //     pt = chrono::tm_2_time_point<Clock ,GMT>(&tm);
                    //     break;

                case anchors::Dummy00001:
                case anchors::Dummy00009:
                case anchors::__COUNT:
                    break;
            }

            (const_cast<periodical_job *>(this)->last_pt) = pt;
            // pool_debug("         %s -> %s", chrono::format_time_point(now).c_str(), chrono::format_time_point(nxt).c_str());
            return last_pt;
        };

        typename Clock::time_point last_pt;
        anchors anchor = anchors::Nothing; // 0: no anchor, 1: month, 2: quarter, 3: half a year, 4: year,
        int ordinal = 1;                   // ordinal in anchor
        int offset = 1;                    // >0: from start, <0: before end
        int times = -1;                    // repeat time. -1: no limit
    };

} // namespace ticker::detail

#endif //TICKER_CXX_TICKER_PERIODICAL_JOB_HH
