/**
    \file Schedule.h
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

#ifndef INCLUDE_HUEPLUSPLUS_SCHEDULE_H
#define INCLUDE_HUEPLUSPLUS_SCHEDULE_H

#include "APICache.h"
#include "TimePattern.h"

namespace hueplusplus
{
class ScheduleCommand
{
public:
    explicit ScheduleCommand(const nlohmann::json& json);

    enum class Method
    {
        post, put, deleteMethod
    };

    std::string getAddress() const;
    Method getMethod() const;
    const nlohmann::json& getBody() const;

    const nlohmann::json& toJson() const;
private:
    static Method parseMethod(const std::string& s);
    static std::string methodToString(Method m);
private:
    nlohmann::json json;
};

class Schedule
{
public:
    enum class Status
    {
        disabled,
        enabled
    };

public:
    Schedule(int id, const HueCommandAPI& commands, std::chrono::steady_clock::duration refreshDuration);

    void refresh();

    int getId() const;

    std::string getName() const;
    std::string getDescription() const;
    ScheduleCommand getCommand() const;
    time::TimePattern getTime() const;
    Status getStatus() const;
    bool getAutodelete() const;
    time::AbsoluteTime getCreated() const;
    time::AbsoluteTime getStartTime() const;

    void setName(const std::string& name);
    void setDescription(const std::string& description);
    void setCommand(const ScheduleCommand& command);
    void setTime(const time::TimePattern& timePattern);
    void setStatus(Status status);
    void setAutodelete(bool autodelete);

private:
    void sendPutRequest(const nlohmann::json& request, FileInfo fileInfo);

private:
    int id;
    APICache state;
    HueCommandAPI commands;
};
} // namespace hueplusplus

#endif
