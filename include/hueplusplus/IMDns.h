/**
    \file IMDns.h
    Copyright Notice\n
    Copyright (C) 2026  Jan Rogall		- developer\n

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

#ifndef INCLUDE_HUEPLUSPLUS_IMDNS_H
#define INCLUDE_HUEPLUSPLUS_IMDNS_H

#include <map>
#include <string>
#include <vector>

namespace hueplusplus
{

//! Abstract class for classes that implement mDNS discovery
class IMDns
{
public:
    struct ServiceInfo
    {
        std::string name;
        int priority;
        int weight;
        int port;
        std::map<std::string, std::string> textRecords;
        std::vector<std::string> aRecords;
        std::vector<std::string> aaaaRecords;
    };

public:
    virtual ~IMDns() = default;

    //! \brief Searches for mDNS devices and returns all found ones.
    //!
    //! \param serviceName The mDNS name of the service.
    //! \return A map from the address to other information of all found devices
    virtual std::vector<ServiceInfo> getDevices(const std::string& serviceName) = 0;
};
} // namespace hueplusplus

#endif