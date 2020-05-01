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

namespace hueplusplus
{
APICache::APICache(std::shared_ptr<APICache> baseCache, const std::string& subEntry)
    : base(baseCache),
      path(subEntry),
      commands(baseCache->commands),
      refreshDuration(baseCache->refreshDuration),
      lastRefresh(baseCache->lastRefresh)
{}

APICache::APICache(const std::string& path, const HueCommandAPI& commands, std::chrono::steady_clock::duration refresh)
    : path(path), commands(commands), refreshDuration(refresh), lastRefresh(std::chrono::steady_clock::duration::zero())
{}

void APICache::refresh()
{
    if (base)
    {
        base->refresh();
    }
    else
    {
        value = commands.GETRequest(path, nlohmann::json::object(), CURRENT_FILE_INFO);
        lastRefresh = std::chrono::steady_clock::now();
    }
}

nlohmann::json& APICache::getValue()
{
    if (base)
    {
        nlohmann::json& baseState = base->getValue();
        auto pos = baseState.find(path);
        if (pos != baseState.end())
        {
            return *pos;
        }
        else
        {
            throw HueException(CURRENT_FILE_INFO, "Child path not present in base cache");
        }
    }
    else
    {
        using clock = std::chrono::steady_clock;
        // Explicitly check for zero in case refreshDuration is duration::max()
        // Negative duration causes overflow check to overflow itself
        if (lastRefresh.time_since_epoch().count() == 0 || refreshDuration.count() < 0)
        {
            // No value set yet
            refresh();
        }
        // Check if nextRefresh would overflow (assumes lastRefresh is not negative, which it should not be).
        // If addition would overflow, do not refresh
        else if (clock::duration::max() - refreshDuration > lastRefresh.time_since_epoch())
        {
            clock::time_point nextRefresh = lastRefresh + refreshDuration;
            if (clock::now() >= nextRefresh)
            {
                refresh();
            }
        }
        return value;
    }
}

const nlohmann::json& APICache::getValue() const
{
    if (base)
    {
        // Make const reference to not refresh
        const APICache& b = *base;
        return b.getValue().at(path);
    }
    else
    {
        if (lastRefresh.time_since_epoch().count() == 0)
        {
            // No value has been requested yet
            throw HueException(CURRENT_FILE_INFO,
                "Tried to call const getValue(), but no value was cached. "
                "Call refresh() or non-const getValue() first.");
        }
        return value;
    }
}

std::chrono::steady_clock::duration APICache::getRefreshDuration() const
{
    return refreshDuration;
}
} // namespace hueplusplus
