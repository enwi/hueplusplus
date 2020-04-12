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

#include "hueplusplus/ExtendedColorHueStrategy.h"
#include "hueplusplus/ExtendedColorTemperatureStrategy.h"
#include "hueplusplus/HueDeviceTypes.h"
#include "hueplusplus/HueExceptionMacro.h"
#include "hueplusplus/SimpleBrightnessStrategy.h"
#include "hueplusplus/SimpleColorHueStrategy.h"
#include "hueplusplus/SimpleColorTemperatureStrategy.h"
#include "hueplusplus/UPnP.h"
#include "hueplusplus/Utils.h"

namespace hueplusplus
{
HueFinder::HueFinder(std::shared_ptr<const IHttpHandler> handler) : http_handler(std::move(handler)) {}

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
      port(port),
      username(username),
      simpleBrightnessStrategy(std::make_shared<SimpleBrightnessStrategy>()),
      simpleColorHueStrategy(std::make_shared<SimpleColorHueStrategy>()),
      extendedColorHueStrategy(std::make_shared<ExtendedColorHueStrategy>()),
      simpleColorTemperatureStrategy(std::make_shared<SimpleColorTemperatureStrategy>()),
      extendedColorTemperatureStrategy(std::make_shared<ExtendedColorTemperatureStrategy>()),
      http_handler(std::move(handler)),
      commands(ip, port, username, http_handler),
      stateCache("", commands, refreshDuration)
{}

std::string Hue::getBridgeIP()
{
    return ip;
}

int Hue::getBridgePort()
{
    return port;
}

std::string Hue::requestUsername()
{
    std::cout << "Please press the link Button! You've got 35 secs!\n"; // when the link
                                                                        // button was
                                                                        // pressed we
                                                                        // got 30
                                                                        // seconds to
                                                                        // get our
                                                                        // username for
                                                                        // control

    nlohmann::json request;
    request["devicetype"] = "HuePlusPlus#User";

    nlohmann::json answer;
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point lastCheck;
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(35))
    {
        if (std::chrono::steady_clock::now() - lastCheck > std::chrono::seconds(1))
        {
            lastCheck = std::chrono::steady_clock::now();
            answer = http_handler->POSTJson("/api", request, ip, port);
            nlohmann::json jsonUser = utils::safeGetMember(answer, 0, "success", "username");
            if (jsonUser != nullptr)
            {
                // [{"success":{"username": "<username>"}}]
                username = jsonUser.get<std::string>();
                // Update commands with new username and ip
                commands = HueCommandAPI(ip, port, username, http_handler);
                stateCache = APICache("", commands, stateCache.GetRefreshDuration());
                std::cout << "Success! Link button was pressed!\n";
                std::cout << "Username is \"" << username << "\"\n";
                break;
            }
            else if (answer.size() > 0 && answer[0].count("error"))
            {
                // All errors except 101: Link button not pressed
                if (utils::safeGetMember(answer, 0, "error", "type") != 101)
                {
                    throw HueAPIResponseException::Create(CURRENT_FILE_INFO, answer[0]);
                }
            }

            std::this_thread::sleep_until(lastCheck + std::chrono::seconds(1));
        }
    }
    return username;
}

std::string Hue::getUsername()
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

HueLight& Hue::getLight(int id)
{
    auto pos = lights.find(id);
    if (pos != lights.end())
    {
        pos->second.state.Refresh();
        return pos->second;
    }
    const nlohmann::json& lightsCache = stateCache.GetValue()["lights"];
    if (!lightsCache.count(std::to_string(id)))
    {
        std::cerr << "Error in Hue getLight(): light with id " << id << " is not valid\n";
        throw HueException(CURRENT_FILE_INFO, "Light id is not valid");
    }
    std::string type = lightsCache[std::to_string(id)]["modelid"];
    auto light = MakeHueLight()(type, id, commands, simpleBrightnessStrategy, extendedColorTemperatureStrategy,
        simpleColorTemperatureStrategy, extendedColorHueStrategy, simpleColorHueStrategy);
    lights.emplace(id, light);
    return lights.find(id)->second;
}

bool Hue::removeLight(int id)
{
    nlohmann::json result
        = commands.DELETERequest("/lights/" + std::to_string(id), nlohmann::json::object(), CURRENT_FILE_INFO);
    bool success = utils::safeGetMember(result, 0, "success") == "/lights/" + std::to_string(id) + " deleted";
    if (success && lights.count(id) != 0)
    {
        lights.erase(id);
    }
    return success;
}

