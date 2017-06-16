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

#include "HueLight.h"

#include "json/json.h"

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

// forward declarations
class Hue;

//! Class to find all Hue bridges on the network and create usernames for them.
class HueFinder
{
public:
	struct HueIdentification
	{
		std::string ip;
		std::string mac;
	};
public:
	//! Function that finds all bridges in the network and returns them.\n
	//! The user should be given the opportunity to select the correct one based on the mac address.
	//! \return vector containing ip and mac of all found bridges
	std::vector<HueIdentification> FindBridges() const;

	//! Function that gets a \ref Hue bridge based on its identification
	//! \param identification \ref HueIdentification that specifies a bridge
	//! \return \ref Hue class object
	Hue GetBridge(const HueIdentification& identification);

	//! Function that adds a \ref username to the \ref usernames map
	//! \param mac MAC address of Hue bridge
	//! \param username Username that is used to control the Hue bridge
	void AddUsername(const std::string& mac, const std::string& username);

	//! Function that returns a map of mac addresses and usernames.
	//! These should be saved at the end and re-loaded next time, so only one username is generated per bridge.
	//! \returns A map mapping mac address to username for every bridge
	const std::map<std::string, std::string>& GetAllUsernames() const;
private:
	//! Function that sends a username request to the Hue bridge for about 30 seconds, but you have 5 seconds to prepare.\n
	//! It returns the username received
	//! \param ip String that specifies the ip the request is send to
	//! \return String containing username
	std::string RequestUsername(const std::string& ip) const;

private:
	std::map<std::string, std::string> usernames;
};

//! Hue class
class Hue
{
public:
	//! Constructor of Hue class
	//! \param ip String that specifies the ip address of the Hue bridge
	//! \param username String that specifies the username that is used to control the bridge. This needs to be acquired in \ref requestUsername
	Hue(const std::string& ip, const std::string& username);

	//! Function to get the ip address of the hue bridge
	//! \return string containing ip
	std::string getBridgeIP();

	//! Function that sends a username request to the Hue bridge for about 30 seconds, but you have 5 seconds to prepare
	//! It automatically sets the \ref username variable according to the username received
	//! This function should only be called once to acquire a username to control the bridge and the username should be saved for future use
	//! \param ip String that specifies the ip the request is send to
	void requestUsername(const std::string& ip);

	//! Function that returns the \ref username
	//! \return String containing \ref username
	std::string getUsername();

	//! Function to set the ip address of the Hue bridge in this class
	//! \param ip String that specifies the ip
	void setIP(const std::string ip);

	// todo: some intelligence of finding light
	//! Function that returns a \HueLight of specified \ref id
	//! \param id Integer that specifies the ID of a Hue light
	//! \return \ref HueLight that can be controlled
	std::unique_ptr<HueLight> getLight(int id);

	//! Function that returns all light types that are associated with this bridge
	//! \return A map mapping light id's to light types for every light
	const std::map<uint8_t, ColorType>& getAllLightTypes();

	//! Function that returns all lights that are associated with this bridge
	//! \return A vector containing pointers pointing to every HueLight
	std::vector<std::unique_ptr<HueLight>> getAllLights();

private:
	//! Function that refreshes the local \ref state of the Hue bridge
	void refreshState();

private:
	std::string ip;
	std::string username;
	Json::Value state;
	std::map<uint8_t, ColorType> lights;
};

#endif