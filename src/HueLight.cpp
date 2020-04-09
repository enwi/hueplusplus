/**
    \file HueLight.cpp
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

#include "include/HueLight.h"

#include <cmath>
#include <iostream>
#include <thread>

#include "include/HueExceptionMacro.h"
#include "include/Utils.h"
#include "include/json/json.hpp"

bool HueLight::On(uint8_t transition)
{
    refreshState();
    return OnNoRefresh(transition);
}

bool HueLight::Off(uint8_t transition)
{
    refreshState();
    return OffNoRefresh(transition);
}

bool HueLight::isOn()
{
    refreshState();
    return state["state"]["on"];
}

bool HueLight::isOn() const
{
    return state["state"]["on"];
}

int HueLight::getId() const
{
    return id;
}

std::string HueLight::getType() const
{
    return state["type"];
}

std::string HueLight::getName()
{
    refreshState();
    return state["name"];
}

std::string HueLight::getName() const
{
    return state["name"];
}

std::string HueLight::getModelId() const
{
    return state["modelid"];
}

std::string HueLight::getUId() const
{
    if (state.count("uniqueid"))
    {
        return state["uniqueid"];
    }
    return std::string();
}

std::string HueLight::getManufacturername() const
{
    if (state.count("manufacturername"))
    {
        return state["manufacturername"];
    }
    return std::string();
}

std::string HueLight::getProductname() const
{
    if (state.count("productname"))
    {
        return state["productname"];
    }
    return std::string();
}

std::string HueLight::getLuminaireUId() const
{
    if (state.count("luminaireuniqueid"))
    {
        return state["luminaireuniqueid"];
    }
    return std::string();
}

std::string HueLight::getSwVersion()
{
    refreshState();
    return state["swversion"];
}

std::string HueLight::getSwVersion() const
{
    return state["swversion"];
}

bool HueLight::setName(const std::string& name)
{
    nlohmann::json request = nlohmann::json::object();
    request["name"] = name;
    nlohmann::json reply = SendPutRequest(request, "/name", CURRENT_FILE_INFO);

    // Check whether request was successful
    return utils::safeGetMember(reply, 0, "success", "/lights/" + std::to_string(id) + "/name") == name;
}

ColorType HueLight::getColorType() const
{
    return colorType;
}

unsigned int HueLight::KelvinToMired(unsigned int kelvin) const
{
    return int(0.5f + (1000000 / kelvin));
}

unsigned int HueLight::MiredToKelvin(unsigned int mired) const
{
    return int(0.5f + (1000000 / mired));
}

bool HueLight::alert()
{
    nlohmann::json request;
    request["alert"] = "select";

    nlohmann::json reply = SendPutRequest(request, "/state", CURRENT_FILE_INFO);

    return utils::validateReplyForLight(request, reply, id);
}

HueLight::HueLight(int id, const HueCommandAPI& commands) : HueLight(id, commands, nullptr, nullptr, nullptr) {}

HueLight::HueLight(int id, const HueCommandAPI& commands, std::shared_ptr<const BrightnessStrategy> brightnessStrategy,
    std::shared_ptr<const ColorTemperatureStrategy> colorTempStrategy,
    std::shared_ptr<const ColorHueStrategy> colorHueStrategy)
    : id(id),
      brightnessStrategy(std::move(brightnessStrategy)),
      colorTemperatureStrategy(std::move(colorTempStrategy)),
      colorHueStrategy(std::move(colorHueStrategy)),
      commands(commands)

{
    refreshState();
}

bool HueLight::OnNoRefresh(uint8_t transition)
{
    nlohmann::json request = nlohmann::json::object();
    if (transition != 4)
    {
        request["transitiontime"] = transition;
    }
    if (state["state"]["on"] != true)
    {
        request["on"] = true;
    }

    if (!request.count("on"))
    {
        // Nothing needs to be changed
        return true;
    }

    nlohmann::json reply = SendPutRequest(request, "/state", CURRENT_FILE_INFO);

    // Check whether request was successful
    return utils::validateReplyForLight(request, reply, id);
}

bool HueLight::OffNoRefresh(uint8_t transition)
{
    nlohmann::json request = nlohmann::json::object();
    if (transition != 4)
    {
        request["transitiontime"] = transition;
    }
    if (state["state"]["on"] != false)
    {
        request["on"] = false;
    }

    if (!request.count("on"))
    {
        // Nothing needs to be changed
        return true;
    }

    nlohmann::json reply = SendPutRequest(request, "/state", CURRENT_FILE_INFO);

    // Check whether request was successful
    return utils::validateReplyForLight(request, reply, id);
}

nlohmann::json HueLight::SendPutRequest(const nlohmann::json& request, const std::string& subPath, FileInfo fileInfo)
{
    return commands.PUTRequest("/lights/" + std::to_string(id) + subPath, request, std::move(fileInfo));
}

void HueLight::refreshState()
{
    // std::chrono::steady_clock::time_point start =
    // std::chrono::steady_clock::now(); std::cout << "\tRefreshing lampstate of
    // lamp with id: " << id << ", ip: " << ip << "\n";
    nlohmann::json answer
        = commands.GETRequest("/lights/" + std::to_string(id), nlohmann::json::object(), CURRENT_FILE_INFO);
    if (answer.count("state"))
    {
        state = answer;
    }
    else
    {
        std::cout << "Answer in HueLight::refreshState of "
                     "http_handler->GETJson(...) is not expected!\nAnswer:\n\t"
                  << answer.dump() << std::endl;
    }
    // std::cout << "\tRefresh state took: " <<
    // std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()
    // - start).count() << "ms" << std::endl;
}
