/**
    \file test_TimePattern.cpp
    Copyright Notice\n
    Copyright (C) 2020  Jan Rogall		- developer\n

    This file is part of hueplusplus.

    hueplusplus is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    hueplusplus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with hueplusplus.  If not, see <http://www.gnu.org/licenses/>.
**/

#include <gtest/gtest.h>
#include <hueplusplus/TimePattern.h>

using namespace hueplusplus::time;
using std::chrono::system_clock;
using namespace std::chrono_literals;

TEST(AbsoluteTime, Constructor)
{
    system_clock::time_point now = system_clock::now();
    {
        AbsoluteTime time(now);
        EXPECT_EQ(now, time.getBaseTime());
        EXPECT_EQ(0s, time.getRandomVariation());
    }
    system_clock::duration variation = 4h + 2min;
    {
        AbsoluteTime time(now, variation);
        EXPECT_EQ(now, time.getBaseTime());
        EXPECT_EQ(variation, time.getRandomVariation());
    }
}

TEST(AbsoluteTime, toString)
{
    std::tm time {};
    time.tm_year = 2020 - 1900;
    time.tm_mon = 2;
    time.tm_mday = 3;
    time.tm_hour = 20;
    time.tm_min = 53;
    time.tm_sec = 3;
    std::time_t ctime = std::mktime(&time);
    const system_clock::time_point timePoint = system_clock::from_time_t(ctime);

    EXPECT_EQ("2020-03-03T20:53:03", AbsoluteTime(timePoint).toString());

    const system_clock::duration noVariation = 0s;
    EXPECT_EQ("2020-03-03T20:53:03", AbsoluteTime(timePoint, noVariation).toString());

    const system_clock::duration variation = 1h + 2min + 1s;
    EXPECT_EQ("2020-03-03T20:53:03A01:02:01", AbsoluteTime(timePoint, variation).toString());
}

TEST(Weekdays, Constructor)
{
    EXPECT_TRUE(Weekdays().isNone());
    EXPECT_TRUE(Weekdays(0).isMonday());
    EXPECT_TRUE(Weekdays(6).isSunday());
}

TEST(Weekdays, isXXX)
{
    Weekdays none = Weekdays::none();
    EXPECT_TRUE(none.isNone());
    EXPECT_FALSE(none.isAll());
    EXPECT_FALSE(none.isMonday());
    EXPECT_FALSE(none.isTuesday());
    EXPECT_FALSE(none.isWednesday());
    EXPECT_FALSE(none.isThursday());
    EXPECT_FALSE(none.isFriday());
    EXPECT_FALSE(none.isSaturday());
    EXPECT_FALSE(none.isSunday());

    Weekdays all = Weekdays::all();
    EXPECT_FALSE(all.isNone());
    EXPECT_TRUE(all.isAll());
    EXPECT_TRUE(all.isMonday());
    EXPECT_TRUE(all.isTuesday());
    EXPECT_TRUE(all.isWednesday());
    EXPECT_TRUE(all.isThursday());
    EXPECT_TRUE(all.isFriday());
    EXPECT_TRUE(all.isSaturday());
    EXPECT_TRUE(all.isSunday());

    // Test that for all days, only their own isXXX function is true
    std::vector<Weekdays> days {Weekdays::monday(), Weekdays::tuesday(), Weekdays::wednesday(), Weekdays::thursday(),
        Weekdays::friday(), Weekdays::saturday(), Weekdays::sunday()};
    using BoolGetter = bool (Weekdays::*)() const;
    std::vector<BoolGetter> getters {&Weekdays::isMonday, &Weekdays::isTuesday, &Weekdays::isWednesday,
        &Weekdays::isThursday, &Weekdays::isFriday, &Weekdays::isSaturday, &Weekdays::isSunday};
    for (int i = 0; i < days.size(); ++i)
    {
        Weekdays day = days[i];
        EXPECT_FALSE(day.isNone());
        EXPECT_FALSE(day.isAll());
        for (int j = 0; j < getters.size(); ++j)
        {
            EXPECT_EQ(j == i, (day.*getters[j])()) << "on Day " << i << ": getter for day " << j << " has wrong result";
        }
    }
}

TEST(Weekdays, unionWith)
{
    Weekdays day = Weekdays::monday().unionWith(Weekdays::saturday());
    EXPECT_TRUE(day.isMonday());
    EXPECT_TRUE(day.isSaturday());

    day = Weekdays::monday() | Weekdays::tuesday() | Weekdays::all();
    EXPECT_TRUE(day.isAll());
}

TEST(Weekdays, toString)
{
    EXPECT_EQ("001", Weekdays(0).toString());
    EXPECT_EQ("064", Weekdays(6).toString());
    EXPECT_EQ("112", (Weekdays(6) | Weekdays(5) | Weekdays(4)).toString());
}
