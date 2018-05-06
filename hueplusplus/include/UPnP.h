/**
	\file UPnP.h
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

#ifndef _UPNP_H
#define _UPNP_H

#include <memory>
#include <string>
#include <vector>

#include "IHttpHandler.h"

//! Class that looks for UPnP devices using an m-search package
class UPnP
{
public:
	//! \brief Function that searches for UPnP devices and returns all found ones.
	//!
	//! It does it by sending an m-search packet and waits for all responses.
	//! Since responses can be received multiple times this function conveniently removes all duplicates.
	//! \param handler HttpHandler of type \ref IHttpHandler for communication with the bridge
	//! \return A vector containing pairs of address and name of all found devices
	std::vector<std::pair<std::string, std::string>> getDevices(std::shared_ptr<const IHttpHandler> handler);
};

#endif
