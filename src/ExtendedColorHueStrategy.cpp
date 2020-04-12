/**
    \file ExtendedColorHueStrategy.cpp
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

#include "hueplusplus/ExtendedColorHueStrategy.h"

#include <cmath>
#include <iostream>
#include <thread>

#include "hueplusplus/HueConfig.h"

namespace hueplusplus
{
bool ExtendedColorHueStrategy::alertHueSaturation(uint16_t hue, uint8_t sat, HueLight& light) const
{
    // Careful, only use state until any light function might refresh the value and invalidate the reference
    const nlohmann::json& state = light.state.GetValue()["state"];
    std::string cType = state["colormode"];
    bool on = state["on"];
    if (cType == "hs")
    {
        uint16_t oldHue = state["hue"];
        uint8_t oldSat = state["sat"];
        if (!light.setColorHueSaturation(hue, sat, 1))
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(c_PRE_ALERT_DELAY));
        if (!light.alert())
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(c_POST_ALERT_DELAY));
        if (!on)
        {
            light.setColorHueSaturation(oldHue, oldSat, 1);
            return light.Off(1);
        }
        else
        {
            return light.setColorHueSaturation(oldHue, oldSat, 1);
        }
    }
    else if (cType == "xy")
    {
        float oldX = state["xy"][0];
        float oldY = state["xy"][1];
        if (!light.setColorHueSaturation(hue, sat, 1))
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(c_PRE_ALERT_DELAY));
        if (!light.alert())
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(c_POST_ALERT_DELAY));
        if (!on)
        {
            light.setColorXY(oldX, oldY, 1);
            return light.Off(1);
        }
        else
        {
            return light.setColorXY(oldX, oldY, 1);
        }
    }
    else if (cType == "ct")
    {
        uint16_t oldCT = light.state.GetValue()["state"]["ct"];
        if (!light.setColorHueSaturation(hue, sat, 1))
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(c_PRE_ALERT_DELAY));
        if (!light.alert())
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(c_POST_ALERT_DELAY));
        if (!on)
        {
            light.setColorTemperature(oldCT, 1);
            return light.Off(1);
        }
        else
        {
            return light.setColorTemperature(oldCT, 1);
        }
    }
    else
    {
        return false;
    }
}

bool ExtendedColorHueStrategy::alertXY(float x, float y, HueLight& light) const
{
    // Careful, only use state until any light function might refresh the value and invalidate the reference
    const nlohmann::json& state = light.state.GetValue()["state"];
    std::string cType = state["colormode"];
    bool on = state["on"];
    if (cType == "hs")
    {
        uint16_t oldHue = state["hue"];
        uint8_t oldSat = state["sat"];
        if (!light.setColorXY(x, y, 1))
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(c_PRE_ALERT_DELAY));
        if (!light.alert())
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(c_POST_ALERT_DELAY));
        if (!on)
        {
            light.setColorHueSaturation(oldHue, oldSat, 1);
            return light.Off(1);
        }
        else
        {
            return light.setColorHueSaturation(oldHue, oldSat, 1);
        }
    }
    else if (cType == "xy")
    {
        float oldX = state["xy"][0];
        float oldY = state["xy"][1];
        if (!light.setColorXY(x, y, 1))
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(c_PRE_ALERT_DELAY));
        if (!light.alert())
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(c_POST_ALERT_DELAY));
        if (!on)
        {
            light.setColorXY(oldX, oldY, 1);
            return light.Off(1);
        }
        else
        {
            return light.setColorXY(oldX, oldY, 1);
        }
    }
    else if (cType == "ct")
    {
        uint16_t oldCT = state["ct"];
        if (!light.setColorXY(x, y, 1))
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(c_PRE_ALERT_DELAY));
        if (!light.alert())
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(c_POST_ALERT_DELAY));
        if (!on)
        {
            light.setColorTemperature(oldCT, 1);
            return light.Off(1);
        }
        else
        {
            return light.setColorTemperature(oldCT, 1);
        }
    }
    else
    {
        return false;
    }
}

bool ExtendedColorHueStrategy::alertRGB(uint8_t r, uint8_t g, uint8_t b, HueLight& light) const
{
    // Careful, only use state until any light function might refresh the value and invalidate the reference
    const nlohmann::json& state = light.state.GetValue()["state"];
    std::string cType = state["colormode"];
    bool on = state["on"];
    if (cType == "hs")
    {
        uint16_t oldHue = state["hue"];
        uint8_t oldSat = state["sat"];
        if (!light.setColorRGB(r, g, b, 1))
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(c_PRE_ALERT_DELAY));
        if (!light.alert())
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(c_POST_ALERT_DELAY));
        if (!on)
        {
            light.setColorHueSaturation(oldHue, oldSat, 1);
            return light.Off(1);
        }
        else
        {
            return light.setColorHueSaturation(oldHue, oldSat, 1);
        }
    }
    else if (cType == "xy")
    {
        float oldX = state["xy"][0];
        float oldY = state["xy"][1];
        if (!light.setColorRGB(r, g, b, 1))
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(c_PRE_ALERT_DELAY));
        if (!light.alert())
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(c_POST_ALERT_DELAY));
        if (!on)
        {
            light.setColorXY(oldX, oldY, 1);
            return light.Off(1);
        }
        else
        {
            return light.setColorXY(oldX, oldY, 1);
        }
    }
    else if (cType == "ct")
    {
        uint16_t oldCT = state["ct"];
        if (!light.setColorRGB(r, g, b, 1))
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(c_PRE_ALERT_DELAY));
        if (!light.alert())
        {
            return false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(c_POST_ALERT_DELAY));
        if (!on)
        {
            light.setColorTemperature(oldCT, 1);
            return light.Off(1);
        }
        else
        {
            return light.setColorTemperature(oldCT, 1);
        }
    }
    else
    {
        return false;
    }
}
} // namespace hueplusplus
