/**
	\file Hue.cpp
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

#include "include/Hue.h"
#include "include/HueLight.h"
#include "include/SimpleBrightnessStrategy.h"
#include "include/SimpleColorHueStrategy.h"
#include "include/ExtendedColorHueStrategy.h"
#include "include/SimpleColorTemperatureStrategy.h"
#include "include/ExtendedColorTemperatureStrategy.h"

#include "include/HttpHandler.h"
#include "include/UPnP.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <regex>
#include <stdexcept>
#include <thread>


std::vector<HueFinder::HueIdentification> HueFinder::FindBridges() const
{
	UPnP uplug;
	std::vector<std::pair<std::string, std::string>> foundDevices = uplug.getDevices();

	//Does not work
	std::regex manufRegex("<manufacturer>Royal Philips Electronics</manufacturer>");
	std::regex manURLRegex("<manufacturerURL>http://www\\.philips\\.com</manufacturerURL>");
	std::regex modelRegex("<modelName>Philips hue bridge[^<]*</modelName>");
	std::regex serialRegex("<serialNumber>(\\w+)</serialNumber>");
	std::vector<HueIdentification> foundBridges;
	for (const std::pair<std::string, std::string> &p : foundDevices)
	{
		size_t found = p.second.find("IpBridge");
		if (found != std::string::npos)
		{
			HueIdentification bridge;
			unsigned int start = p.first.find("//") + 2;
			unsigned int length = p.first.find(":", start) - start;
			bridge.ip = p.first.substr(start, length);
			std::string desc = HttpHandler().sendRequestGetBody("GET /description.xml HTTP/1.0\r\nContent-Type: application/xml\r\nContent-Length: 0\r\n\r\n\r\n\r\n", bridge.ip);
			std::smatch matchResult;
			if (std::regex_search(desc, manufRegex) && std::regex_search(desc, manURLRegex) && std::regex_search(desc, modelRegex) && std::regex_search(desc, matchResult, serialRegex))
			{
				//The string matches
				//Get 1st submatch (0 is whole match)
				bridge.mac = matchResult[1].str();
				foundBridges.push_back(std::move(bridge));
			}
			//break;
		}
	}
	return foundBridges;
}


Hue HueFinder::GetBridge(const HueIdentification& identification)
{
	std::string username;
	auto pos = usernames.find(identification.mac);
	if (pos != usernames.end())
	{
		username = pos->second;
	}
	else
	{
		username = RequestUsername(identification.ip);
		if (username.empty())
		{
			std::cerr << "Failed to request username for ip " << identification.ip << std::endl;
			throw std::runtime_error("Failed to request username!");
		}
		else
		{
			AddUsername(identification.mac, username);
		}
	}
	return Hue(identification.ip, username);
}

void HueFinder::AddUsername(const std::string& mac, const std::string& username)
{
	usernames[mac] = username;
}

const std::map<std::string, std::string>& HueFinder::GetAllUsernames() const
{
	return usernames;
}

std::string HueFinder::RequestUsername(const std::string & ip) const
{
	std::cout << "Please press the link Button! You've got 35 secs!\n";	// when the link button was presed we got 30 seconds to get our username for control

	Json::Value request;
	request["devicetype"] = "HuePlusPlus#System User";

	// POST /api HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: <length>\r\n\r\n<content>\r\n\r\n

	std::string post;
	post.append("POST /api HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: ");
	post.append(std::to_string(request.toStyledString().size()));
	post.append("\r\n\r\n");
	post.append(request.toStyledString());
	post.append("\r\n\r\n");

	std::cout << post << std::endl;
	std::cout << ip << std::endl;

	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;
	std::unique_ptr<Json::CharReader> reader = std::unique_ptr<Json::CharReader>(builder.newCharReader());
	std::string error;

	Json::Value answer;
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point lastCheck;
	while (std::chrono::steady_clock::now() - start < std::chrono::seconds(35))
	{
		if (std::chrono::steady_clock::now() - lastCheck > std::chrono::seconds(1))
		{
			lastCheck = std::chrono::steady_clock::now();
			std::string postAnswer = HttpHandler().sendRequestGetBody(post.c_str(), ip, 80);
			if (!reader->parse(postAnswer.c_str(), postAnswer.c_str() + postAnswer.length(), &answer, &error))
			{
				std::cout << "Error while parsing JSON in getUsername of Hue: " << error << std::endl;
				throw(std::runtime_error("Error while parsing JSON in getUsername of Hue"));
			}
			if (answer[0]["success"] != Json::nullValue)
			{
				// [{"success":{"username": "83b7780291a6ceffbe0bd049104df"}}]
				std::cout << "Success! Link button was pressed!\n";
				std::cout << "Username is \"" << answer[0]["success"]["username"].asString() << "\"\n";;
				return answer[0]["success"]["username"].asString();
			}
			if (answer[0]["error"] != Json::nullValue)
			{
				std::cout << "Link button not pressed!\n";
			}
		}
	}
	return std::string();
}


Hue::Hue(const std::string& ip, const std::string& username) : 
ip(ip), 
username(username)
{
	_simpleBrightnessStrategy = std::shared_ptr<BrightnessStrategy>( new SimpleBrightnessStrategy );
	_simpleColorHueStrategy = std::shared_ptr<ColorHueStrategy>( new SimpleColorHueStrategy );
	_extendedColorHueStrategy = std::shared_ptr<ColorHueStrategy>( new ExtendedColorHueStrategy );
	_simpleColorTemperatureStrategy = std::shared_ptr<ColorTemperatureStrategy>( new SimpleColorTemperatureStrategy );
	_extendedColorTemperatureStrategy = std::shared_ptr<ColorTemperatureStrategy>( new ExtendedColorTemperatureStrategy );
}

std::string Hue::getBridgeIP()
{
	return ip;
}

void Hue::requestUsername(const std::string& ip)
{
	std::cout << "Please press the link Button! You've got 35 secs!\n";	// when the link button was presed we got 30 seconds to get our username for control
	
	Json::Value request;
	request["devicetype"] = "Enwiro Smarthome#System User";

	// POST /api HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: <length>\r\n\r\n<content>\r\n\r\n

	std::string post;
	post.append("POST /api HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: ");
	post.append(std::to_string(request.toStyledString().size()));
	post.append("\r\n\r\n");
	post.append(request.toStyledString());
	post.append("\r\n\r\n");

	std::cout << post << std::endl;
	std::cout << ip << std::endl;

	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;
	std::unique_ptr<Json::CharReader> reader = std::unique_ptr<Json::CharReader>(builder.newCharReader());
	std::string error;

	Json::Value answer;
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	std::chrono::steady_clock::time_point lastCheck;
	while (std::chrono::steady_clock::now() - start < std::chrono::seconds(35))
	{
		if (std::chrono::steady_clock::now() - lastCheck > std::chrono::seconds(1))
		{
			lastCheck = std::chrono::steady_clock::now();
			std::string postAnswer = HttpHandler().sendRequestGetBody(post.c_str(), ip, 80);
			if (!reader->parse(postAnswer.c_str(), postAnswer.c_str() + postAnswer.length(), &answer, &error))
			{
				std::cout << "Error while parsing JSON in getUsername of Hue: " << error << std::endl;
				throw(std::runtime_error("Error while parsing JSON in getUsername of Hue"));
			}
			if (answer[0]["success"] != Json::nullValue)
			{
				// [{"success":{"username": "<username>"}}]
				username = answer[0]["success"]["username"].asString();
				this->ip = ip;
				std::cout << "Success! Link button was pressed!\n";
				break;
			}
			if (answer[0]["error"] != Json::nullValue)
			{
				std::cout << "Link button not pressed!\n";
			}
		}
	}
	return;
}

std::string Hue::getUsername()
{
	return username;
}

void Hue::setIP(const std::string ip)
{
	this->ip = ip;
}

const HueLight& Hue::getLight(int id)
{
	if(lights.count(id) > 0)
	{
		return lights.find(id)->second;
	}
	refreshState();
	if (state["lights"][std::to_string(id)] == Json::nullValue)
	{
		std::cout << "Error in Hue getLight(): light with id " << id << " is not valid\n";
		throw(std::runtime_error("Error in Hue getLight(): light id is not valid"));
	}
	//std::cout << state["lights"][std::to_string(id)] << std::endl;
	std::string type = state["lights"][std::to_string(id)]["modelid"].asString();
	//std::cout << type << std::endl;
	if (type == "LCT001" || type == "LCT002" || type == "LCT003" || type == "LCT007" || type == "LLM001")
	{
		// HueExtendedColorLight Gamut B
		HueLight light = HueLight(ip, username, id);
		light.setBrightnessStrategy(_simpleBrightnessStrategy);
		light.setColorHueStrategy(_extendedColorHueStrategy);
		light.setColorTemperatureStrategy(_extendedColorTemperatureStrategy);
		light.colorType = ColorType::GAMUT_B;
		lights.emplace(id, light);
		return lights.find(id)->second;
	}
	else if (type == "LCT010" || type == "LCT011" || type == "LCT014" || type == "LLC020" || type == "LST002")
	{
		// HueExtendedColorLight Gamut C
		HueLight light = HueLight(ip, username, id);
		light.setBrightnessStrategy(_simpleBrightnessStrategy);
		light.setColorHueStrategy(_extendedColorHueStrategy);
		light.setColorTemperatureStrategy(_extendedColorTemperatureStrategy);
		light.colorType = ColorType::GAMUT_C;
		lights.emplace(id, light);
		return lights.find(id)->second;
	}
	else if (type == "LST001" || type == "LLC006" || type == "LLC007" || type == "LLC010" || type == "LLC011" || type == "LLC012" || type == "LLC013")
	{
		// HueColorLight Gamut A
		HueLight light = HueLight(ip, username, id);
		light.setBrightnessStrategy(_simpleBrightnessStrategy);
		light.setColorHueStrategy(_simpleColorHueStrategy);
		light.setColorTemperatureStrategy(_simpleColorTemperatureStrategy);
		light.colorType = ColorType::GAMUT_A;
		lights.emplace(id, light);
		return lights.find(id)->second;
	}
	else if (type == "LWB004" || type == "LWB006" || type == "LWB007" || type == "LWB010" || type == "LWB014")
	{
		// HueDimmableLight No Color Type
		HueLight light = HueLight(ip, username, id);
		light.setBrightnessStrategy(_simpleBrightnessStrategy);
		//light.setColorHueStrategy();
		//light.setColorTemperatureStrategy();
		light.colorType = ColorType::NONE;
		lights.emplace(id, light);
		return lights.find(id)->second;
	}
	else if (type == "LLM010" || type == "LLM011" || type == "LLM012" || type == "LTW001" || type == "LTW004" || type == "LTW013" || type == "LTW014")
	{
		// HueTemperatureLight
		HueLight light = HueLight(ip, username, id);
		light.setBrightnessStrategy(_simpleBrightnessStrategy);
		//light.setColorHueStrategy();
		light.setColorTemperatureStrategy(_simpleColorTemperatureStrategy);
		light.colorType = ColorType::TEMPERATURE;
		lights.emplace(id, light);
		return lights.find(id)->second;
	}
	std::cout << "Could not determine HueLight type!\n";
	throw(std::runtime_error("Could not determine HueLight type!"));
}

/*const std::map<uint8_t, ColorType>& Hue::getAllLightTypes()
{
	refreshState();
	for (const auto& name : state["lights"].getMemberNames())
	{
		std::string type = state["lights"][name]["modelid"].asString();
		int id = std::stoi(name);

		if (type == "LCT001" || type == "LCT002" || type == "LCT003" || type == "LCT007" || type == "LLM001")
		{
			// HueExtendedColorLight Gamut B
			lights[id].second = ColorType::GAMUT_B;
		}
		else if (type == "LCT010" || type == "LCT011" || type == "LCT014" || type == "LLC020" || type == "LST002")
		{
			// HueExtendedColorLight Gamut C
			lights[id].second = ColorType::GAMUT_C;
		}
		else if (type == "LST001" || type == "LLC006" || type == "LLC007" || type == "LLC010" || type == "LLC011" || type == "LLC012" || type == "LLC013")
		{
			// HueColorLight Gamut A
			lights[id].second = ColorType::GAMUT_A;
		}
		else if (type == "LWB004" || type == "LWB006" || type == "LWB007" || type == "LWB010" || type == "LWB014")
		{
			// HueDimmableLight No Color Type
			lights[id].second = ColorType::NONE;
		}
		else if (type == "LLM010" || type == "LLM011" || type == "LLM012" || type == "LTW001" || type == "LTW004" || type == "LTW013" || type == "LTW014")
		{
			// HueTemperatureLight
			lights[id].second = ColorType::TEMPERATURE;
		}
	}
	return lights;
}*/


