/**
    \file SimpleColorHueStrategy.cpp
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

#include "hueplusplus/SimpleColorHueStrategy.h"

#include <cmath>
#include <iostream>
#include <thread>

#include "hueplusplus/HueConfig.h"
#include "hueplusplus/HueExceptionMacro.h"
#include "hueplusplus/Utils.h"

namespace hueplusplus
{
bool SimpleColorHueStrategy::setColorHue(uint16_t hue, uint8_t transition, HueLight& light) const
{
    return light.transaction().setColorHue(hue).setTransition(transition).commit();
}

bool SimpleColorHueStrategy::setColorSaturation(uint8_t sat, uint8_t transition, HueLight& light) const
{
    return light.transaction().setColorSaturation(sat).setTransition(transition).commit();
}

bool SimpleColorHueStrategy::setColorHueSaturation(uint16_t hue, uint8_t sat, uint8_t transition, HueLight& light) const
{
    return light.transaction().setColorHueSaturation(hue, sat).setTransition(transition).commit();
}

bool SimpleColorHueStrategy::setColorXY(float x, float y, uint8_t transition, HueLight& light) const
{
    return light.transaction().setColorXY(x, y).setTransition(transition).commit();
}

bool SimpleColorHueStrategy::setColorRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t transition, HueLight& light) const
{
    if ((r == 0) && (g == 0) && (b == 0))
    {
        return light.Off();
    }

    const float red = float(r) / 255;
    const float green = float(g) / 255;
    const float blue = float(b) / 255;

    // gamma correction
    const float redCorrected = (red > 0.04045f) ? pow((red + 0.055f) / (1.0f + 0.055f), 2.4f) : (red / 12.92f);
    const float greenCorrected = (green > 0.04045f) ? pow((green + 0.055f) / (1.0f + 0.055f), 2.4f) : (green / 12.92f);
    const float blueCorrected = (blue > 0.04045f) ? pow((blue + 0.055f) / (1.0f + 0.055f), 2.4f) : (blue / 12.92f);

    const float X = redCorrected * 0.664511f + greenCorrected * 0.154324f + blueCorrected * 0.162028f;
    const float Y = redCorrected * 0.283881f + greenCorrected * 0.668433f + blueCorrected * 0.047685f;
    const float Z = redCorrected * 0.000088f + greenCorrected * 0.072310f + blueCorrected * 0.986039f;

    const float x = X / (X + Y + Z);
    const float y = Y / (X + Y + Z);

    return light.setColorXY(x, y, transition);
}

bool SimpleColorHueStrategy::setColorLoop(bool on, HueLight& light) const
{
    return light.transaction().setColorLoop(true).commit();
}

bool SimpleColorHueStrategy::alertHueSaturation(uint16_t hue, uint8_t sat, HueLight& light) const
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
    else
    {
        return false;
    }
}

bool SimpleColorHueStrategy::alertXY(float x, float y, HueLight& light) const
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
    else
    {
        return false;
    }
}

bool SimpleColorHueStrategy::alertRGB(uint8_t r, uint8_t g, uint8_t b, HueLight& light) const
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
    else
    {
        return false;
    }
}

std::pair<uint16_t, uint8_t> SimpleColorHueStrategy::getColorHueSaturation(HueLight& light) const
{
    // Save value, so there are no inconsistent results if it is refreshed between two calls
    const nlohmann::json& state = light.state.GetValue()["state"];
    return std::pair<uint16_t, uint8_t>(static_cast<uint16_t>(state["hue"]), static_cast<uint8_t>(state["sat"]));
}

std::pair<uint16_t, uint8_t> SimpleColorHueStrategy::getColorHueSaturation(const HueLight& light) const
{
    return std::pair<uint16_t, uint8_t>(static_cast<uint16_t>(light.state.GetValue()["state"]["hue"]),
        static_cast<uint8_t>(light.state.GetValue()["state"]["sat"]));
}

std::pair<float, float> SimpleColorHueStrategy::getColorXY(HueLight& light) const
{
    // Save value, so there are no inconsistent results if it is refreshed between two calls
    const nlohmann::json& state = light.state.GetValue()["state"];
    return std::pair<float, float>(state["xy"][0], state["xy"][1]);
}

std::pair<float, float> SimpleColorHueStrategy::getColorXY(const HueLight& light) const
{
    return std::pair<float, float>(light.state.GetValue()["state"]["xy"][0], light.state.GetValue()["state"]["xy"][1]);
}
/*bool SimpleColorHueStrategy::pointInTriangle(float pointx, float pointy, float
x0, float y0, float x1, float y1, float x2, float y2)
{
float A = (-y1 * x2 + y0*(-x1 + x2) + x0*(y1 - y2) + x1 * y1);
int8_t sign = A < 0 ? -1 : 1;
float s = (y0 * x2 - x0 * y2 + (y2 - y0) * pointx + (x0 - x2) * pointy) * sign;
float t = (x0 * y1 - y0 * x1 + (y0 - y1) * pointx + (x1 - x0) * pointy) * sign;

return s > 0 && t > 0 && (s + t) < A * sign;
}*/
} // namespace hueplusplus
