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
    return transaction().setOn(true).setTransition(transition).commit();
}

bool HueLight::Off(uint8_t transition)
{
    return transaction().setOn(false).setTransition(transition).commit();
}

bool HueLight::isOn()
{
    return state.getValue().at("state").at("on").get<bool>();
}

bool HueLight::isOn() const
{
    return state.getValue().at("state").at("on").get<bool>();
}

int HueLight::getId() const
{
    return id;
}

std::string HueLight::getType() const
{
    return state.getValue()["type"];
}

std::string HueLight::getName()
{
    return state.getValue()["name"];
}

std::string HueLight::getName() const
{
    return state.getValue()["name"];
}

std::string HueLight::getModelId() const
{
    return state.getValue()["modelid"];
}

std::string HueLight::getUId() const
{
    return state.getValue().value("uniqueid", std::string());
}

std::string HueLight::getManufacturername() const
{
    return state.getValue().value("manufacturername", std::string());
}

std::string HueLight::getProductname() const
{
    return state.getValue().value("productname", std::string());
}

std::string HueLight::getLuminaireUId() const
{
    return state.getValue().value("luminaireuniqueid", std::string());
}

std::string HueLight::getSwVersion()
{
    return state.getValue()["swversion"];
}

std::string HueLight::getSwVersion() const
{
    return state.getValue()["swversion"];
}

bool HueLight::setName(const std::string& name)
{
    nlohmann::json request = nlohmann::json::object();
    request["name"] = name;
    nlohmann::json reply = sendPutRequest(request, "/name", CURRENT_FILE_INFO);
    state.refresh();

    // Check whether request was successful (returned name is not necessarily the actually set name)
    // If it already exists, a number is added, if it is too long to be returned, "Updated" is returned
    return utils::safeGetMember(reply, 0, "success", "/lights/" + std::to_string(id) + "/name").is_string();
}

ColorType HueLight::getColorType() const
{
    return colorType;
}

ColorGamut HueLight::getColorGamut() const
{
    switch (colorType)
    {
    case ColorType::GAMUT_A:
    case ColorType::GAMUT_A_TEMPERATURE:
        return gamut::gamutA;
    case ColorType::GAMUT_B:
    case ColorType::GAMUT_B_TEMPERATURE:
        return gamut::gamutB;
    case ColorType::GAMUT_C:
    case ColorType::GAMUT_C_TEMPERATURE:
        return gamut::gamutC;
    default: {
        const nlohmann::json& capabilitiesGamut
            = utils::safeGetMember(state.getValue(), "capabilities", "control", "colorgamut");
        if (capabilitiesGamut.is_array() && capabilitiesGamut.size() == 3)
        {
            // Other gamut
            return ColorGamut {{capabilitiesGamut[0].at(0), capabilitiesGamut[0].at(1)},
                {capabilitiesGamut[1].at(0), capabilitiesGamut[1].at(1)},
                {capabilitiesGamut[2].at(0), capabilitiesGamut[2].at(1)}};
        }
        // Unknown or no color light
        return gamut::maxGamut;
    }
    }
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
    return transaction().alert().commit();
}

StateTransaction HueLight::transaction()
{
    return StateTransaction(
        state.getCommandAPI(), "/lights/" + std::to_string(id) + "/state", &state.getValue().at("state"));
}

void HueLight::refresh()
{
    state.refresh();
}

HueLight::HueLight(int id, const HueCommandAPI& commands) : HueLight(id, commands, nullptr, nullptr, nullptr) {}

HueLight::HueLight(int id, const HueCommandAPI& commands, std::shared_ptr<const BrightnessStrategy> brightnessStrategy,
    std::shared_ptr<const ColorTemperatureStrategy> colorTempStrategy,
    std::shared_ptr<const ColorHueStrategy> colorHueStrategy, std::chrono::steady_clock::duration refreshDuration)
    : id(id),
      state("/lights/" + std::to_string(id), commands, refreshDuration),
      colorType(ColorType::NONE),
      brightnessStrategy(std::move(brightnessStrategy)),
      colorTemperatureStrategy(std::move(colorTempStrategy)),
      colorHueStrategy(std::move(colorHueStrategy))
{
    state.refresh();
}

nlohmann::json HueLight::sendPutRequest(const nlohmann::json& request, const std::string& subPath, FileInfo fileInfo)
{
    return state.getCommandAPI().PUTRequest("/lights/" + std::to_string(id) + subPath, request, std::move(fileInfo));
}
} // namespace hueplusplus
