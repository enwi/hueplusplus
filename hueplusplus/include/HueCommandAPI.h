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

#ifndef _HUECOMMANDAPI_H
#define _HUECOMMANDAPI_H

#include <atomic>
#include <chrono>
#include <mutex>

#include "IHttpHandler.h"


//! Handles communication to the bridge via IHttpHandler and enforces a timeout between each request
class HueCommandAPI
{
public:
    //! \brief Construct from ip, username and HttpHandler
    //!
    //! \param ip String that specifies the ip address of the Hue bridge in dotted decimal notation like "192.168.2.1"
    //! \param username String that specifies the username that is used to control the bridge
    //! \param handler HttpHandler of type \ref IHttpHandler for communication with the bridge
    HueCommandAPI(const std::string& ip, const std::string& username, std::shared_ptr<const IHttpHandler> httpHandler);

    //! \brief Copy construct from other HueCommandAPI
    //! \note All copies refer to the same timeout data, so even calls from different objects will be delayed
    HueCommandAPI(const HueCommandAPI&) = default;
    //! \brief Move construct from other HueCommandAPI
    //! \note All copies refer to the same timeout data, so even calls from different objects will be delayed
    HueCommandAPI(HueCommandAPI&&) = default;

    //! \brief Copy assign from other HueCommandAPI
    //! \note All copies refer to the same timeout data, so even calls from different objects will be delayed
    HueCommandAPI& operator=(const HueCommandAPI&) = default;
    //! \brief Move assign from other HueCommandAPI
    //! \note All copies refer to the same timeout data, so even calls from different objects will be delayed
    HueCommandAPI& operator=(HueCommandAPI&&) = default;

    //! \brief Sends a HTTP PUT request via the \ref httpHandler to the bridge and returns the response
    //!
    //! This function will block until at least \ref minDelay has passed to any previous request
    //! \param path String that contains the request path (appended after /api/<username>)
    //! \param request Json value containing the request. May be empty
    //! \returns The return value of the underlying \ref IHttpHandler::PUTJson call
    Json::Value PUTRequest(const std::string& path, const Json::Value& request) const;
    
    //! \brief Sends a HTTP GET request via the \ref httpHandler to the bridge and returns the response
    //!
    //! This function will block until at least \ref minDelay has passed to any previous request
    //! \param path String that contains the request path (appended after /api/<username>)
    //! \param request Json value containing the request. May be empty
    //! \returns The return value of the underlying \ref IHttpHandler::GETJson call
    Json::Value GETRequest(const std::string& path, const Json::Value& request) const;

    //! \brief Sends a HTTP DELETE request via the \ref httpHandler to the bridge and returns the response
    //!
    //! This function will block until at least \ref minDelay has passed to any previous request
    //! \param path String that contains the request path (appended after /api/<username>)
    //! \param request Json value containing the request. May be empty
    //! \returns The return value of the underlying \ref IHttpHandler::DELETEJson call
    Json::Value DELETERequest(const std::string& path, const Json::Value& request) const;
private:
    struct TimeoutData
    {
        std::chrono::steady_clock::time_point timeout;
        std::mutex mutex;
    };
private:
    static constexpr std::chrono::steady_clock::duration minDelay = std::chrono::milliseconds(100);
    std::string ip;
    std::string username;
    std::shared_ptr<const IHttpHandler> httpHandler;
    std::shared_ptr<TimeoutData> timeout;
};

#endif