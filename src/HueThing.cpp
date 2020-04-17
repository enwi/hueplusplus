/**
    \file HueThing.cpp
    Copyright Notice\n
    Copyright (C) 2020  Stefan Herbrechtsmeier - developer\n

    This file is part of hueplusplus.

    hueplusplus is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    hueplusplus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with hueplusplus.  If not, see <http://www.gnu.org/licenses/>.
**/

#include "hueplusplus/HueThing.h"

#include <cmath>
#include <iostream>
#include <thread>

#include "hueplusplus/HueExceptionMacro.h"
#include "hueplusplus/Utils.h"
#include "json/json.hpp"

namespace hueplusplus
{
int HueThing::getId() const
{
    return id;
}

std::string HueThing::getType() const
{
    return state["type"];
}

std::string HueThing::getName()
{
    refreshState();
    return state["name"];
}

std::string HueThing::getName() const
{
    return state["name"];
}

std::string HueThing::getModelId() const
{
    return state["modelid"];
}

std::string HueThing::getUId() const
{
    if (state.count("uniqueid"))
    {
        return state["uniqueid"];
    }
    return std::string();
}

std::string HueThing::getManufacturername() const
{
    if (state.count("manufacturername"))
    {
        return state["manufacturername"];
    }
    return std::string();
}

std::string HueThing::getProductname() const
{
    if (state.count("productname"))
    {
        return state["productname"];
    }
    return std::string();
}

std::string HueThing::getSwVersion()
{
    refreshState();
    return state["swversion"];
}

std::string HueThing::getSwVersion() const
{
    return state["swversion"];
}

bool HueThing::setName(const std::string& name)
{
    nlohmann::json request = nlohmann::json::object();
    request["name"] = name;
    nlohmann::json reply = SendPutRequest(request, "/name", CURRENT_FILE_INFO);

    // Check whether request was successful
    return utils::safeGetMember(reply, 0, "success", path + std::to_string(id) + "/name") == name;
}

HueThing::HueThing(int id, const HueCommandAPI& commands, const std::string& path)
    : id(id),
      commands(commands),
      path(path)
{
    refreshState();
}

nlohmann::json HueThing::SendPutRequest(const nlohmann::json& request, const std::string& subPath, FileInfo fileInfo)
{
    return commands.PUTRequest(path + std::to_string(id) + subPath, request, std::move(fileInfo));
}

void HueThing::refreshState()
{
    nlohmann::json answer
        = commands.GETRequest(path + std::to_string(id), nlohmann::json::object(), CURRENT_FILE_INFO);
    if (answer.count("state"))
    {
        state = answer;
    }
    else
    {
        std::cout << "Answer in HueThing::refreshState of "
                     "http_handler->GETJson(...) is not expected!\nAnswer:\n\t"
                  << answer.dump() << std::endl;
    }
}
} // namespace hueplusplus
