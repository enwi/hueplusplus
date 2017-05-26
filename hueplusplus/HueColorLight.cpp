/**
	\file HueColorLight.cpp
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

#include "include/HueColorLight.h"

#include <cmath>
#include <iostream>
#include <thread>

bool HueColorLight::setColorHue(uint16_t hue, uint8_t transistion)
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
	if (state["state"]["hue"].asUInt() != hue || state["state"]["colormode"].asString() != "hs")
	{
		hue = hue % 65535;
		request["hue"] = hue;
	}

	if (!request.isMember("on") && !request.isMember("hue"))
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
	if (success && request.isMember("hue"))
	{
		//Check if success was sent and the value was changed
		success = !reply[i].isNull() && reply[i].isMember("success") && reply[i]["success"][path + "hue"].asUInt() == request["hue"].asUInt();
	}
	return success;
}

bool HueColorLight::setColorSaturation(uint8_t sat, uint8_t transistion)
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
	if (state["state"]["sat"].asUInt() != sat)
	{
		if (sat > 254)
		{
			sat = 254;
		}
		request["sat"] = sat;
	}

	if (!request.isMember("on") && !request.isMember("sat"))
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
	if (success && request.isMember("sat"))
	{
		//Check if success was sent and the value was changed
		success = !reply[i].isNull() && reply[i].isMember("success") && reply[i]["success"][path + "sat"].asUInt() == request["sat"].asUInt();
	}
	return success;
}

bool HueColorLight::setColorHueSaturation(uint16_t hue, uint8_t sat, uint8_t transistion)
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
	if (state["state"]["hue"].asUInt() != hue || state["state"]["colormode"].asString() != "hs")
	{
		hue = hue % 65535;
		request["hue"] = hue;
	}
	if (state["state"]["sat"].asUInt() != sat || state["state"]["colormode"].asString() != "hs")
	{
		if (sat > 254)
		{
			sat = 254;
		}
		request["sat"] = sat;
	}

	if (!request.isMember("on") && !request.isMember("hue") && !request.isMember("sat"))
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
	if (success && request.isMember("hue"))
	{
		//Check if success was sent and the value was changed
		success = !reply[i].isNull() && reply[i].isMember("success") && reply[i]["success"][path + "hue"].asUInt() == request["hue"].asUInt();
		++i;
	}
	if (success && request.isMember("sat"))
	{
		//Check if success was sent and the value was changed
		success = !reply[i].isNull() && reply[i].isMember("success") && reply[i]["success"][path + "sat"].asUInt() == request["sat"].asUInt();
	}
	return success;
}

bool HueColorLight::setColorXY(float x, float y, uint8_t transistion)
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
	if (state["state"]["xy"][0].asFloat() != x || state["state"]["xy"][1].asFloat() != y || state["state"]["colormode"].asString() != "xy")
	{
		request["xy"][0] = x;
		request["xy"][1] = y;
	}

	if (!request.isMember("on") && !request.isMember("xy"))
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
	if (success && request.isMember("xy"))
	{
		//Check if success was sent and the value was changed
		success = !reply[i].isNull() && reply[i].isMember("success") && reply[i]["success"][path + "xy"][0].asFloat() == request["xy"][0].asFloat();
		if (success)
		{
			success = !reply[i].isNull() && reply[i].isMember("success") && reply[i]["success"][path + "xy"][1].asFloat() == request["xy"][1].asFloat();
		}
	}
	return success;
}

bool HueColorLight::setColorRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t transistion)
{
	float red = r / 255;
	float green = g / 255;
	float blue = b / 255;

	// gamma correction
	red = (red > 0.04045f) ? pow((red + 0.055f) / (1.0f + 0.055f), 2.4f) : (red / 12.92f);
	green = (green > 0.04045f) ? pow((green + 0.055f) / (1.0f + 0.055f), 2.4f) : (green / 12.92f);
	blue = (blue > 0.04045f) ? pow((blue + 0.055f) / (1.0f + 0.055f), 2.4f) : (blue / 12.92f);

	float X = red * 0.664511f + green * 0.154324f + blue * 0.162028f;
	float Y = red * 0.283881f + green * 0.668433f + blue * 0.047685f;
	float Z = red * 0.000088f + green * 0.072310f + blue * 0.986039f;

	float x = X / (X + Y + Z);
	float y = Y / (X + Y + Z);

	return setColorXY(x, y, transistion);
}

bool HueColorLight::setColorLoop(bool on)
{
	//colorloop
	refreshState();
	Json::Value request(Json::objectValue);

	if (state["state"]["on"].asBool() != true)
	{
		request["on"] = true;
	}
	std::string effect;
	if ((effect = on ? "colorloop" : "none") != state["state"]["effect"].asString())
	{
		request["effect"] = effect;
	}
	if (!request.isMember("on") && !request.isMember("effect"))
	{
		//Nothing needs to be changed
		return true;
	}

	Json::Value reply = SendPutRequest(request);

	//Check whether request was successful
	std::string path = "/lights/" + std::to_string(id) + "/state/";
	bool success = true;
	int i = 0;
	if (success && request.isMember("on"))
	{
		//Check if success was sent and the value was changed
		success = !reply[i].isNull() && reply[i].isMember("success") && reply[i]["success"][path + "on"].asBool() == request["on"].asBool();
		++i;
	}
	if (success && request.isMember("effect"))
	{
		//Check if success was sent and the value was changed
		success = !reply[i].isNull() && reply[i].isMember("success") && reply[i]["success"][path + "effect"].asString() == request["effect"].asString();
	}
	return success;
}

bool HueColorLight::alertHueSaturation(uint16_t hue, uint8_t sat)
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
	else
	{
		return false;
	}
}

bool HueColorLight::alertXY(float x, float y)
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
	else
	{
		return false;
	}
}

bool HueColorLight::alertRGB(uint8_t r, uint8_t g, uint8_t b)
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
	else
	{
		return false;
	}
}

/*bool HueColorLight::pointInTriangle(float pointx, float pointy, float x0, float y0, float x1, float y1, float x2, float y2)
{
float A = (-y1 * x2 + y0*(-x1 + x2) + x0*(y1 - y2) + x1 * y1);
int8_t sign = A < 0 ? -1 : 1;
float s = (y0 * x2 - x0 * y2 + (y2 - y0) * pointx + (x0 - x2) * pointy) * sign;
float t = (x0 * y1 - y0 * x1 + (y0 - y1) * pointx + (x1 - x0) * pointy) * sign;

return s > 0 && t > 0 && (s + t) < A * sign;
}*/
