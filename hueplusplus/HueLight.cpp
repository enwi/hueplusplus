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

#include <cmath>
#include <iostream>
#include <thread>

#include "include/json/json.h"

bool HueLight::On(uint8_t transition)
{
    refreshState();
    return OnNoRefresh(transition);
}

bool HueLight::Off(uint8_t transition)
{
    refreshState();
    return OffNoRefresh(transition);
}

bool HueLight::isOn()
{
    refreshState();
    return state["state"]["on"].asBool();
}

bool HueLight::isOn() const
{
    return state["state"]["on"].asBool();
}

int HueLight::getId() const
{
    return id;
}

std::string HueLight::getType() const
{
    return state["type"].asString();
}

std::string HueLight::getName()
{
    refreshState();
    return state["name"].asString();
}

std::string HueLight::getName() const
{
    return state["name"].asString();
}

std::string HueLight::getModelId() const
{
    return state["modelid"].asString();
}

std::string HueLight::getUId() const
{
    if (state.isMember("uniqueid"))
    {
        return state["uniqueid"].asString();
    }
    return std::string();
}

std::string HueLight::getManufacturername() const
{
    if (state.isMember("manufacturername"))
    {
        return state["manufacturername"].asString();
    }
    return std::string();
}

std::string HueLight::getProductname() const
{
    if (state.isMember("productname"))
    {
        return state["productname"].asString();
    }
    return std::string();
}

std::string HueLight::getLuminaireUId() const
{
    if (state.isMember("luminaireuniqueid"))
    {
        return state["luminaireuniqueid"].asString();
    }
    return std::string();
}

std::string HueLight::getSwVersion()
{
    refreshState();
    return state["swversion"].asString();
}

std::string HueLight::getSwVersion() const
{
    return state["swversion"].asString();
}

bool HueLight::setName(const std::string& name)
{
    Json::Value request(Json::objectValue);
    request["name"] = name;
    Json::Value reply = SendPutRequest(request, "/name");

    //Check whether request was successful
    return !reply[0].isNull() && reply[0].isMember("success") && reply[0]["success"]["/lights/" + std::to_string(id) + "/name"] == name;
}

ColorType HueLight::getColorType() const
{
    return colorType;
}

unsigned int HueLight::KelvinToMired(unsigned int kelvin) const
{
    return int(0.5f + (1000000 / kelvin));
}

unsigned int HueLight::MiredToKelvin(unsigned int mired) const
{
    return int(0.5f + (1000000 / mired));
}

bool HueLight::alert()
{
    Json::Value request;
    request["alert"] = "select";

    Json::Value reply = SendPutRequest(request, "/state");

    if (reply[0]["success"]["/lights/" + std::to_string(id) + "/state/alert"].asString() == "select")
    {
        return true;
    }

    return false;
}

HueLight::HueLight(int id, const HueCommandAPI& commands)
    : HueLight(id, commands, nullptr, nullptr, nullptr)
{}

HueLight::HueLight(int id, const HueCommandAPI& commands, std::shared_ptr<const BrightnessStrategy> brightnessStrategy, std::shared_ptr<const ColorTemperatureStrategy> colorTempStrategy, std::shared_ptr<const ColorHueStrategy> colorHueStrategy)
    : id(id),
    brightnessStrategy(std::move(brightnessStrategy)),
    colorTemperatureStrategy(std::move(colorTempStrategy)),
    colorHueStrategy(std::move(colorHueStrategy)),
    commands(commands)

{
    refreshState();
}

bool HueLight::OnNoRefresh(uint8_t transition)
{
    Json::Value request(Json::objectValue);
    if (transition != 4)
    {
        request["transitiontime"] = transition;
    }
    if (state["state"]["on"].asBool() != true)
    {
        request["on"] = true;
    }

    if (!request.isMember("on"))
    {
        //Nothing needs to be changed
        return true;
    }

    Json::Value reply = SendPutRequest(request, "/state");

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
    Json::Value request(Json::objectValue);
    if (transition != 4)
    {
        request["transitiontime"] = transition;
    }
    if (state["state"]["on"].asBool() != false)
    {
        request["on"] = false;
    }

    if (!request.isMember("on"))
    {
        //Nothing needs to be changed
        return true;
    }

    Json::Value reply = SendPutRequest(request, "/state");

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

Json::Value HueLight::SendPutRequest(const Json::Value& request, const std::string& subPath)
{
    return commands.PUTRequest("/lights/" + std::to_string(id) + subPath, request);
}

void HueLight::refreshState()
{
    //std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    //std::cout << "\tRefreshing lampstate of lamp with id: " << id << ", ip: " << ip << "\n";
    Json::Value answer = commands.GETRequest("/lights/" + std::to_string(id), Json::objectValue);
    if (answer.isObject() && answer.isMember("state"))
    {
        state = answer;
    }
    else
    {
        std::cout << "Answer in HueLight::refreshState of http_handler->GETJson(...) is not expected!\nAnswer:\n\t" << answer.toStyledString() << std::endl;
    }
    //std::cout << "\tRefresh state took: " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() << "ms" << std::endl;
}
