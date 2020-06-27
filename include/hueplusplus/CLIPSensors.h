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
class CLIPSwitch : public BaseDevice
{
public:
    CLIPSwitch(Sensor sensor) : BaseDevice(std::move(sensor)) { }

    int getButtonEvent() const;

    static constexpr const char* typeStr = "CLIPSwitch";
};
class CLIPOpenClose : public BaseDevice
{
public:
    CLIPOpenClose(Sensor sensor) : BaseDevice(std::move(sensor)) { }

    bool isOn() const;
    void setOn(bool on);

    bool hasBattery() const;
    int getBatteryState() const;
    void setBatteryState(int percent);

    bool isReachable() const;

    bool hasURL() const;
    std::string getURL() const;
    void setURL(const std::string& url);

    bool isOpen() const;

    time::AbsoluteTime getLastUpdated() const;

    static constexpr const char* typeStr = "CLIPOpenClose";
};

class CLIPPresence : public BaseDevice
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

    bool getPresence() const;
    void setPresence(bool presence);

    time::AbsoluteTime getLastUpdated() const;

    static constexpr const char* typeStr = "CLIPPresence";
};

class CLIPTemperature : public BaseDevice
{
public:
    CLIPTemperature(Sensor sensor) : BaseDevice(std::move(sensor)) { }

    bool isOn() const;
    void setOn(bool on);

    bool hasBattery() const;
    int getBatteryState() const;
    void setBatteryState(int percent);

    bool isReachable() const;

    bool hasURL() const;
    std::string getURL() const;
    void setURL(const std::string& url);

    int getTemperature() const;
    void setTemperature(int temperature);

    time::AbsoluteTime getLastUpdated() const;

    static constexpr const char* typeStr = "CLIPTemperature";
};
class CLIPHumidity : public BaseDevice
{
public:
    CLIPHumidity(Sensor sensor) : BaseDevice(std::move(sensor)) { }

    bool isOn() const;
    void setOn(bool on);

    bool hasBattery() const;
    int getBatteryState() const;
    void setBatteryState(int percent);

    bool isReachable() const;

    bool hasURL() const;
    std::string getURL() const;
    void setURL(const std::string& url);

    int getHumidity() const;
    void setHumidity(int humidity);

    time::AbsoluteTime getLastUpdated() const;

    static constexpr const char* typeStr = "CLIPHumidity";
};
class CLIPLightLevel : public BaseDevice
{
public:
    CLIPLightLevel(Sensor sensor) : BaseDevice(std::move(sensor)) { }

    bool isOn() const;
    void setOn(bool on);

    bool hasBattery() const;
    int getBatteryState() const;
    void setBatteryState(int percent);

    bool isReachable() const;

    int getDarkThreshold() const;
    void setDarkThreshold(int threshold);

    int getThresholdOffset() const;
    void setThresholdOffset(int offset);

    bool hasURL() const;
    std::string getURL() const;
    void setURL(const std::string& url);

    void setLightLevel(int level);
    int getLightLevel() const;
    bool isDark() const;
    bool isDaylight() const;

    static constexpr const char* typeStr = "CLIPLightLevel";
};
class CLIPGenericFlag : public BaseDevice
{
public:
    CLIPGenericFlag(Sensor sensor) : BaseDevice(std::move(sensor)) { }

    bool isOn() const;
    void setOn(bool on);

    bool hasBattery() const;
    int getBatteryState() const;
    void setBatteryState(int percent);

    bool isReachable() const;

    bool hasURL() const;
    std::string getURL() const;
    void setURL(const std::string& url);

    bool getFlag() const;
    void setFlag(bool flag);

    static constexpr const char* typeStr = "CLIPGenericFlag";
};
class CLIPGenericStatus : public BaseDevice
{
public:
    CLIPGenericStatus(Sensor sensor) : BaseDevice(std::move(sensor)) { }

    bool isOn() const;
    void setOn(bool on);

    bool hasBattery() const;
    int getBatteryState() const;
    void setBatteryState(int percent);

    bool isReachable() const;

    bool hasURL() const;
    std::string getURL() const;
    void setURL(const std::string& url);

    int getStatus() const;
    void setStatus(int status);

    static constexpr const char* typeStr = "CLIPGenericStatus";
};
} // namespace sensors
} // namespace hueplusplus

#endif
