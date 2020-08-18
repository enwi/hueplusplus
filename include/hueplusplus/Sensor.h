/**
    \file Sensor.h
    Copyright Notice\n
    Copyright (C) 2020  Stefan Herbrechtsmeier	- developer\n
    Copyright (C) 2020  Jan Rogall          	- developer\n

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

#ifndef INCLUDE_HUEPLUSPLUS_HUE_SENSOR_H
#define INCLUDE_HUEPLUSPLUS_HUE_SENSOR_H

#include <memory>

#include "BaseDevice.h"
#include "HueCommandAPI.h"
#include "TimePattern.h"

#include "json/json.hpp"

namespace hueplusplus
{
enum class Alert
{
    none,
    select,
    lselect
};
std::string alertToString(Alert alert);
Alert alertFromString(const std::string& s);
//!
//! Generic class for Hue sensors
//!
class Sensor : public BaseDevice
{
public:
    //! \brief Construct Sensor.
    //! \param id Integer that specifies the id of this sensor
    //! \param commands HueCommandAPI for communication with the bridge
    //! \param refreshDuration Time between refreshing the cached state.
    Sensor(int id, const HueCommandAPI& commands, std::chrono::steady_clock::duration refreshDuration);

    //!\name Config attributes
    ///@{

    bool hasOn() const;
    // Check whether sensor is on. Does not update when off
    bool isOn() const;
    void setOn(bool on);

    bool hasBatteryState() const;
    // Battery state in percent
    int getBatteryState() const;
    void setBatteryState(int percent);

    bool hasAlert() const;
    Alert getLastAlert() const;
    void sendAlert(Alert type);

    bool hasReachable() const;
    bool isReachable() const;

    bool hasUserTest() const;
    void setUserTest(bool enabled);

    bool hasURL() const;
    std::string getURL() const;
    void setURL(const std::string& url);
 
    std::vector<std::string> getPendingConfig() const;
  
    bool hasLEDIndication() const;
    bool getLEDIndication() const;
    void setLEDIndication(bool on);

    nlohmann::json getConfig() const;
    void setConfigAttribute(const std::string& key, const nlohmann::json& value);

    ///@}
    
    time::AbsoluteTime getLastUpdated() const;
       
    nlohmann::json getState() const;
    void setStateAttribute(const std::string& key, const nlohmann::json& value);

    bool isCertified() const;
    bool isPrimary() const;

    template <typename T>
    T asSensorType() const &
    {
        if (getType() != T::typeStr)
        {
            throw HueException(FileInfo {__FILE__, __LINE__, __func__}, "Sensor type does not match: " + getType());
        }
        return T(*this);
    }
    template <typename T>
    T asSensorType() &&
    {
        if (getType() != T::typeStr)
        {
            throw HueException(FileInfo {__FILE__, __LINE__, __func__}, "Sensor type does not match: " + getType());
        }
        return T(std::move(*this));
    }
};

class CreateSensor
{
public:
    CreateSensor(const std::string& name, const std::string& modelid, const std::string& swversion,
        const std::string& type, const std::string& uniqueid, const std::string& manufacturername);

    CreateSensor& setState(const nlohmann::json& state);
    CreateSensor& setConfig(const nlohmann::json& config);
    CreateSensor& setRecycle(bool recycle);

    nlohmann::json getRequest() const;
protected:
    nlohmann::json request;
};

namespace sensors
{
class DaylightSensor : public BaseDevice
{
public:
    explicit DaylightSensor(Sensor sensor) : BaseDevice(std::move(sensor)) { }

    bool isOn() const;
    void setOn(bool on);

    bool hasBatteryState() const;
    int getBatteryState() const;
    void setBatteryState(int percent);

    void setCoordinates(const std::string& latitude, const std::string& longitude);
    bool isConfigured() const;

    int getSunriseOffset() const;
    void setSunriseOffset(int minutes);

    int getSunsetOffset() const;
    void setSunsetOffset(int minutes);

    bool isDaylight() const;

    static constexpr const char* typeStr = "Daylight";
};

} // namespace sensors

} // namespace hueplusplus

#endif
