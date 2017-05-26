/**
	\file HueExtendedColorLight.cpp
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

#include "include/HueExtendedColorLight.h"

#include <cmath>
#include <iostream>
#include <thread>

bool HueExtendedColorLight::setColorTemperature(unsigned int mired, uint8_t transistion)
{
	refreshState();
	Json::Value request(Json::objectValue);
	if (transistion != 4)
	{
		request["transitiontime"] = transistion;
	}
	if (state["state"]["on"].asBool() != true)
	{
		request["on"] = true;
	}
	if (state["state"]["ct"].asUInt() != mired || state["state"]["colormode"].asString() != "ct")
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

	Json::Value reply = SendPutRequest(request);

	//Check whether request was successful
	std::string path = "/lights/" + std::to_string(id) + "/state/";
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

bool HueExtendedColorLight::alertTemperature(unsigned int mired)
{
	refreshState();
	std::string cType = state["state"]["colormode"].asString();
	bool on = state["state"]["on"].asBool();
	if (cType == "hs")
	{
		uint16_t oldHue = state["state"]["hue"].asUInt();
		uint8_t oldSat = state["state"]["sat"].asUInt();
		if (!setColorTemperature(mired, 1))
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(110));
		if (!alert())
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		if (!on)
		{
			return OffNoRefresh(1);
		}
		else
		{
			return setColorHueSaturation(oldHue, oldSat, 1);
		}
	}
	else if (cType == "xy")
	{
		float oldX = state["state"]["xy"][0].asFloat();
		float oldY = state["state"]["xy"][1].asFloat();
		if (!setColorTemperature(mired, 1))
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(110));
		if (!alert())
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		if (!on)
		{
			return OffNoRefresh(1);
		}
		else
		{
			return setColorXY(oldX, oldY, 1);
		}
	}
	else if (cType == "ct")
	{
		uint16_t oldCT = state["state"]["ct"].asUInt();
		if (!setColorTemperature(mired, 1))
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(110));
		if (!alert())
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		if (!on)
		{
			return OffNoRefresh(1);
		}
		else
		{
			return setColorTemperature(oldCT, 1);
		}
	}
	else
	{
		return false;
	}
}

bool HueExtendedColorLight::alertHueSaturation(uint16_t hue, uint8_t sat)
{
	refreshState();
	std::string cType = state["state"]["colormode"].asString();
	bool on = state["state"]["on"].asBool();
	if (cType == "hs")
	{
		uint16_t oldHue = state["state"]["hue"].asUInt();
		uint8_t oldSat = state["state"]["sat"].asUInt();
		if (!setColorHueSaturation(hue, sat, 1))
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(110));
		if (!alert())
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		if (!on)
		{
			return OffNoRefresh(1);
		}
		else
		{
			return setColorHueSaturation(oldHue, oldSat, 1);
		}
	}
	else if (cType == "xy")
	{
		float oldX = state["state"]["xy"][0].asFloat();
		float oldY = state["state"]["xy"][1].asFloat();
		if (!setColorHueSaturation(hue, sat, 1))
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(110));
		if (!alert())
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		if (!on)
		{
			return OffNoRefresh(1);
		}
		else
		{
			return setColorXY(oldX, oldY, 1);
		}
	}
	else if (cType == "ct")
	{
		uint16_t oldCT = state["state"]["ct"].asUInt();
		if (!setColorHueSaturation(hue, sat, 1))
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(110));
		if (!alert())
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		if (!on)
		{
			return OffNoRefresh(1);
		}
		else
		{
			return setColorTemperature(oldCT, 1);
		}
	}
	else
	{
		return false;
	}
}

bool HueExtendedColorLight::alertXY(float x, float y)
{
	refreshState();
	std::string cType = state["state"]["colormode"].asString();
	bool on = state["state"]["on"].asBool();
	if (cType == "hs")
	{
		uint16_t oldHue = state["state"]["hue"].asUInt();
		uint8_t oldSat = state["state"]["sat"].asUInt();
		if (!setColorXY(x, y, 1))
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(110));
		if (!alert())
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		if (!on)
		{
			return OffNoRefresh(1);
		}
		else
		{
			return setColorHueSaturation(oldHue, oldSat, 1);
		}
	}
	else if (cType == "xy")
	{
		float oldX = state["state"]["xy"][0].asFloat();
		float oldY = state["state"]["xy"][1].asFloat();
		if (!setColorXY(x, y, 1))
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(110));
		if (!alert())
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		if (!on)
		{
			return OffNoRefresh(1);
		}
		else
		{
			return setColorXY(oldX, oldY, 1);
		}
	}
	else if (cType == "ct")
	{
		uint16_t oldCT = state["state"]["ct"].asUInt();
		if (!setColorXY(x, y, 1))
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(110));
		if (!alert())
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		if (!on)
		{
			return OffNoRefresh(1);
		}
		else
		{
			return setColorTemperature(oldCT, 1);
		}
	}
	else
	{
		return false;
	}
}

bool HueExtendedColorLight::alertRGB(uint8_t r, uint8_t g, uint8_t b)
{
	refreshState();
	std::string cType = state["state"]["colormode"].asString();
	bool on = state["state"]["on"].asBool();
	if (cType == "hs")
	{
		uint16_t oldHue = state["state"]["hue"].asUInt();
		uint8_t oldSat = state["state"]["sat"].asUInt();
		if (!setColorRGB(r, g, b, 1))
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(110));
		if (!alert())
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		if (!on)
		{
			return OffNoRefresh(1);
		}
		else
		{
			return setColorHueSaturation(oldHue, oldSat, 1);
		}
	}
	else if (cType == "xy")
	{
		float oldX = state["state"]["xy"][0].asFloat();
		float oldY = state["state"]["xy"][1].asFloat();
		if (!setColorRGB(r, g, b, 1))
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(110));
		if (!alert())
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		if (!on)
		{
			return OffNoRefresh(1);
		}
		else
		{
			return setColorXY(oldX, oldY, 1);
		}
	}
	else if (cType == "ct")
	{
		uint16_t oldCT = state["state"]["ct"].asUInt();
		if (!setColorRGB(r, g, b, 1))
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(110));
		if (!alert())
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		if (!on)
		{
			return OffNoRefresh(1);
		}
		else
		{
			return setColorTemperature(oldCT, 1);
		}
	}
	else
	{
		return false;
	}
}
