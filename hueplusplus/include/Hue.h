/**
    \file Hue.h
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

#ifndef _HUE_H
#define _HUE_H

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "BrightnessStrategy.h"
#include "ColorHueStrategy.h"
#include "ColorTemperatureStrategy.h"
#include "HueCommandAPI.h"
#include "HueLight.h"
#include "IHttpHandler.h"

#include "json/json.h"

// forward declarations
class Hue;

//!
//! Class to find all Hue bridges on the network and create usernames for them.
//!
class HueFinder
{
public:
    struct HueIdentification
    {
        std::string ip;
        std::string mac;
    };

public:

    //! \brief Constructor of HueFinder class
    //!
    //! \param handler HttpHandler of type \ref IHttpHandler for communication with the bridge
    HueFinder(std::shared_ptr<const IHttpHandler> handler);

    //! \brief Function that finds all bridges in the network and returns them.
    //!
    //! The user should be given the opportunity to select the correct one based on the mac address.
    //! \return vector containing ip and mac of all found bridges
    std::vector<HueIdentification> FindBridges() const;

    //! \brief Function that gets a \ref Hue bridge based on its identification
    //!
    //! \param identification \ref HueIdentification that specifies a bridge
    //! \return \ref Hue class object
    Hue GetBridge(const HueIdentification& identification);

    //! \brief Function that adds a username to the \ref usernames map
    //!
    //! \param mac MAC address of Hue bridge
    //! \param username Username that is used to control the Hue bridge
    void AddUsername(const std::string& mac, const std::string& username);

    //! \brief Function that returns a map of mac addresses and usernames.
    //!
    //! Note these should be saved at the end and re-loaded with \ref AddUsername next time, so only one username is generated per bridge.
    //! \returns A map mapping mac address to username for every bridge
    const std::map<std::string, std::string>& GetAllUsernames() const;

private:
    std::map<std::string, std::string> usernames;		//!< Maps all macs to usernames added by \ref HueFinder::AddUsername
    std::shared_ptr<const IHttpHandler> http_handler;
};

//! Hue class
class Hue
{
    friend class HueFinder;

public:
    //! \brief Constructor of Hue class
    //!
    //! \param ip String that specifies the ip address of the Hue bridge in dotted decimal notation like "192.168.2.1"
    //! \param username String that specifies the username that is used to control the bridge. This needs to be acquired in \ref requestUsername
    //! \param handler HttpHandler of type \ref IHttpHandler for communication with the bridge
    Hue(const std::string& ip, const std::string& username, std::shared_ptr<const IHttpHandler> handler);

    //! \brief Function to get the ip address of the hue bridge
    //!
    //! \return string containing ip
    std::string getBridgeIP();

    //! \brief Function that sends a username request to the Hue bridge.
    //!
    //! It does that for about 30 seconds and you have 5 seconds to prepare
    //! It automatically sets the \ref username variable according to the username received and returns the username received
    //! This function should only be called once to acquire a username to control the bridge and the username should be saved for future use
    //! \param ip String that specifies the ip (in dotted decimal notation like "192.168.2.1") the request is send to
    //! \return String containing username
    std::string requestUsername(const std::string& ip);

    //! \brief Function that returns the \ref username
    //!
    //! \return String containing \ref username
    std::string getUsername();

    //! \brief Function to set the ip address of this class representing a bridge
    //!
    //! \param ip String that specifies the ip in dotted decimal notation like "192.168.2.1"
    void setIP(const std::string& ip);

    //! \brief Function that returns a \ref Hue::HueLight of specified id
    //!
    //! \param id Integer that specifies the ID of a Hue light
    //! \return \ref HueLight that can be controlled
    HueLight& getLight(int id);

    //! \brief Function to remove a light from the bridge
    //!
    //! \attention Any use of the light after it was successfully removed results in undefined behavior
    //! \param id Id of the light to remove
    //! \return Bool that is true on success
    bool removeLight(int id);

    //! \brief Function that returns all light types that are associated with this bridge
    //!
    //! \return A map mapping light id's to light types for every light
    //const std::map<uint8_t, ColorType>& getAllLightTypes();

    //! \brief Function that returns all lights that are associated with this bridge
    //!
    //! \return A vector containing references to every HueLight
    std::vector<std::reference_wrapper<HueLight>> getAllLights();

    //! \brief Function that tells whether a given light id represents an existing light
    //!
    //! Calls refreshState to update the local bridge state
    //! \param id Id of a light to check for existance
    //! \return Bool that is true when a light with the given id exists and false when not
    bool lightExists(int id);

    //! \brief Const function that tells whether a given light id represents an existing light
    //!
    //! \note This will not update the local state of the bridge
    //! \param id Id of a light to check for existance
    //! \return Bool that is true when a light with the given id exists and false when not
    bool lightExists(int id) const;

    //! \brief Const function that returns the picture name of a given light id
	//!
	//! \note This will not update the local state of the bridge.
	//! \note This function will only return the filename without extension, because
	//! Philips provides different file types.
	//! \param id Id of a light to get the picture of
	//! \return String that either contains the filename of the picture of the light or if it was not found an empty string
	std::string getPictureOfLight(int id) const;

	//! \brief Const function that returns the picture name of a given model id
	//!
	//! \note This will not update the local state of the bridge.
	//! \note This function will only return the filename without extension, because
	//! Philips provides different file types.
	//! \param model_id Model Id of a device to get the picture of
	//! \return String that either contains the filename of the picture of the device or if it was not found an empty string
	std::string getPictureOfModel(const std::string& model_id) const;

    //! \brief Function that sets the HttpHandler and updates the HueCommandAPI.
    //!
    //! The HttpHandler and HueCommandAPI are used for bridge communication
    //! \param handler a HttpHandler of type \ref IHttpHandler
    void setHttpHandler(std::shared_ptr<const IHttpHandler> handler) { http_handler = std::move(handler); commands = HueCommandAPI(ip, username, http_handler); };

private:
    //! \brief Function that refreshes the local \ref state of the Hue bridge
    void refreshState();

private:
    std::string ip;			//!< IP-Address of the hue bridge in dotted decimal notation like "192.168.2.1"
    std::string username;	//!< Username that is ussed to access the hue bridge
    Json::Value state;		//!< The state of the hue bridge as it is returned from it
    std::map< uint8_t, HueLight > lights;	//!< Maps ids to HueLights that are controlled by this bridge

    std::shared_ptr<BrightnessStrategy>         simpleBrightnessStrategy;           //!< Strategy that is used for controlling the brightness of lights
    std::shared_ptr<ColorHueStrategy>           simpleColorHueStrategy;             //!< Strategy that is used for controlling the color of lights
    std::shared_ptr<ColorHueStrategy>           extendedColorHueStrategy;           //!< Strategy that is used for controlling the color of lights
    std::shared_ptr<ColorTemperatureStrategy>   simpleColorTemperatureStrategy;     //!< Strategy that is used for controlling the color temperature of lights
    std::shared_ptr<ColorTemperatureStrategy>   extendedColorTemperatureStrategy;   //!< Strategy that is used for controlling the color temperature of lights
    std::shared_ptr<const IHttpHandler> http_handler;                               //!< A IHttpHandler that is used to communicate with the bridge
    HueCommandAPI commands;                                                         //!< A HueCommandAPI that is used to communicate with the bridge
};

#endif
