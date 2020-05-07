/**
    \file ExtendedColorTemperatureStrategy.cpp
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

#include "hueplusplus/ExtendedColorTemperatureStrategy.h"

#include <cmath>
#include <iostream>
#include <thread>

#include "hueplusplus/HueConfig.h"
#include "hueplusplus/HueExceptionMacro.h"
#include "hueplusplus/Utils.h"

namespace hueplusplus
{
bool ExtendedColorTemperatureStrategy::setColorTemperature(
    unsigned int mired, uint8_t transition, HueLight& light) const
{
    // Careful, only use state until any light function might refresh the value and invalidate the reference
    const nlohmann::json& state = light.state.getValue()["state"];
    nlohmann::json request = nlohmann::json::object();
    if (transition != 4)
    {
        request["transitiontime"] = transition;
    }
    if (state["on"] != true)
    {
        request["on"] = true;
    }
    if (state["ct"] != mired || state["colormode"] != "ct")
    {
        if (mired > 500)
        {
            mired = 500;
        }
        if (mired < 153)
        {
            mired = 153;
        }
        request["ct"] = mired;
    }

    if (!request.count("on") && !request.count("ct"))
    {
        // Nothing needs to be changed
        return true;
    }

    nlohmann::json reply = light.sendPutRequest(request, "/state", CURRENT_FILE_INFO);

    // Check whether request was successful
    return utils::validateReplyForLight(request, reply, light.id);
}

bool ExtendedColorTemperatureStrategy::alertTemperature(unsigned int mired, HueLight& light) const
{
    // Careful, only use state until any light function might refresh the value and invalidate the reference
    const nlohmann::json& state = light.state.getValue()["state"];
    std::string cType = state["colormode"];
    bool on = state["on"];
    if (cType == "hs")
    {
        uint16_t oldHue = state["hue"];
        uint8_t oldSat = state["sat"];
        if (!light.setColorTemperature(mired, 1))
        {
            return false;
        }
        std::this_thread::sleep_for(Config::instance().getPreAlertDelay());
        if (!light.alert())
        {
            return false;
        }
        std::this_thread::sleep_for(Config::instance().getPostAlertDelay());
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
        if (!light.setColorTemperature(mired, 1))
        {
            return false;
        }
        std::this_thread::sleep_for(Config::instance().getPreAlertDelay());
        if (!light.alert())
        {
            return false;
        }
        std::this_thread::sleep_for(Config::instance().getPostAlertDelay());
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
        if (!light.setColorTemperature(mired, 1))
        {
            return false;
        }
        std::this_thread::sleep_for(Config::instance().getPreAlertDelay());
        if (!light.alert())
        {
            return false;
        }
        std::this_thread::sleep_for(Config::instance().getPostAlertDelay());
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
