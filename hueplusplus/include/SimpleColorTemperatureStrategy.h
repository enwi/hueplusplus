/**
	\file SimpleColorTemperatureStrategy.h
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

#ifndef _SIMPLE_COLOR_TEMPERATURE_STRATEGY_H
#define _SIMPLE_COLOR_TEMPERATURE_STRATEGY_H

#include "ColorTemperatureStrategy.h"
#include "HueLight.h"

class SimpleColorTemperatureStrategy : public ColorTemperatureStrategy
{
    public:
        bool setColorTemperature(unsigned int mired, uint8_t transition, HueLight& light) const;
        bool alertTemperature(unsigned int mired, HueLight& light) const;
};

#endif