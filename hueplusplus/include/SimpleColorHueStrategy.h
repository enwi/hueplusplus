/**
	\file SimpleColorHueStrategy.h
	Copyright Notice\n
	Copyright (C) 2017  Jan Rogall		- developer\n
	Copyright (C) 2017  Moritz Wirger	- developer\n

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 3 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software Foundation,
	Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**/

#ifndef _SIMPLE_COLOR_HUE_STRATEGY_H
#define _SIMPLE_COLOR_HUE_STRATEGY_H

#include "HueLight.h"

class SimpleColorHueStrategy : public ColorHueStrategy
{
    public:
        bool setColorHue(uint16_t hue, uint8_t transition, HueLight& light) const;
		bool setColorSaturation(uint8_t sat, uint8_t transition, HueLight& light) const;
        bool setColorHueSaturation(uint16_t hue, uint8_t sat, uint8_t transition, HueLight& light) const;
        bool setColorXY(float x, float y, uint8_t transition, HueLight& light) const;
        bool setColorRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t transition, HueLight& light) const;
        bool setColorLoop(bool on, HueLight& light) const;
        bool alertHueSaturation(uint16_t hue, uint8_t sat, HueLight& light) const;
        bool alertXY(float x, float y, HueLight& light) const;
        bool alertRGB(uint8_t r, uint8_t g, uint8_t b, HueLight& light) const;
};

#endif