/**
    \file Units.h
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

#ifndef INCLUDE_HUEPLUSPLUS_UNITS_H
#define INCLUDE_HUEPLUSPLUS_UNITS_H

#include <cstdint>

namespace hueplusplus
{

struct HueSaturation
{
    int hue;
    int saturation;
};

struct XY
{
    float x;
    float y;
};

struct RGB
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};
} // namespace hueplusplus

#endif
