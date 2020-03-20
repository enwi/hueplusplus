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
    //! \brief Function for validating that a request was executed correctly
    //!
    //! \param request The request that was sent initially
    //! \param reply The reply that was received
    //! \param lightId The identifier of the light
    //! \return True if request was executed correctly
    bool validateReplyForLight(nlohmann::json request, nlohmann::json reply, int lightId);
} // namespace utils

#endif