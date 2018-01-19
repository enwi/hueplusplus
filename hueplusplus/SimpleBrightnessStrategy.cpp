/**
	\file SimpleBrightnessStrategy.cpp
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

#include "include/SimpleBrightnessStrategy.h"

#include <cmath>
#include <iostream>
#include <thread>

bool SimpleBrightnessStrategy::setBrightness(unsigned int bri, uint8_t transition, HueLight& light) const
{
	light.refreshState();
	if (bri == 0)
	{
		if (light.state["state"]["on"] == true)
		{
			return light.OffNoRefresh(transition);
		}
		else
		{
			return true;
		}
	}
	else
	{
		Json::Value request(Json::objectValue);
		if (transition != 4)
		{
			request["transitiontime"] = transition;
		}
		if (light.state["state"]["on"].asBool() != true)
		{
			request["on"] = true;
		}
		if (light.state["state"]["bri"].asUInt() != bri)
		{
			if (bri > 254)
			{
				bri = 254;
			}
			request["bri"] = bri;
		}

		if (!request.isMember("on") && !request.isMember("bri"))
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
		if (success && request.isMember("bri"))
		{
			//Check if success was sent and the value was changed
			success = !reply[i].isNull() && reply[i].isMember("success") && reply[i]["success"][path + "bri"].asUInt() == request["bri"].asUInt();
		}
		return success;
	}
}

unsigned int SimpleBrightnessStrategy::getBrightness(HueLight& light) const
{
	light.refreshState();
	return light.state["state"]["bri"].asUInt();
}

unsigned int SimpleBrightnessStrategy::getBrightness(const HueLight& light) const
{
	return light.state["state"]["bri"].asUInt();
}
