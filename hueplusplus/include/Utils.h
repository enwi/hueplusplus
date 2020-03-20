/**
    \file Utils.h
    Copyright Notice\n
    Copyright (C) 2020  Jan Rogall		- developer\n
    Copyright (C) 2020  Moritz Wirger	- developer\n

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

#ifndef _UTILS_H
#define _UTILS_H

#include "json/json.hpp"

namespace utils
{
    namespace detail
    {
        // Forward declaration
        template <typename KeyT, typename... Paths>
        nlohmann::json safeGetMemberHelper(const nlohmann::json& json, std::size_t index, Paths&&... otherPaths);

        inline nlohmann::json safeGetMemberHelper(const nlohmann::json& json) { return json; }

        template <typename KeyT, typename... Paths,
            std::enable_if_t<!std::is_integral<std::remove_reference_t<KeyT>>::value>* = nullptr>
            nlohmann::json safeGetMemberHelper(const nlohmann::json& json, KeyT&& key, Paths&&... otherPaths)
        {
            auto memberIt = json.find(std::forward<KeyT>(key));
            if (memberIt == json.end())
            {
                return nullptr;
            }
            return safeGetMemberHelper(*memberIt, std::forward<Paths>(otherPaths)...);
        }

        // Needs to be after the other safeGetMemberHelper, otherwise another forward declaration is needed
        template <typename... Paths>
        nlohmann::json safeGetMemberHelper(const nlohmann::json& json, std::size_t index, Paths&&... otherPaths)
        {
            if (!json.is_array() || json.size() <= index)
            {
                return nullptr;
            }
            return safeGetMemberHelper(json[index], std::forward<Paths>(otherPaths)...);
        }
    }

    //! \brief Function for validating that a request was executed correctly
    //!
    //! \param request The request that was sent initially
    //! \param reply The reply that was received
    //! \param lightId The identifier of the light
    //! \return True if request was executed correctly
    bool validateReplyForLight(const nlohmann::json& request, const nlohmann::json& reply, int lightId);

    //! \brief Returns the object/array member or null if it does not exist
    //! 
    //! \param json The base json value
    //! \param paths Any number of child accesses (e.g. 0, "key" would access json[0]["key"])
    //! \returns The specified member or null if any intermediate object does not contain the specified child.
    template<typename... Paths>
    nlohmann::json safeGetMember(const nlohmann::json& json, Paths&&... paths)
    {
        return detail::safeGetMemberHelper(json, std::forward<Paths>(paths)...);
    }

} // namespace utils

#endif