/**
    \file APICache.h
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

#ifndef INCLUDE_API_CACHE_H
#define INCLUDE_API_CACHE_H

#include <chrono>
#include <string>

#include "HueCommandAPI.h"

namespace hueplusplus
{
//! \brief Caches API GET requests and refreshes regularly.
class APICache
{
public:
    //! \brief Constructs APICache
    //! \param path URL appended after username, may be empty.
    //! \param commands HueCommandAPI for making API requests.
    //! \param refresh Interval between cache refreshing. May be 0 to always refresh.
    APICache(const std::string& path, const HueCommandAPI& commands, std::chrono::steady_clock::duration refresh);

    //! \brief Refresh cache now.
    //! \throws std::system_error when system or socket operations fail
    //! \throws HueException when response contained no body
    //! \throws HueAPIResponseException when response contains an error
    //! \throws nlohmann::json::parse_error when response could not be parsed
    void Refresh();

    //! \brief Get cached value, refresh if necessary.
    //! \throws std::system_error when system or socket operations fail
    //! \throws HueException when response contained no body
    //! \throws HueAPIResponseException when response contains an error
    //! \throws nlohmann::json::parse_error when response could not be parsed
    nlohmann::json& GetValue();
    //! \brief Get cached value, does not refresh.
    const nlohmann::json& GetValue() const;

    //! \brief Get duration between refreshes.
    std::chrono::steady_clock::duration GetRefreshDuration() const;

private:
    std::string path;
    HueCommandAPI commands;
    std::chrono::steady_clock::duration refreshDuration;
    std::chrono::steady_clock::time_point lastRefresh;
    nlohmann::json value;
};
} // namespace hueplusplus

#endif