std::vector<std::reference_wrapper<HueLight>> Hue::getAllLights()
{
    // No reference because getLight may invalidate it
    nlohmann::json lightsState = stateCache.GetValue()["lights"];
    for (auto it = lightsState.begin(); it != lightsState.end(); ++it)
    {
        getLight(std::stoi(it.key()));
    }
    std::vector<std::reference_wrapper<HueLight>> result;
    for (auto& entry : lights)
    {
        result.emplace_back(entry.second);
    }
    return result;
}

bool Hue::lightExists(int id)
{
    auto pos = lights.find(id);
    if (pos != lights.end())
    {
        return true;
    }
    if (stateCache.GetValue()["lights"].count(std::to_string(id)))
    {
        return true;
    }
    return false;
}

bool Hue::lightExists(int id) const
{
    auto pos = lights.find(id);
    if (pos != lights.end())
    {
        return true;
    }
    if (stateCache.GetValue()["lights"].count(std::to_string(id)))
    {
        return true;
    }
    return false;
}

std::string Hue::getPictureOfLight(int id) const
{
    std::string ret = "";
    auto pos = lights.find(id);
    if (pos != lights.end())
    {
        ret = getPictureOfModel(pos->second.getModelId());
    }
    return ret;
}

std::string Hue::getPictureOfModel(const std::string& model_id) const
{
    std::string ret = "";
    if (model_id == "LCT001" || model_id == "LCT007" || model_id == "LCT010" || model_id == "LCT014"
        || model_id == "LTW010" || model_id == "LTW001" || model_id == "LTW004" || model_id == "LTW015"
        || model_id == "LWB004" || model_id == "LWB006")
    {
        ret.append("e27_waca");
    }
    else if (model_id == "LWB010" || model_id == "LWB014")
    {
        ret.append("e27_white");
    }
    else if (model_id == "LCT012" || model_id == "LTW012")
    {
        ret.append("e14");
    }
    else if (model_id == "LCT002")
    {
        ret.append("br30");
    }
    else if (model_id == "LCT011" || model_id == "LTW011")
    {
        ret.append("br30_slim");
    }
    else if (model_id == "LCT003")
    {
        ret.append("gu10");
    }
    else if (model_id == "LTW013")
    {
        ret.append("gu10_perfectfit");
    }
    else if (model_id == "LST001" || model_id == "LST002")
    {
        ret.append("lightstrip");
    }
    else if (model_id == "LLC006 " || model_id == "LLC010")
    {
        ret.append("iris");
    }
    else if (model_id == "LLC005" || model_id == "LLC011" || model_id == "LLC012" || model_id == "LLC007")
    {
        ret.append("bloom");
    }
    else if (model_id == "LLC014")
    {
        ret.append("aura");
    }
    else if (model_id == "LLC013")
    {
        ret.append("storylight");
    }
    else if (model_id == "LLC020")
    {
        ret.append("go");
    }
    else if (model_id == "HBL001" || model_id == "HBL002" || model_id == "HBL003")
    {
        ret.append("beyond_ceiling_pendant_table");
    }
    else if (model_id == "HIL001 " || model_id == "HIL002")
    {
        ret.append("impulse");
    }
    else if (model_id == "HEL001 " || model_id == "HEL002")
    {
        ret.append("entity");
    }
    else if (model_id == "HML001" || model_id == "HML002" || model_id == "HML003" || model_id == "HML004"
        || model_id == "HML005")
    {
        ret.append("phoenix_ceiling_pendant_table_wall");
    }
    else if (model_id == "HML006")
    {
        ret.append("phoenix_down");
    }
    else if (model_id == "LTP001" || model_id == "LTP002" || model_id == "LTP003" || model_id == "LTP004"
        || model_id == "LTP005" || model_id == "LTD003")
    {
        ret.append("pendant");
    }
    else if (model_id == "LDF002" || model_id == "LTF001" || model_id == "LTF002" || model_id == "LTC001"
        || model_id == "LTC002" || model_id == "LTC003" || model_id == "LTC004" || model_id == "LTD001"
        || model_id == "LTD002" || model_id == "LDF001")
    {
        ret.append("ceiling");
    }
    else if (model_id == "LDD002 " || model_id == "LFF001")
    {
        ret.append("floor");
    }
    else if (model_id == "LDD001 " || model_id == "LTT001")
    {
        ret.append("table");
    }
    else if (model_id == "LDT001 " || model_id == "MWM001")
    {
        ret.append("recessed");
    }
    else if (model_id == "BSB001")
    {
        ret.append("bridge_v1");
    }
    else if (model_id == "BSB002")
    {
        ret.append("bridge_v2");
    }
    else if (model_id == "SWT001")
    {
        ret.append("tap");
    }
    else if (model_id == "RWL021")
    {
        ret.append("hds");
    }
    else if (model_id == "SML001")
    {
        ret.append("motion_sensor");
    }
    return ret;
}
} // namespace hueplusplus
