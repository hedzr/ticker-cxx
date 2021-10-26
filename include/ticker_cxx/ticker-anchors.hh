// ticker_cxx Library
// Copyright © 2021 Hedzr Yeh.
//
// This file is released under the terms of the MIT license.
// Read /LICENSE for more information.

//
// Created by Hedzr Yeh on 2021/10/26.
//

#ifndef TICKER_CXX_TICKER_ANCHORS_HH
#define TICKER_CXX_TICKER_ANCHORS_HH


#include "ticker-def.hh"

namespace ticker {

    AWESOME_MAKE_ENUM(anchors,
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

                      Dummy00000 = 1000,
                      // Hour,
                      // Minute,
                      // Second,
                      // Millisecond,
                      // Microsecond,
                      // Nanosecond,

                      Dummy00001 = 2000,
                      Dummy00009 = 9000)

}

#endif //TICKER_CXX_TICKER_ANCHORS_HH
