/**
    \file MDnsWrapper.h
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

#ifndef INCLUDE_HUEPLUSPLUS_MDNS_H
#define INCLUDE_HUEPLUSPLUS_MDNS_H

#include <chrono>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "hueplusplus/IMDns.h"

namespace hueplusplus
{
//! Class that resolves mDNS devices
//!
//! Exposes only the subset of the public domain mdns.h library that we need for this library, not a complete
//! implementation.
class MDnsWrapper : public IMDns
{
public:
    //! \brief Creates an instance that binds to all network interfaces
    MDnsWrapper() = default;

    //! \brief Creates an instance that binds only to the selected network interfaces
    //! \param interfaces Network interface specifications (IP address) to bind to.
    //! \param searchTimeout Duration to wait for mDNS responses, default 4 seconds.
    //! An empty vector binds to all interfaces
    //!
    //! If an interface does not exist it will be ignored, but at least one must be valid.
    explicit MDnsWrapper(std::vector<std::string> interfaces,
        std::chrono::steady_clock::duration searchTimeout = std::chrono::seconds(4));

    ~MDnsWrapper() override = default;

    //! \brief Searches for mDNS devices and returns all found ones.
    //!
    //! \param serviceName The mDNS name of the service.
    //! \throws HueException If no sockets could be opened.
    //! \return A map from the address to other information of all found devices
    std::vector<ServiceInfo> getDevices(const std::string& serviceName) override;

private:
    std::vector<std::string> interfaces;
    std::chrono::steady_clock::duration searchTimeout{std::chrono::seconds(4)};
};
} // namespace hueplusplus

#endif