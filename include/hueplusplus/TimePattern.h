/**
    \file TimePattern.h
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

#ifndef INCLUDE_HUEPLUSPLUS_TIME_PATTERN
#define INCLUDE_HUEPLUSPLUS_TIME_PATTERN

#include <chrono>
#include <string>

namespace hueplusplus
{
namespace time
{
std::string timepointToTimestamp(std::chrono::system_clock::time_point time);
std::chrono::system_clock::time_point parseTimestamp(const std::string& timestamp);

std::string durationTo_hh_mm_ss(std::chrono::system_clock::duration duration);
std::chrono::system_clock::duration parseDuration(const std::string& hourMinSec);

class AbsoluteTime
{
    using clock = std::chrono::system_clock;

public:
    explicit AbsoluteTime(clock::time_point baseTime, clock::duration variation = std::chrono::seconds(0));

    clock::time_point getBaseTime() const;
    clock::duration getRandomVariation() const;

    std::string toString() const;

    static AbsoluteTime parse(const std::string& s);

private:
    clock::time_point base;
    clock::duration variation;
};

class Weekdays
{
public:
    Weekdays() : bitmask(0) {}
    explicit Weekdays(int num) : bitmask(1 << num) {}

    bool isNone() const;
    bool isAll() const;
    bool isMonday() const;
    bool isTuesday() const;
    bool isWednesday() const;
    bool isThursday() const;
    bool isFriday() const;
    bool isSaturday() const;
    bool isSunday() const;

    Weekdays unionWith(Weekdays other) const;
    Weekdays operator|(Weekdays other) const { return unionWith(other); }

    std::string toString() const;

    static Weekdays none();
    static Weekdays all();
    static Weekdays monday();
    static Weekdays tuesday();
    static Weekdays wednesday();
    static Weekdays thursday();
    static Weekdays friday();
    static Weekdays saturday();
    static Weekdays sunday();

    static Weekdays parse(const std::string& s);

    bool operator==(const Weekdays& other) const { return bitmask == other.bitmask; }
    bool operator!=(const Weekdays& other) const { return bitmask != other.bitmask; }

private:
    int bitmask;
};

class RecurringTime
{
    using clock = std::chrono::system_clock;

public:
    explicit RecurringTime(clock::duration daytime, Weekdays days, clock::duration variation = std::chrono::seconds(0));

    clock::duration getDaytime() const;
    clock::duration getRandomVariation() const;
    Weekdays getWeekdays() const;

    std::string toString() const;

private:
    clock::duration time;
    clock::duration variation;
    Weekdays days;
};

class TimeInterval
{
    using clock = std::chrono::system_clock;

public:
    TimeInterval(clock::duration start, clock::duration end, Weekdays days = Weekdays::all());

    clock::duration getStartTime() const;
    clock::duration getEndTime() const;
    Weekdays getWeekdays() const;

    std::string toString() const;

private:
    clock::duration start;
    clock::duration end;
    Weekdays days;
};

class Timer
{
    using clock = std::chrono::system_clock;

public:
    Timer(clock::duration duration, clock::duration variation = std::chrono::seconds(0));
    Timer(clock::duration duration, int numExecutions, clock::duration variation = std::chrono::seconds(0));

    bool isRecurring() const;
    int getNumberOfExecutions() const;
    clock::duration getExpiryTime() const;
    clock::duration getRandomVariation() const;

    std::string toString() const;

private:
    clock::duration expires;
    clock::duration variation;
    int numExecutions;
};

class TimePattern
{
public:
    enum class Type
    {
        undefined,
        absolute,
        recurring,
        interval,
        timer
    };

    TimePattern();
    ~TimePattern();
    explicit TimePattern(const AbsoluteTime& absolute);
    explicit TimePattern(const RecurringTime& recurring);
    explicit TimePattern(const TimeInterval& interval);
    explicit TimePattern(const Timer& timer);

    TimePattern(const TimePattern& other);

    TimePattern& operator=(const TimePattern& other);

    Type getType() const;

    AbsoluteTime asAbsolute() const;

    RecurringTime asRecurring() const;

    TimeInterval asInterval() const;

    Timer asTimer() const;

    std::string toString() const;

    static TimePattern parse(const std::string& s);

private:
    void destroy();

private:
    Type type;
    union
    {
        nullptr_t undefined;
        AbsoluteTime absolute;
        RecurringTime recurring;
        TimeInterval interval;
        Timer timer;
    };
};
} // namespace time
} // namespace hueplusplus

#endif