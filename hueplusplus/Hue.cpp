/**
    \file Hue.cpp
    Copyright Notice\n
    Copyright (C) 2017  Jan Rogall		- developer\n
    Copyright (C) 2017  Moritz Wirger	- developer\n

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

#include "include/Hue.h"

#include <chrono>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <thread>

#include "include/ExtendedColorHueStrategy.h"
#include "include/ExtendedColorTemperatureStrategy.h"
#include "include/SimpleBrightnessStrategy.h"
#include "include/SimpleColorHueStrategy.h"
#include "include/SimpleColorTemperatureStrategy.h"

#include "include/UPnP.h"

HueFinder::HueFinder(std::shared_ptr<const IHttpHandler> handler) : http_handler(std::move(handler))
{}

std::vector<HueFinder::HueIdentification> HueFinder::FindBridges() const
{
    UPnP uplug;
    std::vector<std::pair<std::string, std::string>> foundDevices = uplug.getDevices(http_handler);

    //Does not work
    std::regex manufRegex("<manufacturer>Royal Philips Electronics</manufacturer>");
    std::regex manURLRegex("<manufacturerURL>http://www\\.philips\\.com</manufacturerURL>");
    std::regex modelRegex("<modelName>Philips hue bridge[^<]*</modelName>");
    std::regex serialRegex("<serialNumber>(\\w+)</serialNumber>");
    std::vector<HueIdentification> foundBridges;
    for (const std::pair<std::string, std::string> &p : foundDevices)
    {
        size_t found = p.second.find("IpBridge");
        if (found != std::string::npos)
        {
            HueIdentification bridge;
            size_t start = p.first.find("//") + 2;
            size_t length = p.first.find(":", start) - start;
            bridge.ip = p.first.substr(start, length);
            std::string desc = http_handler->GETString("/description.xml", "application/xml", "", bridge.ip);
            std::smatch matchResult;
            if (std::regex_search(desc, manufRegex) && std::regex_search(desc, manURLRegex) && std::regex_search(desc, modelRegex) && std::regex_search(desc, matchResult, serialRegex))
            {
                //The string matches
                //Get 1st submatch (0 is whole match)
                bridge.mac = matchResult[1].str();
                foundBridges.push_back(std::move(bridge));
            }
            //break;
        }
    }
    return foundBridges;
}


Hue HueFinder::GetBridge(const HueIdentification& identification)
{
    auto pos = usernames.find(identification.mac);
    if (pos != usernames.end())
    {
        return Hue(identification.ip, pos->second, http_handler);
    }
    Hue bridge(identification.ip, "", http_handler);
    bridge.requestUsername(identification.ip);
    if (bridge.getUsername().empty())
    {
        std::cerr << "Failed to request username for ip " << identification.ip << std::endl;
        throw std::runtime_error("Failed to request username!");
    }
    AddUsername(identification.mac, bridge.getUsername());

    return bridge;
}

void HueFinder::AddUsername(const std::string& mac, const std::string& username)
{
    usernames[mac] = username;
}

const std::map<std::string, std::string>& HueFinder::GetAllUsernames() const
{
    return usernames;
}


Hue::Hue(const std::string& ip, const std::string& username, std::shared_ptr<const IHttpHandler> handler)
    : ip(ip),
    username(username),
    http_handler(std::move(handler)),
    commands(ip, username, http_handler),
    simpleBrightnessStrategy(std::make_shared<SimpleBrightnessStrategy>()),
    simpleColorHueStrategy(std::make_shared<SimpleColorHueStrategy>()),
    extendedColorHueStrategy(std::make_shared<ExtendedColorHueStrategy>()),
    simpleColorTemperatureStrategy(std::make_shared<SimpleColorTemperatureStrategy>()),
    extendedColorTemperatureStrategy(std::make_shared<ExtendedColorTemperatureStrategy>())
{}

std::string Hue::getBridgeIP()
{
    return ip;
}

std::string Hue::requestUsername(const std::string& ip)
{
    std::cout << "Please press the link Button! You've got 35 secs!\n";	// when the link button was presed we got 30 seconds to get our username for control

    Json::Value request;
    request["devicetype"] = "HuePlusPlus#User";

    Json::Value answer;
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point lastCheck;
    while (std::chrono::steady_clock::now() - start < std::chrono::seconds(35))
    {
        if (std::chrono::steady_clock::now() - lastCheck > std::chrono::seconds(1))
        {
            lastCheck = std::chrono::steady_clock::now();
            answer = http_handler->POSTJson("/api", request, ip);

            if (answer[0]["success"] != Json::nullValue)
            {
                // [{"success":{"username": "<username>"}}]
                username = answer[0]["success"]["username"].asString();
                this->ip = ip;
                //Update commands with new username and ip
                commands = HueCommandAPI(ip, username, http_handler);
                std::cout << "Success! Link button was pressed!\n";
                std::cout << "Username is \"" << username << "\"\n";
                break;
            }
            if (answer[0]["error"] != Json::nullValue)
            {
                std::cout << "Link button not pressed!\n";
            }
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

HueLight& Hue::getLight(int id)
{
    auto pos = lights.find(id);
    if (pos != lights.end())
    {
        pos->second.refreshState();
        return pos->second;
    }
    refreshState();
    if (!state["lights"].isMember(std::to_string(id)))
    {
        std::cerr << "Error in Hue getLight(): light with id " << id << " is not valid\n";
        throw(std::runtime_error("Error in Hue getLight(): light id is not valid"));
    }
    //std::cout << state["lights"][std::to_string(id)] << std::endl;
    std::string type = state["lights"][std::to_string(id)]["modelid"].asString();
    //std::cout << type << std::endl;
    if (type == "LCT001" || type == "LCT002" || type == "LCT003" || type == "LCT007" || type == "LLM001")
    {
        // HueExtendedColorLight Gamut B
        HueLight light = HueLight(id, commands, simpleBrightnessStrategy, extendedColorTemperatureStrategy, extendedColorHueStrategy);
        light.colorType = ColorType::GAMUT_B;
        lights.emplace(id, light);
        return lights.find(id)->second;
    }
    else if (type == "LCT010" || type == "LCT011" || type == "LCT014" || type == "LCT015" || type == "LCT016" || type == "LLC020" || type == "LST002")
    {
        // HueExtendedColorLight Gamut C
        HueLight light = HueLight(id, commands, simpleBrightnessStrategy, extendedColorTemperatureStrategy, extendedColorHueStrategy);
        light.colorType = ColorType::GAMUT_C;
        lights.emplace(id, light);
        return lights.find(id)->second;
    }
    else if (type == "LST001" || type == "LLC006" || type == "LLC007" || type == "LLC010" || type == "LLC011" || type == "LLC012" || type == "LLC013")
    {
        // HueColorLight Gamut A
        HueLight light = HueLight(id, commands, simpleBrightnessStrategy, nullptr, simpleColorHueStrategy);
        light.colorType = ColorType::GAMUT_A;
        lights.emplace(id, light);
        return lights.find(id)->second;
    }
    else if (type == "LWB004" || type == "LWB006" || type == "LWB007" || type == "LWB010" || type == "LWB014")
    {
        // HueDimmableLight No Color Type
        HueLight light = HueLight(id, commands, simpleBrightnessStrategy, nullptr, nullptr);
        light.colorType = ColorType::NONE;
        lights.emplace(id, light);
        return lights.find(id)->second;
    }
    else if (type == "LLM010" || type == "LLM011" || type == "LLM012" || type == "LTW001" || type == "LTW004" || type == "LTW013" || type == "LTW014")
    {
        // HueTemperatureLight
        HueLight light = HueLight(id, commands, simpleBrightnessStrategy, simpleColorTemperatureStrategy, nullptr);
        light.colorType = ColorType::TEMPERATURE;
        lights.emplace(id, light);
        return lights.find(id)->second;
    }
    std::cerr << "Could not determine HueLight type!\n";
    throw(std::runtime_error("Could not determine HueLight type!"));
}

bool Hue::removeLight(int id)
{
    Json::Value result = commands.DELETERequest("/lights/" + std::to_string(id), Json::objectValue);
    bool success = result.isArray() && !result[0].isNull() && result[0].isMember("success") && result[0]["success"] == "/lights/" + std::to_string(id) + " deleted";
    if (success && lights.count(id) != 0)
    {
        lights.erase(id);
    }
    return success;
}

std::vector<std::reference_wrapper<HueLight>> Hue::getAllLights()
{
    refreshState();
    for (const auto& id : state["lights"].getMemberNames())
    {
        getLight(std::stoi(id));
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
    refreshState();
    auto pos = lights.find(id);
    if (pos != lights.end())
    {
        return true;
    }
    if (state["lights"].isMember(std::to_string(id)))
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
    if (state["lights"].isMember(std::to_string(id)))
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
	if(model_id == "LCT001" || model_id == "LCT007" || model_id == "LCT010" || model_id == "LCT014" ||
		model_id == "LTW010" || model_id == "LTW001" || model_id == "LTW004" || model_id == "LTW015" ||
		model_id == "LWB004" || model_id == "LWB006")
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
	else if (model_id == "LLC005" || model_id == "LLC011" || model_id == "LLC012" ||
		model_id == "LLC007")
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
	else if (model_id == "HML001" || model_id == "HML002" || model_id == "HML003" ||
		model_id == "HML004" || model_id == "HML005")
	{
		ret.append("phoenix_ceiling_pendant_table_wall");
	}
	else if (model_id == "HML006")
	{
		ret.append("phoenix_down");
	}
	else if (model_id == "LTP001" || model_id == "LTP002" || model_id == "LTP003" ||
		model_id == "LTP004" || model_id == "LTP005" || model_id == "LTD003")
	{
		ret.append("pendant");
	}
	else if (model_id == "LDF002" || model_id == "LTF001" || model_id == "LTF002" ||
		model_id == "LTC001" || model_id == "LTC002" || model_id == "LTC003" ||
		model_id == "LTC004" || model_id == "LTD001" || model_id == "LTD002" ||
		model_id == "LDF001")
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

void Hue::refreshState()
{
    if (username.empty())
    {
        return;
    }
    Json::Value answer = commands.GETRequest("", Json::objectValue);
    if (answer.isObject() && answer.isMember("lights"))
    {
        state = answer;
    }
    else
    {
        std::cout << "Answer in Hue::refreshState of http_handler->GETJson(...) is not expected!\nAnswer:\n\t" << answer.toStyledString() << std::endl;
    }
}
