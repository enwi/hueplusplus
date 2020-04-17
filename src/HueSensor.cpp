/**
    \file HueSensor.cpp
    Copyright Notice\n
    Copyright (C) 2020  Stefan Herbrechtsmeier - developer\n

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

#include "hueplusplus/HueSensor.h"

#include "hueplusplus/HueThing.h"
#include "json/json.hpp"

namespace hueplusplus
{
int HueSensor::getButtonEvent()
{
    refreshState();
    if (hasButtonEvent())
    {
        return state["state"]["buttonevent"];
    }
    return 0;
}

int HueSensor::getButtonEvent() const
{
    if (hasButtonEvent())
    {
        return state["state"]["buttonevent"];
    }
    return 0;
}

int HueSensor::getStatus()
{
    refreshState();
    if (hasStatus())
    {
        return state["state"]["status"];
    }
    return 0;
}

int HueSensor::getStatus() const
{
    if (hasStatus())
    {
        return state["state"]["status"];
    }
    return 0;
}

bool HueSensor::hasButtonEvent() const
{
    return state["state"].count("buttonevent") > 0;
}

bool HueSensor::hasStatus() const
{
    return state["state"].count("status") > 0;
}

HueSensor::HueSensor(int id, const HueCommandAPI& commands)
    : HueThing(id, commands, "/sensors/")
{
    refreshState();
}
} // namespace hueplusplus
