/**
    \file ZLLSensors.h
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

#ifndef INCLUDE_HUEPLUSPLUS_ZLL_SENSORS_H
#define INCLUDE_HUEPLUSPLUS_ZLL_SENSORS_H

#include "Sensor.h"

namespace hueplusplus
{
namespace sensors
{
class ZGPSwitch : public BaseDevice
{
public:
    explicit ZGPSwitch(Sensor sensor) : BaseDevice(std::move(sensor)) { }

    bool isOn() const;
    void setOn(bool on);

    int getButtonEvent() const;

    static constexpr int c_button1 = 34;
    static constexpr int c_button2 = 16;
    static constexpr int c_button3 = 17;
    static constexpr int c_button4 = 18;

    static constexpr const char* typeStr = "ZGPSwitch";
};
class ZLLSwitch : public BaseDevice
{
public:
    explicit ZLLSwitch(Sensor sensor) : BaseDevice(std::move(sensor)) { }

    bool isOn() const;
    void setOn(bool on);

    bool hasBatteryState() const;
    int getBatteryState() const;

    Alert getLastAlert() const;
    void sendAlert(Alert type);

    bool isReachable() const;

    int getButtonEvent() const;

    time::AbsoluteTime getLastUpdated() const;

    static constexpr int c_ON_INITIAL_PRESS = 1000;
    static constexpr int c_ON_HOLD = 1001;
    static constexpr int c_ON_SHORT_RELEASED = 1002;
    static constexpr int c_ON_LONG_RELEASED = 1003;
    static constexpr int c_UP_INITIAL_PRESS = 2000;
    static constexpr int c_UP_HOLD = 2001;
    static constexpr int c_UP_SHORT_RELEASED = 2002;
    static constexpr int c_UP_LONG_RELEASED = 2003;
    static constexpr int c_DOWN_INITIAL_PRESS = 3000;
    static constexpr int c_DOWN_HOLD = 3001;
    static constexpr int c_DOWN_SHORT_RELEASED = 3002;
    static constexpr int c_DOWN_LONG_RELEASED = 3003;
    static constexpr int c_OFF_INITIAL_PRESS = 4000;
    static constexpr int c_OFF_HOLD = 4001;
    static constexpr int c_OFF_SHORT_RELEASED = 4002;
    static constexpr int c_OFF_LONG_RELEASED = 4003;

    static constexpr const char* typeStr = "ZLLSwitch";
};
class ZLLPresence : public BaseDevice
{
public:
    explicit ZLLPresence(Sensor sensor) : BaseDevice(std::move(sensor)) { }
    bool isOn() const;
    void setOn(bool on);

    bool hasBatteryState() const;
    int getBatteryState() const;

    Alert getLastAlert() const;
    void sendAlert(Alert type);

    bool isReachable() const;

    int getSensitivity() const;
    int getMaxSensitivity() const;
    void setSensitivity(int sensitivity);

    bool getPresence() const;

    time::AbsoluteTime getLastUpdated() const;

    static constexpr const char* typeStr = "ZLLPresence";
};

class ZLLTemperature : public BaseDevice
{
public:
    explicit ZLLTemperature(Sensor sensor) : BaseDevice(std::move(sensor)) { }

    bool isOn() const;
    void setOn(bool on);

    bool hasBatteryState() const;
    int getBatteryState() const;

    Alert getLastAlert() const;
    void sendAlert(Alert type);

    bool isReachable() const;

    int getTemperature() const;

    static constexpr const char* typeStr = "ZLLTemperature";
};
class ZLLLightLevel : public BaseDevice
{
public:
    explicit ZLLLightLevel(Sensor sensor) : BaseDevice(std::move(sensor)) { }

    bool isOn() const;
    void setOn(bool on);

    bool hasBatteryState() const;
    int getBatteryState() const;

    bool isReachable() const;

    int getDarkThreshold() const;
    void setDarkThreshold(int threshold);

    int getThresholdOffset() const;
    void setThresholdOffset(int offset);

    int getLightLevel() const;
    bool isDark() const;
    bool isDaylight() const;

    time::AbsoluteTime getLastUpdated() const;

    static constexpr const char* typeStr = "ZLLLightLevel";
};
} // namespace sensors
} // namespace hueplusplus

#endif
