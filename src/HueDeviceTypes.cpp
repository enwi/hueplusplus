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

#include "hueplusplus/ExtendedColorHueStrategy.h"
#include "hueplusplus/ExtendedColorTemperatureStrategy.h"
#include "hueplusplus/HueDeviceTypes.h"
#include "hueplusplus/HueExceptionMacro.h"
#include "hueplusplus/SimpleBrightnessStrategy.h"
#include "hueplusplus/SimpleColorHueStrategy.h"
#include "hueplusplus/SimpleColorTemperatureStrategy.h"
#include "hueplusplus/Utils.h"

namespace hueplusplus
{
namespace
{
const std::set<std::string>& getGamutBTypes()
{
    static const std::set<std::string> c_EXTENDEDCOLORLIGHT_GAMUTB_TYPES
        = {"LCT001", "LCT002", "LCT003", "LCT007", "LLM001"};
    return c_EXTENDEDCOLORLIGHT_GAMUTB_TYPES;
};

const std::set<std::string>& getGamutCTypes()
{
    static const std::set<std::string> c_EXTENDEDCOLORLIGHT_GAMUTC_TYPES
        = {"LCT010", "LCT011", "LCT012", "LCT014", "LCT015", "LCT016", "LLC020", "LST002"};
    return c_EXTENDEDCOLORLIGHT_GAMUTC_TYPES;
}

const std::set<std::string>& getGamutATypes()
{
    static const std::set<std::string> c_EXTENDEDCOLORLIGHT_GAMUTA_TYPES
        = {"LST001", "LLC005", "LLC006", "LLC007", "LLC010", "LLC011", "LLC012", "LLC013", "LLC014"};
    return c_EXTENDEDCOLORLIGHT_GAMUTA_TYPES;
}
} // namespace

HueLightFactory::HueLightFactory(const HueCommandAPI& commands, std::chrono::steady_clock::duration refreshDuration)
    : commands(commands),
      refreshDuration(refreshDuration),
      simpleBrightness(std::make_shared<SimpleBrightnessStrategy>()),
      simpleColorTemperature(std::make_shared<SimpleColorTemperatureStrategy>()),
      extendedColorTemperature(std::make_shared<ExtendedColorTemperatureStrategy>()),
      simpleColorHue(std::make_shared<SimpleColorHueStrategy>()),
      extendedColorHue(std::make_shared<ExtendedColorHueStrategy>())
{}

HueLight HueLightFactory::createLight(const nlohmann::json& lightState, int id)
{
    std::string type = lightState.value("type", "");
    // Ignore case
    std::transform(type.begin(), type.end(), type.begin(), [](char c) { return std::tolower(c); });

    if (type == "on/off light" || type == "on/off plug-in unit")
    {
        HueLight light(id, commands, nullptr, nullptr, nullptr, refreshDuration);
        light.colorType = ColorType::NONE;
        return light;
    }
    else if (type == "dimmable light" || type =="dimmable plug-in unit")
    {
        HueLight light(id, commands, simpleBrightness, nullptr, nullptr, refreshDuration);
        light.colorType = ColorType::NONE;
        return light;
    }
    else if (type == "color temperature light")
    {
        HueLight light(id, commands, simpleBrightness, simpleColorTemperature, nullptr, refreshDuration);
        light.colorType = ColorType::TEMPERATURE;
        return light;
    }
    else if (type == "color light")
    {
        HueLight light(id, commands, simpleBrightness, nullptr, simpleColorHue, refreshDuration);
        light.colorType = getColorType(lightState, false);
        return light;
    }
    else if (type == "extended color light")
    {
        HueLight light(id, commands, simpleBrightness, extendedColorTemperature, extendedColorHue, refreshDuration);
        light.colorType = getColorType(lightState, true);
        return light;
    }
    std::cerr << "Could not determine HueLight type:" << type << "!\n";
    throw HueException(CURRENT_FILE_INFO, "Could not determine HueLight type!");
}

ColorType HueLightFactory::getColorType(const nlohmann::json& lightState, bool hasCt) const
{
    // Try to get color type via capabilities
    const nlohmann::json& gamuttype = utils::safeGetMember(lightState, "capabilities", "control", "colorgamuttype");
    if (gamuttype.is_string())
    {
        const std::string gamut = gamuttype.get<std::string>();
        if (gamut == "A")
        {
            return hasCt ? ColorType::GAMUT_A_TEMPERATURE : ColorType::GAMUT_A;
        }
        else if (gamut == "B")
        {
            return hasCt ? ColorType::GAMUT_B_TEMPERATURE : ColorType::GAMUT_B;
        }
        else if (gamut == "C")
        {
            return hasCt ? ColorType::GAMUT_C_TEMPERATURE : ColorType::GAMUT_C;
        }
        else
        {
            // Only other type is "Other" which does not have an enum value
            return ColorType::UNDEFINED;
        }
    }
    else
    {
        // Old version without capabilities, fall back to hardcoded types
        std::string modelid = lightState.at("modelid").get<std::string>();
        if (getGamutATypes().count(modelid))
        {
            return hasCt ? ColorType::GAMUT_A_TEMPERATURE : ColorType::GAMUT_A;
        }
        else if (getGamutBTypes().count(modelid))
        {
            return hasCt ? ColorType::GAMUT_B_TEMPERATURE : ColorType::GAMUT_B;
        }
        else if (getGamutCTypes().count(modelid))
        {
            return hasCt ? ColorType::GAMUT_C_TEMPERATURE : ColorType::GAMUT_C;
        }
        std::cerr << "Could not determine HueLight color type:" << modelid << "!\n";
        throw HueException(CURRENT_FILE_INFO, "Could not determine HueLight color type!");
    }
}
} // namespace hueplusplus
