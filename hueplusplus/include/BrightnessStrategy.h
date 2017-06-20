/**
	\file BrightnessStrategy.h
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

#ifndef _BRIGHTNESS_STRATEGY_H
#define _BRIGHTNESS_STRATEGY_H

#include <stdint.h>

class HueLight;

class BrightnessStrategy
{
    public:
        virtual bool setBrightness(unsigned int bri, uint8_t transition, HueLight& light) const = 0;
        virtual ~BrightnessStrategy() {}
};

#endif