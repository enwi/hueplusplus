/**
    \file StateTransaction.h
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

#ifndef INCLUDE_HUEPLUSPLUS_STATE_TRANSACTION_H
#define INCLUDE_HUEPLUSPLUS_STATE_TRANSACTION_H

#include <string>

#include "HueCommandAPI.h"

#include "json/json.hpp"

namespace hueplusplus
{
class StateTransaction
{
public:
    StateTransaction(const HueCommandAPI& commands, const std::string& path, const nlohmann::json& currentState);

    StateTransaction(const StateTransaction&) = delete;
    StateTransaction(StateTransaction&&) = default;

    bool commit() &&;

    StateTransaction&& setOn(bool on) &&;
    StateTransaction&& setBrightness(uint8_t brightness) &&;
    StateTransaction&& setColorHue(uint16_t hue) &&;
    StateTransaction&& setColorSaturation(uint8_t saturation) &&;
    StateTransaction&& setColorHueSaturation(uint16_t hue, uint8_t saturation) &&;
    StateTransaction&& setColorXY(float x, float y) &&;
    StateTransaction&& setColorTemperature(unsigned int mired) &&;
    StateTransaction&& setColorLoop(bool on) &&;
    StateTransaction&& incrementBrightness(int increment) &&;
    StateTransaction&& incrementSaturation(int increment) &&;
    StateTransaction&& incrementHue(int increment) &&;
    StateTransaction&& incrementColorTemperature(int increment) &&;
    StateTransaction&& incrementColorXY(float xInc, float yInc) &&;
    StateTransaction&& setTransition(uint16_t transition) &&;
    StateTransaction&& alert() &&;
    StateTransaction&& longAlert() &&;
    StateTransaction&& stopAlert() &&;

protected:
    const HueCommandAPI& commands;
    std::string path;
    nlohmann::json state;
    nlohmann::json request;
};

} // namespace hueplusplus

#endif