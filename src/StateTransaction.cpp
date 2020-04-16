/**
    \file StateTransaction.cpp
    Copyright Notice\n
    Copyright (C) 2020  Jan Rogall		- developer\n
    Copyright (C) 2020  Moritz Wirger	- developer\n

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

#include "hueplusplus/StateTransaction.h"

#include "hueplusplus/HueExceptionMacro.h"
#include "hueplusplus/StateTransaction.h"
#include "hueplusplus/Utils.h"

namespace hueplusplus
{
StateTransaction::StateTransaction(
    const HueCommandAPI& commands, const std::string& path, const nlohmann::json& currentState)
    : commands(commands), path(path), state(currentState), request(nlohmann::json::object())
{
    assert(currentState.is_object());
}

bool StateTransaction::commit() &&
{
    // Empty request or request with only transition makes no sense
    if (!request.empty() && !(request.size() == 1 && request.count("transitiontime")))
    {
        if (!request.count("on"))
        {
            if (request.value("bri", 254) == 0 && state.value("on", true))
            {
                // Turn off if brightness is 0
                request["on"] = false;
            }
            else if (!state.value("on", false)
                && (request.value("bri", 0) != 0 || request.count("effect") || request.count("hue")
                    || request.count("sat") || request.count("xy") || request.count("ct")))
            {
                // Turn on if it was turned off
                request["on"] = true;
            }
        }

        nlohmann::json reply = commands.PUTRequest(path, request, CURRENT_FILE_INFO);
        return utils::validatePUTReply(path, request, reply);
    }
    return true;
}

StateTransaction&& StateTransaction::setOn(bool on) &&
{
    if (!state.count("on") || state["on"] != on)
    {
        request["on"] = on;
    }
    return std::move(*this);
}

StateTransaction&& StateTransaction::setBrightness(uint8_t brightness) &&
{
    uint8_t clamped = std::min<uint8_t>(brightness, 254);
    if (!state.count("bri") || state["bri"].get<unsigned int>() != clamped)
    {
        request["bri"] = clamped;
    }
    return std::move(*this);
}

StateTransaction&& StateTransaction::setColorSaturation(uint8_t saturation) &&
{
    uint8_t clamped = std::min<uint8_t>(saturation, 254);
    if (!state.count("sat") || state["sat"].get<unsigned int>() != clamped || state.value("colormode", "") != "hs")
    {
        request["sat"] = clamped;
    }
    return std::move(*this);
}

StateTransaction&& StateTransaction::setColorHue(uint16_t hue) &&
{
    if (!state.count("hue") || state["hue"].get<int>() != hue || state.value("colormode", "") != "hs")
    {
        request["hue"] = hue;
    }
    return std::move(*this);
}

StateTransaction&& StateTransaction::setColorXY(float x, float y) &&
{
    float clampedX = std::max(0.f, std::min(x, 1.f));
    float clampedY = std::max(0.f, std::min(y, 1.f));
    if (!state.count("xy") || !state.count("colormode") || !state["xy"].is_array()
        || !utils::floatEquals(state["xy"][0].get<float>(), clampedX)
        || !utils::floatEquals(state["xy"][1].get<float>(), clampedY) || state["colormode"] != "xy")
    {
        request["xy"] = {clampedX, clampedY};
    }
    return std::move(*this);
}

StateTransaction&& StateTransaction::setColorTemperature(unsigned int mired) &&
{
    unsigned int clamped = std::max(153u, std::min(mired, 500u));
    if (state.value("ct", 0u) != clamped || state.value("colormode", "") != "ct")
    {
        request["ct"] = clamped;
    }
    return std::move(*this);
}

StateTransaction&& StateTransaction::setColorLoop(bool on) &&
{
    std::string effect = on ? "colorloop" : "none";
    if (state.value("effect", "") != effect)
    {
        request["effect"] = effect;
    }
    return std::move(*this);
}

StateTransaction&& StateTransaction::incrementBrightness(int increment) &&
{
    request["bri_inc"] = std::max(-254, std::min(increment, 254));
    return std::move(*this);
}

StateTransaction&& StateTransaction::incrementSaturation(int increment) &&
{
    request["sat_inc"] = std::max(-254, std::min(increment, 254));
    return std::move(*this);
}

StateTransaction&& StateTransaction::incrementHue(int increment) &&
{
    request["hue_inc"] = std::max(-65534, std::min(increment, 65534));
    return std::move(*this);
}

StateTransaction&& StateTransaction::incrementColorTemperature(int increment) &&
{
    request["ct_inc"] = std::max(-65534, std::min(increment, 65534));
    return std::move(*this);
}

StateTransaction&& StateTransaction::incrementColorXY(float xInc, float yInc) &&
{
    request["xy_inc"] = {std::max(-0.5f, std::min(xInc, 0.5f)), std::max(-0.5f, std::min(yInc, 0.5f))};
    return std::move(*this);
}

StateTransaction&& StateTransaction::setTransition(uint16_t transition) &&
{
    if (transition != 4)
    {
        request["transitiontime"] = transition;
    }
    return std::move(*this);
}
StateTransaction&& StateTransaction::alert() &&
{
    request["alert"] = "select";
    return std::move(*this);
}
StateTransaction&& StateTransaction::longAlert() &&
{
    request["alert"] = "lselect";
    return std::move(*this);
}
StateTransaction&& StateTransaction::stopAlert() &&
{
    request["alert"] = "none";
    return std::move(*this);
}
} // namespace hueplusplus