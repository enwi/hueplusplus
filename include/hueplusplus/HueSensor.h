/**
    \file HueSensor.h
    Copyright Notice\n
    Copyright (C) 2020  Stefan Herbrechtsmeier	- developer\n

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

#include "HueCommandAPI.h"
#include "HueThing.h"

#include "json/json.hpp"

namespace hueplusplus
{
//!
//! Class for Hue Sensor fixtures
//!
class HueSensor : public HueThing
{
    friend class Hue;

public:
    //! \brief std dtor
    ~HueSensor() = default;

    //! \brief Function to get button event
    //!
    //! \return integer representing the button event
    //! \throws std::system_error when system or socket operations fail
    //! \throws HueException when response contained no body
    //! \throws HueAPIResponseException when response contains an error
    //! \throws nlohmann::json::parse_error when response could not be parsed
    virtual int getButtonEvent();

    //! \brief Const function to get button event
    //!
    //! \note This will not refresh the sensor state
    //! \return integer representing the button event
    virtual int getButtonEvent() const;

    //! \brief Function to get sensor status
    //!
    //! \return integer representing the status
    //! \throws std::system_error when system or socket operations fail
    //! \throws HueException when response contained no body
    //! \throws HueAPIResponseException when response contains an error
    //! \throws nlohmann::json::parse_error when response could not be parsed
    virtual int getStatus();

    //! \brief Const function to get sensor status
    //!
    //! \note This will not refresh the sensor state
    //! \return integer representing the button event
    virtual int getStatus() const;

    //! \brief Const function to check whether this sensor has a button event
    //!
    //! \return Bool that is true when the sensor has specified abilities and false
    //! when not
    virtual bool hasButtonEvent() const;

    //! \brief Const function to check whether this sensor has a status
    //!
    //! \return Bool that is true when the sensor has specified abilities and false
    //! when not
    virtual bool hasStatus() const;

protected:
    //! \brief Protected ctor that is used by \ref Hue class.
    //!
    //! \param id Integer that specifies the id of this sensor
    //! \param commands HueCommandAPI for communication with the bridge
    //!
    //! leaves strategies unset
    HueSensor(int id, const HueCommandAPI& commands);
};
} // namespace hueplusplus

#endif
