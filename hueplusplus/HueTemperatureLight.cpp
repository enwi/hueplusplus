/**
	\file HueTemperatureLight.cpp
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

#include "include/HueTemperatureLight.h"

#include <cmath>
#include <iostream>
#include <thread>


bool HueTemperatureLight::setColorTemperature(unsigned int mired, uint8_t transistion)
{
	refreshState();
	Json::Value request(Json::objectValue);
	if (transistion != 4)
	{
		request["transistiontime"] = transistion;
	}
	if (state["state"]["on"].asBool() != true)
	{
		request["on"] = true;
	}
	if (state["state"]["ct"].asUInt() != mired)
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
	if (success && request.isMember("transistiontime"))
	{
		//Check if success was sent and the value was changed
		success = !reply[i].isNull() && reply[i].isMember("success") && reply[i]["success"][path + "transistiontime"].asUInt() == request["transistiontime"].asUInt();
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

bool HueTemperatureLight::alertTemperature(unsigned int mired)
{
	refreshState();
	std::string cType = state["state"]["colormode"].asString();
	bool on = state["state"]["on"].asBool();
	if (cType == "ct")
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

