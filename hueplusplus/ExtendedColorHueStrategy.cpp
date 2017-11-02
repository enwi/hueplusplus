/**
	\file ExtendedColorHueStrategy.cpp
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

#include "include/ExtendedColorHueStrategy.h"
#include "include/HueConfig.h"

#include <cmath>
#include <iostream>
#include <thread>

bool ExtendedColorHueStrategy::alertHueSaturation(uint16_t hue, uint8_t sat, HueLight& light) const
{
	light.refreshState();
	std::string cType = light.state["state"]["colormode"].asString();
	bool on = light.state["state"]["on"].asBool();
	if (cType == "hs")
	{
		uint16_t oldHue = light.state["state"]["hue"].asUInt();
		uint8_t  oldSat = light.state["state"]["sat"].asUInt();
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
			return light.OffNoRefresh(1);
		}
		else
		{
			return light.setColorHueSaturation(oldHue, oldSat, 1);
		}
	}
	else if (cType == "xy")
	{
		float oldX = light.state["state"]["xy"][0].asFloat();
		float oldY = light.state["state"]["xy"][1].asFloat();
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
			return light.OffNoRefresh(1);
		}
		else
		{
			return light.setColorXY(oldX, oldY, 1);
		}
	}
	else if (cType == "ct")
	{
		uint16_t oldCT = light.state["state"]["ct"].asUInt();
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
			light.setColorTemperature(oldCT, 1);
			return light.OffNoRefresh(1);
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

bool ExtendedColorHueStrategy::alertXY(float x, float y, HueLight& light) const
{
	light.refreshState();
	std::string cType = light.state["state"]["colormode"].asString();
	bool on = light.state["state"]["on"].asBool();
	if (cType == "hs")
	{
		uint16_t oldHue = light.state["state"]["hue"].asUInt();
		uint8_t  oldSat = light.state["state"]["sat"].asUInt();
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
			return light.OffNoRefresh(1);
		}
		else
		{
			return light.setColorHueSaturation(oldHue, oldSat, 1);
		}
	}
	else if (cType == "xy")
	{
		float oldX = light.state["state"]["xy"][0].asFloat();
		float oldY = light.state["state"]["xy"][1].asFloat();
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
			return light.OffNoRefresh(1);
		}
		else
		{
			return light.setColorXY(oldX, oldY, 1);
		}
	}
	else if (cType == "ct")
	{
		uint16_t oldCT = light.state["state"]["ct"].asUInt();
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
			light.setColorTemperature(oldCT, 1);
			return light.OffNoRefresh(1);
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

bool ExtendedColorHueStrategy::alertRGB(uint8_t r, uint8_t g, uint8_t b, HueLight& light) const
{
	light.refreshState();
	std::string cType = light.state["state"]["colormode"].asString();
	bool on = light.state["state"]["on"].asBool();
	if (cType == "hs")
	{
		uint16_t oldHue = light.state["state"]["hue"].asUInt();
		uint8_t  oldSat = light.state["state"]["sat"].asUInt();
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
			return light.OffNoRefresh(1);
		}
		else
		{
			return light.setColorHueSaturation(oldHue, oldSat, 1);
		}
	}
	else if (cType == "xy")
	{
		float oldX = light.state["state"]["xy"][0].asFloat();
		float oldY = light.state["state"]["xy"][1].asFloat();
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
			return light.OffNoRefresh(1);
		}
		else
		{
			return light.setColorXY(oldX, oldY, 1);
		}
	}
	else if (cType == "ct")
	{
		uint16_t oldCT = light.state["state"]["ct"].asUInt();
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
			light.setColorTemperature(oldCT, 1);
			return light.OffNoRefresh(1);
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
