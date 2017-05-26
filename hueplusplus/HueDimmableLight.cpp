/**
	\file HueDimmableLight.cpp
	Copyright Notice\n
	Copyright (C) 2017  Jan Rogall		- developer\n
	Copyright (C) 2017  Moritz Wirger	- developer\n

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free SofHueDimmableLighttware Foundation; either version 3 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software Foundation,
	Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
**/

#include "include/HueDimmableLight.h"

#include <cmath>
#include <iostream>
#include <thread>

bool HueDimmableLight::setBrightness(unsigned int bri, uint8_t transistion)
{
	std::cout << "Setting lamp with id: " << id << " to brightness of " << bri << std::endl;
	refreshState();
	if (bri == 0)
	{
		if (state["state"]["on"] == true)
		{
			return OffNoRefresh(transistion);
		}
		else
		{
			return true;
		}
	}
	else
	{
		Json::Value request(Json::objectValue);
		if (transistion != 4)
		{
			request["transistiontime"] = transistion;
		}
		if (state["state"]["on"].asBool() != true)
		{
			request["on"] = true;
		}
		if (state["state"]["bri"].asUInt() != bri)
		{
			bri -= 1;
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
		if (success && request.isMember("bri"))
		{
			//Check if success was sent and the value was changed
			success = !reply[i].isNull() && reply[i].isMember("success") && reply[i]["success"][path + "bri"].asUInt() == request["bri"].asUInt();
		}
		return success;
	}
}
