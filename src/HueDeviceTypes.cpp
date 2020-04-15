/**
    \file HueDeviceTypes.cpp
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

#include "hueplusplus/HueDeviceTypes.h"

#include <set>

#include "hueplusplus/HueExceptionMacro.h"

namespace hueplusplus
{
namespace
{
const std::set<std::string> getGamutBTypes()
{
    static const std::set<std::string> c_EXTENDEDCOLORLIGHT_GAMUTB_TYPES
        = {"LCT001", "LCT002", "LCT003", "LCT007", "LLM001"};
    return c_EXTENDEDCOLORLIGHT_GAMUTB_TYPES;
};

const std::set<std::string> getGamutCTypes()
{
    static const std::set<std::string> c_EXTENDEDCOLORLIGHT_GAMUTC_TYPES
        = {"LCT010", "LCT011", "LCT012", "LCT014", "LCT015", "LCT016", "LLC020", "LST002"};
    return c_EXTENDEDCOLORLIGHT_GAMUTC_TYPES;
}

const std::set<std::string> getGamutATypes()
{
    static const std::set<std::string> c_EXTENDEDCOLORLIGHT_GAMUTA_TYPES
        = {"LST001", "LLC005", "LLC006", "LLC007", "LLC010", "LLC011", "LLC012", "LLC013", "LLC014"};
    return c_EXTENDEDCOLORLIGHT_GAMUTA_TYPES;
}

const std::set<std::string> getNoColorTypes()
{
    static const std::set<std::string> c_DIMMABLELIGHT_NO_COLOR_TYPES
        = {"LWB004", "LWB006", "LWB007", "LWB010", "LWB014", "LDF001", "LDF002", "LDD001", "LDD002", "MWM001"};
    return c_DIMMABLELIGHT_NO_COLOR_TYPES;
}

const std::set<std::string> getTemperatureLightTypes()
{
    static const std::set<std::string> c_TEMPERATURELIGHT_TYPES
        = {"LLM010", "LLM011", "LLM012", "LTW001", "LTW004", "LTW010", "LTW011", "LTW012", "LTW013", "LTW014", "LTW015",
            "LTP001", "LTP002", "LTP003", "LTP004", "LTP005", "LTD003", "LTF001", "LTF002", "LTC001", "LTC002",
            "LTC003", "LTC004", "LTC011", "LTC012", "LTD001", "LTD002", "LFF001", "LTT001", "LDT001"};
    return c_TEMPERATURELIGHT_TYPES;
}
} // namespace

auto MakeHueLight::operator()(std::string type, int id, HueCommandAPI commands,
    std::shared_ptr<BrightnessStrategy> simpleBrightnessStrategy,
    std::shared_ptr<ColorTemperatureStrategy> extendedColorTemperatureStrategy,
    std::shared_ptr<ColorTemperatureStrategy> simpleColorTemperatureStrategy,
    std::shared_ptr<ColorHueStrategy> extendedColorHueStrategy,
    std::shared_ptr<ColorHueStrategy> simpleColorHueStrategy) -> HueLight
{
    if (getGamutBTypes().count(type))
    {
        auto light = HueLight(
            id, commands, simpleBrightnessStrategy, extendedColorTemperatureStrategy, extendedColorHueStrategy);
        light.colorType = ColorType::GAMUT_B;
        return light;
    }

    else if (getGamutCTypes().count(type))
    {
        auto light = HueLight(
            id, commands, simpleBrightnessStrategy, extendedColorTemperatureStrategy, extendedColorHueStrategy);
        light.colorType = ColorType::GAMUT_C;
        return light;
    }

    else if (getGamutATypes().count(type))
    {
        auto light = HueLight(id, commands, simpleBrightnessStrategy, nullptr, simpleColorHueStrategy);
        light.colorType = ColorType::GAMUT_A;
        return light;
    }

    else if (getNoColorTypes().count(type))
    {
        auto light = HueLight(id, commands, simpleBrightnessStrategy, nullptr, nullptr);
        light.colorType = ColorType::NONE;
        return light;
    }

    else if (getTemperatureLightTypes().count(type))
    {
        auto light = HueLight(id, commands, simpleBrightnessStrategy, simpleColorTemperatureStrategy, nullptr);
        light.colorType = ColorType::TEMPERATURE;
        return light;
    }
    std::cerr << "Could not determine HueLight type:" << type << "!\n";
    throw HueException(CURRENT_FILE_INFO, "Could not determine HueLight type!");
}
} // namespace hueplusplus
