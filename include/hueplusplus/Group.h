/**
    \file Group.h
    Copyright Notice\n
    Copyright (C) 2020  Jan Rogall		- developer\n
    Copyright (C) 2020  Moritz Wirger	- developer\n

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

#ifndef INCLUDE_HUEPLUSPLUS_GROUP_H
#define INCLUDE_HUEPLUSPLUS_GROUP_H

#include <string>
#include <vector>

#include "APICache.h"
#include "HueCommandAPI.h"
#include "StateTransaction.h"

#include "json/json.hpp"

namespace hueplusplus
{
class Group
{
public:
    Group(int id, const HueCommandAPI& commands, std::chrono::steady_clock::duration refreshDuration);

    virtual ~Group() = default;

    void Refresh();

    int getId() const;
    std::string getName() const;
    std::string getType() const;
    std::vector<int> getLightIds() const;

    void setName(const std::string& name);
    void setLights(const std::vector<int>& ids);

    // Only for type room
    std::string getRoomType() const;
    void setRoomType(const std::string& type);

    // Only for type luminaire
    std::string getModelId() const;
    // For luminaire or lightsource
    std::string getUniqueId() const;

    bool getAllOn();
    bool getAllOn() const;
    bool getAnyOn();
    bool getAnyOn() const;

    bool getActionOn();
    bool getActionOn() const;
    std::pair<uint16_t, uint8_t> getActionHueSaturation();
    std::pair<uint16_t, uint8_t> getActionHueSaturation() const;
    unsigned int getActionBrightness();
    unsigned int getActionBrightness() const;
    unsigned int getActionColorTemperature();
    unsigned int getActionColorTemperature() const;
    std::pair<float, float> getActionColorXY();
    std::pair<float, float> getActionColorXY() const;
    std::string getActionColorMode();
    std::string getActionColorMode() const;

    StateTransaction transaction();

    void setOn(bool on, uint8_t transition = 4);
    void setBrightness(uint8_t brightness, uint8_t transition = 4);
    void setColorHueSaturation(uint16_t hue, uint8_t saturation, uint8_t transition = 4);
    void setColorXY(float x, float y, uint8_t transition = 4);
    void setColorTemperature(unsigned int mired, uint8_t transition = 4);
    void setColorLoop(bool on, uint8_t transition = 4);
    void incrementBrightness(int increment, uint8_t transition = 4);
    void incrementSaturation(int increment, uint8_t transition = 4);
    void incrementHue(int increment, uint8_t transition = 4);
    void incrementColorTemperature(int increment, uint8_t transition = 4);
    void incrementColorXY(float incX, float incY, uint8_t transition = 4);
    void setScene(const std::string& scene);

protected:
    nlohmann::json SendPutRequest(const nlohmann::json& request, const std::string& subPath, FileInfo fileInfo);

protected:
    int id;
    APICache state;
    HueCommandAPI commands;
};

class CreateGroup
{
public:
    static CreateGroup LightGroup(const std::vector<int>& lights, const std::string& name = "");
    static CreateGroup Room(
        const std::vector<int>& lights, const std::string& name = "", const std::string& roomType = "");
    static CreateGroup Entertainment(const std::vector<int>& lights, const std::string& name = "");

    nlohmann::json getRequest() const;

protected:
    CreateGroup(
        const std::vector<int>& lights, const std::string& name, const std::string& type, const std::string& roomType);

private:
    std::vector<int> lights;
    std::string name;
    std::string type;
    std::string roomType;
};
} // namespace hueplusplus

#endif