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

#include "hueplusplus/HueLight.h"

#include <cmath>
#include <iostream>
#include <thread>

#include "hueplusplus/HueExceptionMacro.h"
#include "hueplusplus/Utils.h"
#include "json/json.hpp"

namespace hueplusplus
{
bool HueLight::On(uint8_t transition)
{
    nlohmann::json request = nlohmann::json::object();
    if (transition != 4)
    {
        request["transitiontime"] = transition;
    }
    if (state.GetValue()["state"]["on"] != true)
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

bool HueLight::Off(uint8_t transition)
{
    nlohmann::json request = nlohmann::json::object();
    if (transition != 4)
    {
        request["transitiontime"] = transition;
    }
    if (state.GetValue()["state"]["on"] != false)
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

bool HueLight::isOn()
{
    return state.GetValue()["state"]["on"];
}

bool HueLight::isOn() const
{
    return state.GetValue()["state"]["on"];
}

int HueLight::getId() const
{
    return id;
}

std::string HueLight::getType() const
{
    return state.GetValue()["type"];
}

std::string HueLight::getName()
{
    return state.GetValue()["name"];
}

std::string HueLight::getName() const
{
    return state.GetValue()["name"];
}

std::string HueLight::getModelId() const
{
    return state.GetValue()["modelid"];
}

std::string HueLight::getUId() const
{
    return state.GetValue().value("uniqueid", std::string());
}

std::string HueLight::getManufacturername() const
{
    return state.GetValue().value("manufacturername", std::string());
}

std::string HueLight::getProductname() const
{
    return state.GetValue().value("productname", std::string());
}

std::string HueLight::getLuminaireUId() const
{
    return state.GetValue().value("luminaireuniqueid", std::string());
}

std::string HueLight::getSwVersion()
{
    return state.GetValue()["swversion"];
}

std::string HueLight::getSwVersion() const
{
    return state.GetValue()["swversion"];
}

bool HueLight::setName(const std::string& name)
{
    nlohmann::json request = nlohmann::json::object();
    request["name"] = name;
    nlohmann::json reply = SendPutRequest(request, "/name", CURRENT_FILE_INFO);

    // Check whether request was successful (returned name is not necessarily the actually set name)
    // If it already exists, a number is added, if it is too long to be returned, "Updated" is returned
    return utils::safeGetMember(reply, 0, "success", "/lights/" + std::to_string(id) + "/name").is_string();
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
    std::shared_ptr<const ColorHueStrategy> colorHueStrategy, std::chrono::steady_clock::duration refreshDuration)
    : id(id),
      brightnessStrategy(std::move(brightnessStrategy)),
      colorTemperatureStrategy(std::move(colorTempStrategy)),
      colorHueStrategy(std::move(colorHueStrategy)),
      commands(commands),
      state("/lights/" + std::to_string(id), commands, refreshDuration)

{
    state.Refresh();
}

nlohmann::json HueLight::SendPutRequest(const nlohmann::json& request, const std::string& subPath, FileInfo fileInfo)
{
    return commands.PUTRequest("/lights/" + std::to_string(id) + subPath, request, std::move(fileInfo));
}
} // namespace hueplusplus
