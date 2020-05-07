/**
    \file Scene.h
    Copyright Notice\n
    Copyright (C) 2020  Jan Rogall		- developer\n

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

#ifndef INCLUDE_HUEPLUSPLUS_SCENE_H
#define INCLUDE_HUEPLUSPLUS_SCENE_H

#include <map>
#include <string>
#include <vector>

#include <json/json.hpp>

#include "APICache.h"
#include "TimePattern.h"
#include "Units.h"

namespace hueplusplus
{
//! \brief Immutable state of a light
class LightState
{
public:

    LightState(const nlohmann::json& state);

    bool isOn() const;

    bool hasBrightness() const;
    int getBrightness() const;

    bool hasHueSat() const;
    HueSaturation getHueSat() const;

    bool hasXY() const;
    XY getXY() const;

    bool hasCt() const;
    int getCt() const;

    bool hasEffect() const;
    bool getColorloop() const;

    int getTransitionTime() const;

    nlohmann::json toJson() const;
private:
    nlohmann::json state;
};

class Scene
{
public:
    enum class Type
    {
        lightScene,
        groupScene
    };

    void refresh();
    std::string getId() const;
    std::string getName() const;
    void setName(const std::string& name);

    Type getType() const;

    int getGroupId() const;
    void setGroupId(int id);

    std::vector<int> getLightIds() const;
    void setLightIds(const std::vector<int>& ids);

    std::string getOwner() const;
    bool getRecycle() const;
    bool isLocked() const;

    std::string getAppdata() const;
    int getAppdataVersion() const;
    void setAppdata(const std::string& data, int version);

    std::string getPicture() const;
    time::AbsoluteTime getLastUpdated() const;
    int getVersion() const;

    std::map<int, LightState> getLightStates() const;
    void setLightStates(const std::map<int, LightState>& states);

    void storeCurrentLightState();
    void storeCurrentLightState(int transition);

    void recall();
private:
    void sendPutRequest(const std::string& path, const nlohmann::json& request, FileInfo fileInfo);
private:
    int id;
    APICache state;
};

class CreateScene
{
public:
    CreateScene& setName(const std::string& name);
    CreateScene& setGroupId(int id);
    CreateScene& setLightIds(const std::vector<int>& ids);
    CreateScene& setRecycle(bool recycle);
    CreateScene& setAppdata(const std::string& data, int version);
    CreateScene& setLightStates(const std::map<int, LightState>& states);

    nlohmann::json getRequest() const;

private:
    nlohmann::json request;
};
} // namespace hueplusplus

#endif
