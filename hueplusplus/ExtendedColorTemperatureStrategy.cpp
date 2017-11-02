/**
	\file ExtendedColorTemperatureStrategy.cpp
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

#include "include/ExtendedColorTemperatureStrategy.h"
#include "include/HueConfig.h"

#include <cmath>
#include <iostream>
#include <thread>


bool ExtendedColorTemperatureStrategy::setColorTemperature(unsigned int mired, uint8_t transition, HueLight& light) const
{
	light.refreshState();
	Json::Value request(Json::objectValue);
	if (transition != 4)
	{
		request["transitiontime"] = transition;
	}
	if (light.state["state"]["on"].asBool() != true)
	{
		request["on"] = true;
	}
	if (light.state["state"]["ct"].asUInt() != mired || light.state["state"]["colormode"].asString() != "ct")
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

	if (!request.isMember("on") && !request.isMember("ct"))
	{
		//Nothing needs to be changed
		return true;
	}

	Json::Value reply = light.SendPutRequest(request, "/state");

	//Check whether request was successful
	std::string path = "/lights/" + std::to_string(light.id) + "/state/";
	bool success = true;
	int i = 0;
	if (success && request.isMember("transitiontime"))
	{
		//Check if success was sent and the value was changed
		success = !reply[i].isNull() && reply[i].isMember("success") && reply[i]["success"][path + "transitiontime"].asUInt() == request["transitiontime"].asUInt();
		++i;
	}
	if (success && request.isMember("on"))
	{
		//Check if success was sent and the value was changed
		success = !reply[i].isNull() && reply[i].isMember("success") && reply[i]["success"][path + "on"].asBool() == request["on"].asBool();
		++i;
	}
	if (success && request.isMember("ct"))
	{
		//Check if success was sent and the value was changed
		success = !reply[i].isNull() && reply[i].isMember("success") && reply[i]["success"][path + "ct"].asUInt() == request["ct"].asUInt();
	}
	return success;
}

bool ExtendedColorTemperatureStrategy::alertTemperature(unsigned int mired, HueLight& light) const
{
	light.refreshState();
	std::string cType = light.state["state"]["colormode"].asString();
	bool on = light.state["state"]["on"].asBool();
	if (cType == "hs")
	{
		uint16_t oldHue = light.state["state"]["hue"].asUInt();
		uint8_t oldSat = light.state["state"]["sat"].asUInt();
		if (!light.setColorTemperature(mired, 1))
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
		if (!light.setColorTemperature(mired, 1))
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
		if (!light.setColorTemperature(mired, 1))
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
