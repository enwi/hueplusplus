/**
    \file CLIPSensors.h
    Copyright Notice\n
    Copyright (C) 2020  Jan Rogall	- developer\n

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
 */

#ifndef INCLUDE_HUEPLUSPLUS_CLIP_SENSORS_H
#define INCLUDE_HUEPLUSPLUS_CLIP_SENSORS_H

#include "Sensor.h"

namespace hueplusplus
{
namespace sensors
{
class BaseCLIP : public BaseDevice
{
public:
    bool isOn() const;
    void setOn(bool on);

    bool hasBattery() const;
    int getBatteryState() const;
    void setBatteryState(int percent);

    bool isReachable() const;

    bool hasURL() const;
    std::string getURL() const;
    void setURL(const std::string& url);

    time::AbsoluteTime getLastUpdated() const;
protected:
    BaseCLIP(Sensor sensor) : BaseDevice(std::move(sensor)) { }
};
class CLIPSwitch : public BaseCLIP
{
public:
    CLIPSwitch(Sensor sensor) : BaseCLIP(std::move(sensor)) { }

    int getButtonEvent() const;
    void setButtonEvent(int code);

    static constexpr const char* typeStr = "CLIPSwitch";
};
class CLIPOpenClose : public BaseCLIP
{
public:
    CLIPOpenClose(Sensor sensor) : BaseCLIP(std::move(sensor)) { }

    bool isOpen() const;

    static constexpr const char* typeStr = "CLIPOpenClose";
};

class CLIPPresence : public BaseCLIP
{
public:
    CLIPPresence(Sensor sensor) : BaseCLIP(std::move(sensor)) { }

    bool getPresence() const;
    void setPresence(bool presence);

    static constexpr const char* typeStr = "CLIPPresence";
};

class CLIPTemperature : public BaseCLIP
{
public:
    CLIPTemperature(Sensor sensor) : BaseCLIP(std::move(sensor)) { }

    int getTemperature() const;
    void setTemperature(int temperature);

    static constexpr const char* typeStr = "CLIPTemperature";
};
class CLIPHumidity : public BaseCLIP
{
public:
    CLIPHumidity(Sensor sensor) : BaseCLIP(std::move(sensor)) { }

    int getHumidity() const;
    void setHumidity(int humidity);

    static constexpr const char* typeStr = "CLIPHumidity";
};
class CLIPLightLevel : public BaseCLIP
{
public:
    CLIPLightLevel(Sensor sensor) : BaseCLIP(std::move(sensor)) { }

    int getDarkThreshold() const;
    void setDarkThreshold(int threshold);

    int getThresholdOffset() const;
    void setThresholdOffset(int offset);

    void setLightLevel(int level);
    int getLightLevel() const;
    bool isDark() const;
    bool isDaylight() const;

    static constexpr const char* typeStr = "CLIPLightLevel";
};
class CLIPGenericFlag : public BaseCLIP
{
public:
    CLIPGenericFlag(Sensor sensor) : BaseCLIP(std::move(sensor)) { }

    bool getFlag() const;
    void setFlag(bool flag);

    static constexpr const char* typeStr = "CLIPGenericFlag";
};
class CLIPGenericStatus : public BaseCLIP
{
public:
    CLIPGenericStatus(Sensor sensor) : BaseCLIP(std::move(sensor)) { }

    int getStatus() const;
    void setStatus(int status);

    static constexpr const char* typeStr = "CLIPGenericStatus";
};
} // namespace sensors
} // namespace hueplusplus

#endif
