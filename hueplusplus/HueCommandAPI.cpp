/**
    \file HueCommandAPI.h
    Copyright Notice\n
    Copyright (C) 2018  Jan Rogall		- developer\n
    Copyright (C) 2018  Moritz Wirger	- developer\n

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**/

#include "include\HueCommandAPI.h"

#include <thread>

HueCommandAPI::HueCommandAPI(const std::string& ip, const std::string& username, std::shared_ptr<const IHttpHandler> httpHandler)
    : ip(ip),
    username(username),
    httpHandler(std::move(httpHandler)),
    timeout(new TimeoutData{std::chrono::steady_clock::now()})
{}

Json::Value HueCommandAPI::PUTRequest(const std::string& path, const Json::Value& request) const
{
    auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(timeout->mutex);
    if (timeout->timeout > now)
    {
        std::this_thread::sleep_until(timeout->timeout);
    }
    Json::Value v = httpHandler->PUTJson("/api/" + username + path, request, ip);
    timeout->timeout = now + minDelay;

    return v;
}

Json::Value HueCommandAPI::GETRequest(const std::string& path, const Json::Value& request) const
{
    auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(timeout->mutex);
    if (timeout->timeout > now)
    {
        std::this_thread::sleep_until(timeout->timeout);
    }
    Json::Value v = httpHandler->GETJson("/api/" + username + path, request, ip);
    timeout->timeout = now + minDelay;

    return v;
}

Json::Value HueCommandAPI::DELETERequest(const std::string& path, const Json::Value& request) const
{
    auto now = std::chrono::steady_clock::now();
    std::lock_guard<std::mutex> lock(timeout->mutex);
    if (timeout->timeout > now)
    {
        std::this_thread::sleep_until(timeout->timeout);
    }
    Json::Value v = httpHandler->DELETEJson("/api/" + username + path, request, ip);
    timeout->timeout = now + minDelay;

    return v;
}
