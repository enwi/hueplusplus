#include "hueplusplus/APICache.h"
/**
    \file BaseHttpHandler.cpp
    Copyright Notice\n
    Copyright (C) 2020  Jan Rogall		- developer\n
    Copyright (C) 2020  Moritz Wirger	- developer\n

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

#include "hueplusplus/HueExceptionMacro.h"

hueplusplus::APICache::APICache(
    const std::string& path, const HueCommandAPI& commands, std::chrono::steady_clock::duration refresh)
    : path(path), commands(commands), refreshDuration(refresh), lastRefresh(std::chrono::steady_clock::duration::zero())
{}

void hueplusplus::APICache::Refresh()
{
    value = commands.GETRequest(path, nlohmann::json::object(), CURRENT_FILE_INFO);
    lastRefresh = std::chrono::steady_clock::now();
}

nlohmann::json& hueplusplus::APICache::GetValue()
{
    using clock = std::chrono::steady_clock;
    // Explicitly check for zero in case refreshDuration is duration::max()
    // Negative duration causes overflow check to overflow itself
    if (lastRefresh.time_since_epoch().count() == 0 || refreshDuration.count() < 0)
    {
        // No value set yet
        Refresh();
    }
    // Check if nextRefresh would overflow (assumes lastRefresh is not negative, which it should not be).
    // If addition would overflow, do not refresh
    else if (clock::duration::max() - refreshDuration > lastRefresh.time_since_epoch())
    {
        clock::time_point nextRefresh = lastRefresh + refreshDuration;
        if (clock::now() >= nextRefresh)
        {
            Refresh();
        }
    }
    return value;
}

const nlohmann::json& hueplusplus::APICache::GetValue() const
{
    return value;
}

std::chrono::steady_clock::duration hueplusplus::APICache::GetRefreshDuration() const
{
    return refreshDuration;
}
