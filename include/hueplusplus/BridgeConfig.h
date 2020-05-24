/**
    \file BridgeConfig.h
    Copyright Notice\n
    Copyright (C) 2020  Jan Rogall		- developer\n

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

#include <string>
#include <vector>

#include "APICache.h"
#include "TimePattern.h"

namespace hueplusplus
{
struct Version
{
    int major;
    int minor;
    int patch;
};

struct WhitelistedUser
{
    std::string key;
    std::string name;
    time::AbsoluteTime lastUsed;
    time::AbsoluteTime created;
};

class BridgeConfig
{
public:
    BridgeConfig(std::shared_ptr<APICache> baseCache, std::chrono::steady_clock::duration refreshDuration);

    void refresh();

    std::vector<WhitelistedUser> getWhitelistedUsers() const;
    void removeUser(const std::string& userKey);

    bool getLinkButton() const;
    void pressLinkButton();

    void touchLink();

    std::string getMACAddress() const;
    time::AbsoluteTime getUTCTime() const;
    std::string getTimezone() const;

protected:
    BridgeConfig(const BridgeConfig&) = default;
    BridgeConfig(BridgeConfig&&) = default;
    BridgeConfig& operator=(const BridgeConfig&) = default;
    BridgeConfig& operator=(BridgeConfig&&) = default;

private:
    APICache cache;
};
} // namespace hueplusplus