std::vector<std::reference_wrapper<const HueLight>> Hue::getAllLights()
{
	refreshState();
	for (const auto& name : state["lights"].getMemberNames())
	{
		uint8_t id = std::stoi(name);
		if(lights.count(id)<=0)
		{
			getLight(id);
		}
	}
	std::vector<std::reference_wrapper<const HueLight>> result;
	for (const auto& entry : lights)
	{
		result.emplace_back(entry.second);
	}
	return result;
}

void Hue::refreshState()
{
	if (username.empty())
	{
		return;
	}
	std::string get;
	get.append("GET /api/");
	get.append(username);
	get.append(" HTTP / 1.0\r\nContent - Type: application / json\r\nContent - Length: 0\r\n\r\n\r\n\r\n");

	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;
	std::unique_ptr<Json::CharReader> reader = std::unique_ptr<Json::CharReader>(builder.newCharReader());
	std::string error;

	Json::Value answer;
	std::string postAnswer = HttpHandler().sendRequestGetBody(get.c_str(), ip, 80);
	//std::cout <<"\""<< postAnswer << "\"\n" << std::endl;
	if (!reader->parse(postAnswer.c_str(), postAnswer.c_str() + postAnswer.length(), &answer, &error))
	{
		std::cout << "Error while parsing JSON in refreshState of Hue: " << error << std::endl;
		throw(std::runtime_error("Error while parsing JSON in refreshState of Hue"));
	}
	if (answer.isMember("lights"))
	{
		state = answer;
	}
	else
	{
		std::cout << "Answer in refreshState() of HttpHandler().sendRequestGetBody() is not expected!\n";
	}
}
