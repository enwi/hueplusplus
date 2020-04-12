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
    : path(path),
      commands(commands),
      refreshDuration(refresh),
      lastRefresh(std::chrono::steady_clock::duration::zero())
{}

void hueplusplus::APICache::Refresh() {
    value = commands.GETRequest(path, nlohmann::json::object(), CURRENT_FILE_INFO);
}

nlohmann::json& hueplusplus::APICache::GetValue()
{
    if (std::chrono::steady_clock::now() >= lastRefresh + refreshDuration)
    {
        Refresh();
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
