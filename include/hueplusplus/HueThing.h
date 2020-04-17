/**
    \file HueThing.h
    Copyright Notice\n
    Copyright (C) 2017  Jan Rogall		- developer\n
    Copyright (C) 2017  Moritz Wirger	- developer\n

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

#ifndef INCLUDE_HUEPLUSPLUS_HUE_THING_H
#define INCLUDE_HUEPLUSPLUS_HUE_THING_H

#include <memory>

#include "HueCommandAPI.h"

#include "json/json.hpp"

namespace hueplusplus
{
//!
//! Class for Hue Thing fixtures
//!
class HueThing
{
public:
    //! \brief std dtor
    ~HueThing() = default;

    //! \brief Const function that returns the id of this thing
    //!
    //! \return integer representing the thing id
    virtual int getId() const;

    //! \brief Const function that returns the thing type
    //!
    //! \return String containing the type
    virtual std::string getType() const;

    //! \brief Function that returns the name of the thing.
    //!
    //! \return String containig the name of the thing
    //! \throws std::system_error when system or socket operations fail
    //! \throws HueException when response contained no body
    //! \throws HueAPIResponseException when response contains an error
    //! \throws nlohmann::json::parse_error when response could not be parsed
    virtual std::string getName();

    //! \brief Const function that returns the name of the thing.
    //!
    //! \note This will not refresh the thing state
    //! \return String containig the name of the thing
    virtual std::string getName() const;

    //! \brief Const function that returns the modelid of the thing
    //!
    //! \return String conatining the modelid
    virtual std::string getModelId() const;

    //! \brief Const function that returns the uniqueid of the thing
    //!
    //! \note Only working on bridges with versions starting at 1.4
    //! \return String containing the uniqueid or an empty string when the function is not supported
    virtual std::string getUId() const;

    //! \brief Const function that returns the manufacturername of the thing
    //!
    //! \note Only working on bridges with versions starting at 1.7
    //! \return String containing the manufacturername or an empty string when the function is not supported
    virtual std::string getManufacturername() const;

    //! \brief Const function that returns the productname of the thing
    //!
    //! \note Only working on bridges with versions starting at 1.24
    //! \return String containing the productname or an empty string when the function is not supported
    virtual std::string getProductname() const;

    //! \brief Function that returns the software version of the thing
    //!
    //! \return String containing the software version
    //! \throws std::system_error when system or socket operations fail
    //! \throws HueException when response contained no body
    //! \throws HueAPIResponseException when response contains an error
    //! \throws nlohmann::json::parse_error when response could not be parsed
    virtual std::string getSwVersion();

    //! \brief Const function that returns the software version of the thing
    //!
    //! \note This will not refresh the thing state
    //! \return String containing the software version
    virtual std::string getSwVersion() const;

    //! \brief Function that sets the name of the thing
    //!
    //! \return Bool that is true on success
    //! \throws std::system_error when system or socket operations fail
    //! \throws HueException when response contained no body
    //! \throws HueAPIResponseException when response contains an error
    //! \throws nlohmann::json::parse_error when response could not be parsed
    virtual bool setName(const std::string& name);

protected:
    //! \brief Protected ctor that is used by \ref Hue class.
    //!
    //! \param id Integer that specifies the id of this thing
    //! \param commands HueCommandAPI for communication with the bridge
    //!
    //! leaves strategies unset
    HueThing(int id, const HueCommandAPI& commands, const std::string& path);

    //! \brief Protected function that sets the HueCommandAPI.
    //!
    //! The HueCommandAPI is used for bridge communication
    //! \param commandAPI the new HueCommandAPI
    virtual void setCommandAPI(const HueCommandAPI& commandAPI) { commands = commandAPI; };

    //! \brief Utility function to send a put request to the thing.
    //!
    //! \throws nlohmann::json::parse_error if the reply could not be parsed
    //! \param request A nlohmann::json aka the request to send
    //! \param subPath A path that is appended to the uri, note it should always start with a slash ("/")
    //! \param fileInfo FileInfo from calling function for exception details.
    //! \return The parsed reply
    //! \throws std::system_error when system or socket operations fail
    //! \throws HueException when response contained no body
    //! \throws HueAPIResponseException when response contains an error
    //! \throws nlohmann::json::parse_error when response could not be parsed
    virtual nlohmann::json SendPutRequest(const nlohmann::json& request, const std::string& subPath, FileInfo fileInfo);

    //! \brief Virtual function that refreshes the \ref state of the thing.
    //! \throws std::system_error when system or socket operations fail
    //! \throws HueException when response contained no body
    //! \throws HueAPIResponseException when response contains an error
    //! \throws nlohmann::json::parse_error when response could not be parsed
    virtual void refreshState();

protected:
    int id; //!< holds the id of the thing
    std::string path; //!< holds the path of the thing
    nlohmann::json state; //!< holds the current state of the thing updated by \ref refreshState

    HueCommandAPI commands; //!< A IHttpHandler that is used to communicate with the bridge
};
} // namespace hueplusplus

#endif
