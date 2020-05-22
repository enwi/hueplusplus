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

#include <set>

#include "hueplusplus/HueExceptionMacro.h"
#include "hueplusplus/StateTransaction.h"
#include "hueplusplus/Utils.h"

namespace hueplusplus
{
StateTransaction::StateTransaction(const HueCommandAPI& commands, const std::string& path, nlohmann::json* currentState)
    : commands(commands), path(path), state(currentState), request(nlohmann::json::object())
{}

bool StateTransaction::commit(bool trimRequest) &&
{
    if (trimRequest)
    {
        this->trimRequest();
    }
    // Empty request or request with only transition makes no sense
    if (!request.empty() && !(request.size() == 1 && request.count("transitiontime")))
    {
        const nlohmann::json& stateJson = (state != nullptr) ? *state : nlohmann::json::object();
        if (!request.count("on"))
        {
            if (!stateJson.value("on", false)
                && (request.value("bri", 0) != 0 || request.count("effect") || request.count("hue")
                    || request.count("sat") || request.count("xy") || request.count("ct")))
            {
                // Turn on if it was turned off
                request["on"] = true;
            }
            else if (request.value("bri", 254) == 0 && stateJson.value("on", true))
            {
                // Turn off if brightness is 0
                request["on"] = false;
            }
        }

        nlohmann::json reply = commands.PUTRequest(path, request, CURRENT_FILE_INFO);
        if (utils::validatePUTReply(path, request, reply))
        {
            if (state != nullptr)
            {
                // Apply changes to state
                for (auto it = request.begin(); it != request.end(); ++it)
                {
                    if (it.key() == "transitiontime")
                    {
                        continue;
                    }
                    (*state)[it.key()] = it.value();
                }
            }
            return true;
        }
        return false;
    }
    return true;
}

ScheduleCommand StateTransaction::toScheduleCommand() &&
{
    nlohmann::json command {{"method", "PUT"}, {"address", commands.combinedPath(path)}, {"body", request}};
    return ScheduleCommand(command);
}

StateTransaction&& StateTransaction::setOn(bool on) &&
{
    request["on"] = on;
    return std::move(*this);
}

StateTransaction&& StateTransaction::setBrightness(uint8_t brightness) &&
{
    uint8_t clamped = std::min<uint8_t>(brightness, 254);
    request["bri"] = clamped;
    return std::move(*this);
}

StateTransaction&& StateTransaction::setColorSaturation(uint8_t saturation) &&
{
    uint8_t clamped = std::min<uint8_t>(saturation, 254);
    request["sat"] = clamped;
    return std::move(*this);
}

StateTransaction&& StateTransaction::setColorHue(uint16_t hue) &&
{
    request["hue"] = hue;
    return std::move(*this);
}

StateTransaction&& StateTransaction::setColorXY(float x, float y) &&
{
    float clampedX = std::max(0.f, std::min(x, 1.f));
    float clampedY = std::max(0.f, std::min(y, 1.f));
    request["xy"] = {clampedX, clampedY};
    return std::move(*this);
}

StateTransaction&& StateTransaction::setColorXY(const XYBrightness& xy) &&
{
    request["xy"] = {xy.xy.x, xy.xy.y};
    request["bri"] = static_cast<int>(std::round(xy.brightness * 255.f));

    return std::move(*this);
}

StateTransaction&& StateTransaction::setColorTemperature(unsigned int mired) &&
{
    unsigned int clamped = std::max(153u, std::min(mired, 500u));
    request["ct"] = clamped;
    return std::move(*this);
}

StateTransaction&& StateTransaction::setColorLoop(bool on) &&
{
    request["effect"] = on ? "colorloop" : "none";
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

void StateTransaction::trimRequest()
{
    static const std::map<std::string, std::string> colormodes
        = {{"sat", "hs"}, {"hue", "hs"}, {"xy", "xy"}, {"ct", "ct"}};
    static const std::set<std::string> otherRemove = {"on", "bri", "effect"};
    // Skip when there is no state provided (e.g. for groups)
    if (!state)
    {
        return;
    }
    for (auto it = request.begin(); it != request.end();)
    {
        auto colormodeIt = colormodes.find(it.key());
        if (colormodeIt != colormodes.end())
        {
            // Only erase color commands if colormode and value matches
            auto stateIt = state->find(it.key());
            if (stateIt != state->end() && state->value("colormode", "") == colormodeIt->second)
            {
                // Compare xy using float comparison
                if ((!it->is_array() && *stateIt == *it)
                    || (stateIt->is_array() && utils::floatEquals((*stateIt)[0].get<float>(), (*it)[0].get<float>())
                        && utils::floatEquals((*stateIt)[1].get<float>(), (*it)[1].get<float>())))
                {
                    it = request.erase(it);
                    continue;
                }
            }
        }
        else if (otherRemove.count(it.key()))
        {
            if (state->count(it.key()) && (*state)[it.key()] == *it)
            {
                it = request.erase(it);
                continue;
            }
        }
        ++it;
    }
}

} // namespace hueplusplus