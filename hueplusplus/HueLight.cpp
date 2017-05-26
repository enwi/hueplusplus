/**
	\file HueLight.cpp
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

#include "HueLight.h"

#include "HttpHandler.h"
#include "json/json.h"

#include <cmath>
#include <iostream>
#include <thread>

bool HueLight::On(uint8_t transistion)
{
	std::cout << "Turning lamp with id: " << id << " on\n";
	refreshState();
	return OnNoRefresh(transistion);
}

bool HueLight::Off(uint8_t transistion)
{
	std::cout << "Turning lamp with id: " << id << " off\n";
	refreshState();
	return OffNoRefresh(transistion);
}

bool HueLight::alert()
{
	std::cout << "alert()\n";

	Json::Value request;
	request["alert"] = "select";

	Json::Value reply = SendPutRequest(request);

	if (reply[0]["success"]["/lights/" + std::to_string(id) + "/state/alert"].asString() == "select")
	{
		return true;
	}

	return false;
}

std::string HueLight::getName()
{
	refreshState();
	return state["name"].asString();
}

unsigned int HueLight::KelvinToMired(unsigned int kelvin)
{
	return int(0.5f + (1000000 / kelvin));
}

unsigned int HueLight::MiredToKelvin(unsigned int mired)
{
	return int(0.5f + (1000000 / mired));
}


HueLight::HueLight(const std::string& ip, const std::string& username, int id) : ip(ip), username(username), id(id)
{
	refreshState();
}

bool HueLight::OnNoRefresh(uint8_t transistion)
{
	std::cout << "\tOnNoRefresh()\n";
	Json::Value request(Json::objectValue);
	if (transistion != 4)
	{
		request["transistiontime"] = transistion;
	}
	if (state["state"]["on"].asBool() != true)
	{
		request["on"] = true;
	}

	if (!request.isMember("on") && !request.isMember("transistiontime"))
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
	}
	return success;
}

bool HueLight::OffNoRefresh(uint8_t transistion)
{
	std::cout << "\tOffNoRefresh()\n";
	Json::Value request(Json::objectValue);
	if (transistion != 4)
	{
		request["transistiontime"] = transistion;
	}
	if (state["state"]["on"].asBool() != false)
	{
		request["on"] = false;
	}

	if (!request.isMember("on") && !request.isMember("transistiontime"))
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
	}
	return success;
}

Json::Value HueLight::SendPutRequest(const Json::Value& request)
{
	std::string put;
	put.append("PUT /api/");
	put.append(username);
	put.append("/lights/");
	put.append(std::to_string(id));
	put.append("/state HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: ");
	put.append(std::to_string(request.toStyledString().size()));
	put.append("\r\n\r\n");
	put.append(request.toStyledString());
	put.append("\r\n\r\n");

	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;
	std::unique_ptr<Json::CharReader> reader = std::unique_ptr<Json::CharReader>(builder.newCharReader());
	std::string error;
	std::string putAnswer = HttpHandler().sendRequestGetBody(put.c_str(), ip, 80);

	Json::Value result;
	if (!reader->parse(putAnswer.c_str(), putAnswer.c_str() + putAnswer.length(), &result, &error))
	{
		std::cout << "Error while parsing JSON in function SendRequest() of HueLight: " << error << std::endl;
		throw(std::runtime_error("Error while parsing JSON in function SendRequest() of HueLight"));
	}
	return result;
}

void HueLight::refreshState()
{
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	std::cout << "\tRefreshing lampstate of lamp with id: " << id << ", ip: " << ip << "\n";
	// GET /api/<username>/lights/<id> HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: <length>\r\n\r\n<content>\r\n\r\n
	std::string get;
	get.append("GET /api/");
	get.append(username);
	get.append("/lights/");
	get.append(std::to_string(id));
	get.append(" HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: 0\r\n\r\n\r\n\r\n");

	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;
	std::unique_ptr<Json::CharReader> reader = std::unique_ptr<Json::CharReader>(builder.newCharReader());
	std::string error;
	std::string getAnswer = HttpHandler().sendRequestGetBody(get.c_str(), ip, 80);
	// todo check whether getAnswer is containing right information
	if (!reader->parse(getAnswer.c_str(), getAnswer.c_str() + getAnswer.length(), &state, &error))
	{
		std::cout << "Error while parsing JSON in function refreshState() of HueLight: " << error << std::endl;
		throw(std::runtime_error("Error while parsing JSON in function refreshState() of HueLight"));
	}
	std::cout << "\tRefresh state took: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << "ms" << std::endl;
}

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
