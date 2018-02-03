/**
	\file SimpleColorHueStrategy.cpp
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

#include "include/SimpleColorHueStrategy.h"
#include "include/HueConfig.h"

#include <cmath>
#include <iostream>
#include <thread>

bool SimpleColorHueStrategy::setColorHue(uint16_t hue, uint8_t transition, HueLight& light) const
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
	if (light.state["state"]["hue"].asUInt() != hue || light.state["state"]["colormode"].asString() != "hs")
	{
		hue = hue % 65535;
		request["hue"] = hue;
	}

	if (!request.isMember("on") && !request.isMember("hue"))
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
	if (success && request.isMember("hue"))
	{
		//Check if success was sent and the value was changed
		success = !reply[i].isNull() && reply[i].isMember("success") && reply[i]["success"][path + "hue"].asUInt() == request["hue"].asUInt();
	}
	return success;
}

bool SimpleColorHueStrategy::setColorSaturation(uint8_t sat, uint8_t transition, HueLight& light) const
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
	if (light.state["state"]["sat"].asUInt() != sat)
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
	if (success && request.isMember("sat"))
	{
		//Check if success was sent and the value was changed
		success = !reply[i].isNull() && reply[i].isMember("success") && reply[i]["success"][path + "sat"].asUInt() == request["sat"].asUInt();
	}
	return success;
}

bool SimpleColorHueStrategy::setColorHueSaturation(uint16_t hue, uint8_t sat, uint8_t transition, HueLight& light) const
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
	if (light.state["state"]["hue"].asUInt() != hue || light.state["state"]["colormode"].asString() != "hs")
	{
		hue = hue % 65535;
		request["hue"] = hue;
	}
	if (light.state["state"]["sat"].asUInt() != sat || light.state["state"]["colormode"].asString() != "hs")
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

bool SimpleColorHueStrategy::setColorXY(float x, float y, uint8_t transition, HueLight& light) const
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
	if (light.state["state"]["xy"][0].asFloat() != x || light.state["state"]["xy"][1].asFloat() != y || light.state["state"]["colormode"].asString() != "xy")
	{
		request["xy"][0] = x;
		request["xy"][1] = y;
	}

	if (!request.isMember("on") && !request.isMember("xy"))
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
	if (success && request.isMember("xy"))
	{
		//Check if success was sent and the value was changed
		success = !reply[i].isNull() && reply[i].isMember("success") && static_cast<int>(reply[i]["success"][path + "xy"][0].asFloat() * 10000 + 0.5) == static_cast<int>(request["xy"][0].asFloat() * 10000 + 0.5);
		if (success)
		{
			success = !reply[i].isNull() && reply[i].isMember("success") && static_cast<int>(reply[i]["success"][path + "xy"][1].asFloat() * 10000 + 0.5) == static_cast<int>(request["xy"][1].asFloat() * 10000 + 0.5);
		}
	}
	return success;
}

bool SimpleColorHueStrategy::setColorRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t transition, HueLight& light) const
{
	if( (r == 0) && (g == 0) && (b == 0) )
	{
		return light.OffNoRefresh();
	}

	const float red = float(r) / 255;
	const float green = float(g) / 255;
	const float blue = float(b) / 255;

	// gamma correction
	const float redCorrected = (red > 0.04045f) ? pow((red + 0.055f) / (1.0f + 0.055f), 2.4f) : (red / 12.92f);
	const float greenCorrected = (green > 0.04045f) ? pow((green + 0.055f) / (1.0f + 0.055f), 2.4f) : (green / 12.92f);
	const float blueCorrected = (blue > 0.04045f) ? pow((blue + 0.055f) / (1.0f + 0.055f), 2.4f) : (blue / 12.92f);

	const float X = redCorrected * 0.664511f + greenCorrected * 0.154324f + blueCorrected * 0.162028f;
	const float Y = redCorrected * 0.283881f + greenCorrected * 0.668433f + blueCorrected * 0.047685f;
	const float Z = redCorrected * 0.000088f + greenCorrected * 0.072310f + blueCorrected * 0.986039f;

	const float x = X / (X + Y + Z);
	const float y = Y / (X + Y + Z);

	return light.setColorXY(x, y, transition);
}

bool SimpleColorHueStrategy::setColorLoop(bool on, HueLight& light) const
{
	//colorloop
	light.refreshState();
	Json::Value request(Json::objectValue);

	if (light.state["state"]["on"].asBool() != true)
	{
		request["on"] = true;
	}
	std::string effect;
	if ((effect = on ? "colorloop" : "none") != light.state["state"]["effect"].asString())
	{
		request["effect"] = effect;
	}
	if (!request.isMember("on") && !request.isMember("effect"))
	{
		//Nothing needs to be changed
		return true;
	}

	Json::Value reply = light.SendPutRequest(request, "/state");

	//Check whether request was successful
	std::string path = "/lights/" + std::to_string(light.id) + "/state/";
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

bool SimpleColorHueStrategy::alertHueSaturation(uint16_t hue, uint8_t sat, HueLight& light) const
{
	light.refreshState();
	std::string cType = light.state["state"]["colormode"].asString();
	bool on = light.state["state"]["on"].asBool();
	if (cType == "hs")
	{
		uint16_t oldHue = light.state["state"]["hue"].asUInt();
		uint8_t oldSat = light.state["state"]["sat"].asUInt();
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
	else
	{
		return false;
	}
}

bool SimpleColorHueStrategy::alertXY(float x, float y, HueLight& light) const
{
	light.refreshState();
	std::string cType = light.state["state"]["colormode"].asString();
	bool on = light.state["state"]["on"].asBool();
	if (cType == "hs")
	{
		uint16_t oldHue = light.state["state"]["hue"].asUInt();
		uint8_t oldSat = light.state["state"]["sat"].asUInt();
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
	else
	{
		return false;
	}
}

bool SimpleColorHueStrategy::alertRGB(uint8_t r, uint8_t g, uint8_t b, HueLight& light) const
{
	light.refreshState();
	std::string cType = light.state["state"]["colormode"].asString();
	bool on = light.state["state"]["on"].asBool();
	if (cType == "hs")
	{
		uint16_t oldHue = light.state["state"]["hue"].asUInt();
		uint8_t oldSat = light.state["state"]["sat"].asUInt();
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
	else
	{
		return false;
	}
}

std::pair<uint16_t, uint8_t> SimpleColorHueStrategy::getColorHueSaturation(HueLight & light) const
{
	light.refreshState();
	return std::pair<uint16_t, uint8_t>(static_cast<uint16_t>(light.state["state"]["hue"].asUInt()), static_cast<uint8_t>(light.state["state"]["sat"].asUInt()));
}

std::pair<uint16_t, uint8_t> SimpleColorHueStrategy::getColorHueSaturation(const HueLight & light) const
{
	return std::pair<uint16_t, uint8_t>(static_cast<uint16_t>(light.state["state"]["hue"].asUInt()), static_cast<uint8_t>(light.state["state"]["sat"].asUInt()));
}

std::pair<float, float> SimpleColorHueStrategy::getColorXY(HueLight & light) const
{
	light.refreshState();
	return std::pair<float, float>(light.state["state"]["xy"][0].asFloat(), light.state["state"]["xy"][1].asFloat());
}

std::pair<float, float> SimpleColorHueStrategy::getColorXY(const HueLight & light) const
{
	return std::pair<float, float>(light.state["state"]["xy"][0].asFloat(), light.state["state"]["xy"][1].asFloat());
}
/*bool SimpleColorHueStrategy::pointInTriangle(float pointx, float pointy, float x0, float y0, float x1, float y1, float x2, float y2)
{
float A = (-y1 * x2 + y0*(-x1 + x2) + x0*(y1 - y2) + x1 * y1);
int8_t sign = A < 0 ? -1 : 1;
float s = (y0 * x2 - x0 * y2 + (y2 - y0) * pointx + (x0 - x2) * pointy) * sign;
float t = (x0 * y1 - y0 * x1 + (y0 - y1) * pointx + (x1 - x0) * pointy) * sign;

return s > 0 && t > 0 && (s + t) < A * sign;
}*/
