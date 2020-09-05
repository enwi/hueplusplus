/**
    \file Rule.h
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

#ifndef INCLUDE_HUEPLUSPLUS_RULE_H
#define INCLUDE_HUEPLUSPLUS_RULE_H

#include <string>

#include "APICache.h"
#include "Action.h"
#include "Condition.h"
#include "HueCommandAPI.h"
#include "TimePattern.h"

namespace hueplusplus
{

class Rule
{
public:
    //! \brief Creates rule with id
    //! \param id Rule id in the bridge
    //! \param commands HueCommandAPI for requests
    //! \param refreshDuration Time between refreshing the cached state.
    Rule(int id, const HueCommandAPI& commands, std::chrono::steady_clock::duration refreshDuration);

    //! \brief Refreshes internal cached state.
    //! \param force \c true forces a refresh, regardless of how long the last refresh was ago.
    //! \c false to only refresh when enough time has passed (needed e.g. when calling only const methods).
    //! \throws std::system_error when system or socket operations fail
    //! \throws HueException when response contained no body
    //! \throws HueAPIResponseException when response contains an error
    //! \throws nlohmann::json::parse_error when response could not be parsed
    void refresh(bool force = false);

    //! \brief Get rule identifier
    int getId() const;

    //! \brief Get rule name
    std::string getName() const;

    //! \brief Get created time
    time::AbsoluteTime getCreated() const;

    //! \brief Get time the rule was last triggered
    time::AbsoluteTime getLastTriggered() const;

    //! \brief Get the number of times the rule was triggered
    int getTimesTriggered() const;

    //! \brief Get whether rule is enabled or disabled
    bool isEnabled() const;

    //! \brief Get user that created or last changed the rule.
    std::string getOwner() const;

    std::vector<Condition> getConditions() const;
    std::vector<Action> getActions() const;

private:
    int id;
    APICache state;
};

} // namespace hueplusplus

#endif
