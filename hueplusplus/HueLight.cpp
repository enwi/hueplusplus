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

#include "include/HueLight.h"


#include "include/HttpHandler.h"
#include "include/json/json.h"

#include <cmath>
#include <iostream>
#include <thread>

bool HueLight::On(uint8_t transition)
{
	std::cout << "Turning lamp with id: " << id << " on\n";
	refreshState();
	return OnNoRefresh(transition);
}

bool HueLight::Off(uint8_t transition)
{
	std::cout << "Turning lamp with id: " << id << " off\n";
	refreshState();
	return OffNoRefresh(transition);
}

std::string HueLight::getName()
{
	refreshState();
	return state["name"].asString();
}

ColorType HueLight::getColorType()
{
	return colorType;
}

unsigned int HueLight::KelvinToMired(unsigned int kelvin)
{
	return int(0.5f + (1000000 / kelvin));
}

unsigned int HueLight::MiredToKelvin(unsigned int mired)
{
	return int(0.5f + (1000000 / mired));
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

HueLight::HueLight(const std::string& ip, const std::string& username, int id) : 
ip(ip), 
username(username), 
id(id),
_brightnessStrategy(nullptr),
_colorTemperatureStrategy(nullptr),
_colorHueStategy(nullptr)
{
	refreshState();
}

bool HueLight::OnNoRefresh(uint8_t transition)
{
	std::cout << "\tOnNoRefresh()\n";
	Json::Value request(Json::objectValue);
	if (transition != 4)
	{
		request["transitiontime"] = transition;
	}
	if (state["state"]["on"].asBool() != true)
	{
		request["on"] = true;
	}

	if (!request.isMember("on") && !request.isMember("transitiontime"))
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
	}
	return success;
}

bool HueLight::OffNoRefresh(uint8_t transition)
{
	std::cout << "\tOffNoRefresh()\n";
	Json::Value request(Json::objectValue);
	if (transition != 4)
	{
		request["transitiontime"] = transition;
	}
	if (state["state"]["on"].asBool() != false)
	{
		request["on"] = false;
	}

	if (!request.isMember("on") && !request.isMember("transitiontime"))
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
