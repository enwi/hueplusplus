/**
    \file Hue.cpp
    Copyright Notice\n
    Copyright (C) 2017  Jan Rogall		- developer\n
    Copyright (C) 2017  Moritz Wirger	- developer\n

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

#include "hueplusplus/Hue.h"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstring>
#include <iostream>
#include <locale>
#include <stdexcept>
#include <thread>

#include "hueplusplus/APIConfig.h"
#include "hueplusplus/HueExceptionMacro.h"
#include "hueplusplus/UPnP.h"
#include "hueplusplus/Utils.h"

namespace hueplusplus
{
HueFinder::HueFinder(std::shared_ptr<const IHttpHandler> handler) : http_handler(std::move(handler)) { }

std::vector<HueFinder::HueIdentification> HueFinder::FindBridges() const
{
    UPnP uplug;
    std::vector<std::pair<std::string, std::string>> foundDevices = uplug.getDevices(http_handler);

    std::vector<HueIdentification> foundBridges;
    for (const std::pair<std::string, std::string>& p : foundDevices)
    {
        size_t found = p.second.find("IpBridge");
        if (found != std::string::npos)
        {
            HueIdentification bridge;
            size_t start = p.first.find("//") + 2;
            size_t length = p.first.find(":", start) - start;
            bridge.ip = p.first.substr(start, length);
            std::string desc
                = http_handler->GETString("/description.xml", "application/xml", "", bridge.ip, bridge.port);
            std::string mac = ParseDescription(desc);
            if (!mac.empty())
            {
                bridge.mac = NormalizeMac(mac);
                foundBridges.push_back(std::move(bridge));
            }
        }
    }
    return foundBridges;
}

Hue HueFinder::GetBridge(const HueIdentification& identification)
{
    std::string normalizedMac = NormalizeMac(identification.mac);
    auto pos = usernames.find(normalizedMac);
    if (pos != usernames.end())
    {
        return Hue(identification.ip, identification.port, pos->second, http_handler);
    }
    Hue bridge(identification.ip, identification.port, "", http_handler);
    bridge.requestUsername();
    if (bridge.getUsername().empty())
    {
        std::cerr << "Failed to request username for ip " << identification.ip << std::endl;
        throw HueException(CURRENT_FILE_INFO, "Failed to request username!");
    }
    AddUsername(normalizedMac, bridge.getUsername());

    return bridge;
}

void HueFinder::AddUsername(const std::string& mac, const std::string& username)
{
    usernames[NormalizeMac(mac)] = username;
}

const std::map<std::string, std::string>& HueFinder::GetAllUsernames() const
{
    return usernames;
}

std::string HueFinder::NormalizeMac(std::string input)
{
    // Remove any non alphanumeric characters (e.g. ':' and whitespace)
    input.erase(std::remove_if(input.begin(), input.end(), [](char c) { return !std::isalnum(c, std::locale()); }),
        input.end());
    // Convert to lower case
    std::transform(input.begin(), input.end(), input.begin(), [](char c) { return std::tolower(c, std::locale()); });
    return input;
}

std::string HueFinder::ParseDescription(const std::string& description)
{
    const char* model = "<modelName>Philips hue bridge";
    const char* serialBegin = "<serialNumber>";
    const char* serialEnd = "</serialNumber>";
    if (description.find(model) != std::string::npos)
    {
        std::size_t begin = description.find(serialBegin);
        std::size_t end = description.find(serialEnd, begin);
        if (begin != std::string::npos && end != std::string::npos)
        {
            begin += std::strlen(serialBegin);
            if (begin < description.size())
            {
                std::string result = description.substr(begin, end - begin);
                return result;
            }
        }
    }
    return std::string();
}

Hue::Hue(const std::string& ip, const int port, const std::string& username,
    std::shared_ptr<const IHttpHandler> handler, std::chrono::steady_clock::duration refreshDuration)
    : ip(ip),
      username(username),
      port(port),
      http_handler(std::move(handler)),
      refreshDuration(refreshDuration),
      stateCache(std::make_shared<APICache>(
          "", HueCommandAPI(ip, port, username, http_handler), std::chrono::steady_clock::duration::max())),
      lightList(stateCache, "lights", refreshDuration,
          [factory = HueLightFactory(stateCache->getCommandAPI(), refreshDuration)](
              int id, const nlohmann::json& state) mutable { return factory.createLight(state, id); }),
      groupList(stateCache, "groups", refreshDuration),
      scheduleList(stateCache, "schedules", refreshDuration),
      sceneList(stateCache, "scenes", refreshDuration),
      sensorList(stateCache, "sensors", refreshDuration),
      bridgeConfig(stateCache, refreshDuration)
{ }

void Hue::refresh()
{
    stateCache->refresh();
}

std::string Hue::getBridgeIP() const
{
    return ip;
}

int Hue::getBridgePort() const
{
    return port;
}

std::string Hue::requestUsername()
{
    std::chrono::steady_clock::duration timeout = Config::instance().getRequestUsernameTimeout();
    std::chrono::steady_clock::duration checkInterval = Config::instance().getRequestUsernameAttemptInterval();
    std::cout << "Please press the link Button! You've got "
              << std::chrono::duration_cast<std::chrono::seconds>(timeout).count() << " secs!\n";

    // when the link button was pressed we got 30 seconds to get our username for control
    nlohmann::json request;
    request["devicetype"] = "HuePlusPlus#User";

    nlohmann::json answer;
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    // do-while loop to check at least once when timeout is 0
    do
    {
        std::this_thread::sleep_for(checkInterval);
        answer = http_handler->POSTJson("/api", request, ip, port);
        nlohmann::json jsonUser = utils::safeGetMember(answer, 0, "success", "username");
        if (jsonUser != nullptr)
        {
            // [{"success":{"username": "<username>"}}]
            username = jsonUser.get<std::string>();
            // Update commands with new username and ip
            setHttpHandler(http_handler);
            std::cout << "Success! Link button was pressed!\n";
            std::cout << "Username is \"" << username << "\"\n";
            break;
        }
        else if (answer.size() > 0 && answer[0].count("error"))
        {
            HueAPIResponseException exception = HueAPIResponseException::Create(CURRENT_FILE_INFO, answer[0]);
            // All errors except 101: Link button not pressed
            if (exception.GetErrorNumber() != 101)
            {
                throw exception;
            }
        }
    } while (std::chrono::steady_clock::now() - start < timeout);

    return username;
}

std::string Hue::getUsername() const
{
    return username;
}

void Hue::setIP(const std::string& ip)
{
    this->ip = ip;
}

void Hue::setPort(const int port)
{
    this->port = port;
}

BridgeConfig& Hue::config()
{
    return bridgeConfig;
}

const BridgeConfig& Hue::config() const
{
    return bridgeConfig;
}

Hue::LightList& Hue::lights()
{
    return lightList;
}

const Hue::LightList& Hue::lights() const
{
    return lightList;
}

Hue::GroupList& Hue::groups()
{
    return groupList;
}

const Hue::GroupList& Hue::groups() const
{
    return groupList;
}

Hue::ScheduleList& Hue::schedules()
{
    return scheduleList;
}

const Hue::ScheduleList& Hue::schedules() const
{
    return scheduleList;
}

Hue::SceneList& Hue::scenes()
{
    return sceneList;
}

const Hue::SceneList& Hue::scenes() const
{
    return sceneList;
}

Hue::SensorList& Hue::sensors()
{
    return sensorList;
}

const Hue::SensorList& Hue::sensors() const
{
    return sensorList;
}

void Hue::setHttpHandler(std::shared_ptr<const IHttpHandler> handler)
{
    http_handler = handler;
    stateCache = std::make_shared<APICache>("", HueCommandAPI(ip, port, username, handler), refreshDuration);
    lightList = ResourceList<HueLight, int>(stateCache, "lights", refreshDuration,
        [factory = HueLightFactory(stateCache->getCommandAPI(), refreshDuration)](
            int id, const nlohmann::json& state) mutable { return factory.createLight(state, id); });
    groupList = GroupList(stateCache, "groups", refreshDuration);
    scheduleList = ScheduleList(stateCache, "schedules", refreshDuration);
    sceneList = SceneList(stateCache, "scenes", refreshDuration);
    sensorList = SensorList(stateCache, "sensors", refreshDuration);
    bridgeConfig = BridgeConfig(stateCache, refreshDuration);
    stateCache->refresh();
}
} // namespace hueplusplus
