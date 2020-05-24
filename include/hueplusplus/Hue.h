/**
    \file Hue.h
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

#ifndef INCLUDE_HUEPLUSPLUS_HUE_H
#define INCLUDE_HUEPLUSPLUS_HUE_H

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "APICache.h"
#include "BridgeConfig.h"
#include "BrightnessStrategy.h"
#include "ColorHueStrategy.h"
#include "ColorTemperatureStrategy.h"
#include "Group.h"
#include "HueCommandAPI.h"
#include "HueDeviceTypes.h"
#include "HueLight.h"
#include "IHttpHandler.h"
#include "ResourceList.h"
#include "Scene.h"
#include "Schedule.h"

#include "json/json.hpp"

//! \brief Namespace for the hueplusplus library
namespace hueplusplus
{
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
        int port = 80;
        std::string mac;
    };

public:
    //! \brief Constructor of HueFinder class
    //!
    //! \param handler HttpHandler of type \ref IHttpHandler for communication with the bridge
    HueFinder(std::shared_ptr<const IHttpHandler> handler);

    //! \brief Finds all bridges in the network and returns them.
    //!
    //! The user should be given the opportunity to select the correct one based on the mac address.
    //! \return vector containing ip and mac of all found bridges
    //! \throws std::system_error when system or socket operations fail
    //! \throws HueException when response contained no body
    std::vector<HueIdentification> FindBridges() const;

    //! \brief Gets a \ref Hue bridge based on its identification
    //!
    //! \param identification \ref HueIdentification that specifies a bridge
    //! \return \ref Hue class object
    //! \throws std::system_error when system or socket operations fail
    //! \throws HueException when response contained no body or username could not be requested
    //! \throws HueAPIResponseException when response contains an error
    //! \throws nlohmann::json::parse_error when response could not be parsed
    Hue GetBridge(const HueIdentification& identification);

    //! \brief Function that adds a username to the usernames map
    //!
    //! \param mac MAC address of Hue bridge
    //! \param username Username that is used to control the Hue bridge
    void AddUsername(const std::string& mac, const std::string& username);

    //! \brief Function that returns a map of mac addresses and usernames.
    //!
    //! Note these should be saved at the end and re-loaded with \ref AddUsername
    //! next time, so only one username is generated per bridge. \returns A map
    //! mapping mac address to username for every bridge
    const std::map<std::string, std::string>& GetAllUsernames() const;

    //! \brief Normalizes mac address to plain hex number.
    //! \returns \p input without separators and whitespace, in lower case.
    static std::string NormalizeMac(std::string input);

private:
    //! \brief Parses mac address from description.xml
    //!
    //! \param description Content of description.xml file as returned by GET request.
    //! \returns Content of xml element \c serialNumber if description matches a Hue bridge, otherwise an empty
    //! string.
    static std::string ParseDescription(const std::string& description);

    std::map<std::string, std::string> usernames; //!< Maps all macs to usernames added by \ref
                                                  //!< HueFinder::AddUsername
    std::shared_ptr<const IHttpHandler> http_handler;
};

template <typename T>
class MakeCopyable : public T
{
public:
    using T::T;
    using T::operator=;
};

//! \brief Hue class for a bridge.
//!
//! This is the main class used to interact with the Hue bridge.
class Hue
{
    friend class HueFinder;

public:
    using LightList = ResourceList<HueLight, int>;
    using GroupList = GroupResourceList<Group, CreateGroup>;
    using ScheduleList = CreateableResourceList<Schedule, int, CreateSchedule>;
    using SceneList = CreateableResourceList<Scene, std::string, CreateScene>;

public:
    //! \brief Constructor of Hue class
    //!
    //! \param ip IP address in dotted decimal notation like "192.168.2.1"
    //! \param port Port of the hue bridge
    //! \param username String that specifies the username that is used to control
    //! the bridge. Can be left empty and acquired in \ref requestUsername.
    //! \param handler HttpHandler for communication with the bridge
    //! \param refreshDuration Time between refreshing the cached state.
    Hue(const std::string& ip, const int port, const std::string& username, std::shared_ptr<const IHttpHandler> handler,
        std::chrono::steady_clock::duration refreshDuration = std::chrono::seconds(10));

    void refresh();

    //! \name Configuration
    ///@{

    //! \brief Function to get the ip address of the hue bridge
    //!
    //! \return string containing ip
    std::string getBridgeIP() const;

    //! \brief Function to get the port of the hue bridge
    //!
    //! \return integer containing port
    int getBridgePort() const;

    //! \brief Send a username request to the Hue bridge.
    //!
    //! Blocks for about 30 seconds and 5 seconds to prepare.
    //! It automatically sets the username variable according to the username received and returns the username
    //! received. This function should only be called once to acquire a username to control the bridge and the
    //! username should be saved for future use. \return username for API usage \throws std::system_error when
    //! system or socket operations fail \throws HueException when response contained no body \throws
    //! HueAPIResponseException when response contains an error except link button not pressed. \throws
    //! nlohmann::json::parse_error when response could not be parsed
    std::string requestUsername();

    //! \brief Function that returns the username
    //!
    //! \return The username used for API access
    std::string getUsername() const;

    //! \brief Function to set the ip address of this class representing a bridge
    //!
    //! \param ip String that specifies the ip in dotted decimal notation like "192.168.2.1"
    void setIP(const std::string& ip);

    //! \brief Function to set the port of this class representing a bridge
    //!
    //! \param port Integer that specifies the port of an address like
    //! "192.168.2.1:8080"
    void setPort(const int port);

    //! \brief Function that sets the HttpHandler and updates the HueCommandAPI.
    //!
    //! The HttpHandler and HueCommandAPI are used for bridge communication
    //! \param handler a HttpHandler of type \ref IHttpHandler
    void setHttpHandler(std::shared_ptr<const IHttpHandler> handler);

    BridgeConfig& config();
    const BridgeConfig& config() const;

    LightList& lights();
    const LightList& lights() const;

    GroupList& groups();
    const GroupList& groups() const;

    ScheduleList& schedules();
    const ScheduleList& schedules() const;

    SceneList& scenes();
    const SceneList& scenes() const;

private:
    std::string ip; //!< IP-Address of the hue bridge in dotted decimal notation
                    //!< like "192.168.2.1"
    std::string username; //!< Username that is ussed to access the hue bridge
    int port;

    std::shared_ptr<const IHttpHandler> http_handler; //!< A IHttpHandler that is used to communicate with the
                                                      //!< bridge
    std::chrono::steady_clock::duration refreshDuration;
    std::shared_ptr<APICache> stateCache;
    MakeCopyable<LightList> lightList;
    MakeCopyable<GroupList> groupList;
    MakeCopyable<ScheduleList> scheduleList;
    MakeCopyable<SceneList> sceneList;
    MakeCopyable<BridgeConfig> bridgeConfig;
};
} // namespace hueplusplus

#endif
