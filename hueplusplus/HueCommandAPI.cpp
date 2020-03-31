/**
    \file HueCommandAPI.h
    Copyright Notice\n
    Copyright (C) 2018  Jan Rogall		- developer\n
    Copyright (C) 2018  Moritz Wirger	- developer\n

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

#include "include/HueCommandAPI.h"

#include <thread>

constexpr std::chrono::steady_clock::duration HueCommandAPI::minDelay;

HueCommandAPI::HueCommandAPI(
    const std::string& ip, const int port, const std::string& username, std::shared_ptr<const IHttpHandler> httpHandler)
    : ip(ip),
      port(port),
      username(username),
      httpHandler(std::move(httpHandler)),
      timeout(new TimeoutData {std::chrono::steady_clock::now(), {}})
{}

nlohmann::json HueCommandAPI::PUTRequest(const std::string& path, const nlohmann::json& request) const
{
    auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(timeout->mutex);
    if (timeout->timeout > now)
    {
        std::this_thread::sleep_until(timeout->timeout);
    }
    // If path does not begin with '/', insert it unless path is empty
    const std::string combinedPath = "/api/" + username + (path.empty() || path.front() == '/' ? "" : "/") + path;
    try
    {
        nlohmann::json v = httpHandler->PUTJson(combinedPath, request, ip);
        timeout->timeout = now + minDelay;
        return v;
    }
    catch (const std::system_error& e)
    {
        if (e.code() == std::errc::connection_reset || e.code() == std::errc::timed_out)
        {
            // Happens when hue is too busy, wait and try again (once)
            std::this_thread::sleep_for(minDelay);
            nlohmann::json v = httpHandler->PUTJson(combinedPath, request, ip);
            timeout->timeout = std::chrono::steady_clock::now() + minDelay;
            return v;
        }
        // Cannot recover from other types of errors
        throw;
    }
}

nlohmann::json HueCommandAPI::GETRequest(const std::string& path, const nlohmann::json& request) const
{
    auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(timeout->mutex);
    if (timeout->timeout > now)
    {
        std::this_thread::sleep_until(timeout->timeout);
    }
    // If path does not begin with '/', insert it unless path is empty
    const std::string combinedPath = "/api/" + username + (path.empty() || path.front() == '/' ? "" : "/") + path;
    try
    {
        nlohmann::json v = httpHandler->GETJson(combinedPath, request, ip);
        timeout->timeout = now + minDelay;
        return v;
    }
    catch (const std::system_error& e)
    {
        if (e.code() == std::errc::connection_reset || e.code() == std::errc::timed_out)
        {
            // Happens when hue is too busy, wait and try again (once)
            std::this_thread::sleep_for(minDelay);
            nlohmann::json v = httpHandler->GETJson(combinedPath, request, ip);
            timeout->timeout = std::chrono::steady_clock::now() + minDelay;
            return v;
        }
        // Cannot recover from other types of errors
        throw;
    }
}

nlohmann::json HueCommandAPI::DELETERequest(const std::string& path, const nlohmann::json& request) const
{
    auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(timeout->mutex);
    if (timeout->timeout > now)
    {
        std::this_thread::sleep_until(timeout->timeout);
    }
    // If path does not begin with '/', insert it unless path is empty
    const std::string combinedPath = "/api/" + username + (path.empty() || path.front() == '/' ? "" : "/") + path;
    try
    {
        nlohmann::json v = httpHandler->DELETEJson(combinedPath, request, ip);
        timeout->timeout = now + minDelay;
        return v;
    }
    catch (const std::system_error& e)
    {
        if (e.code() == std::errc::connection_reset || e.code() == std::errc::timed_out)
        {
            // Happens when hue is too busy, wait and try again (once)
            std::this_thread::sleep_for(minDelay);
            nlohmann::json v = httpHandler->DELETEJson(combinedPath, request, ip);
            timeout->timeout = std::chrono::steady_clock::now() + minDelay;
            return v;
        }
        // Cannot recover from other types of errors
        throw;
    }
}
