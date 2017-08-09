/**
	\file HttpHandler.h
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

#ifndef _HTTPHANDLER_H
#define _HTTPHANDLER_H

#include <string>
#include <vector>

//! Class to handle http requests and multicast requests
class HttpHandler
{
public:
    //! \brief Function that sends a http request with the specified message and returns the response.
    //!
    //! It returns a string containing the response of the host.
    //! \param msg String that contains the request that is sent to the specified address
    //! \param adr String that contains an ip or hostname in dotted decimal notation like "192.168.2.1"
    //! \param port Optional integer that specifies the port to which the request is sent to. Default is 80
    //! \return String containing the response of the host
    std::string sendRequest(const std::string &msg, const std::string &adr, int port=80);

    //! \brief Function that sends a http request with the specified message and returns the body of the response.
    //!
    //! It returns a string containing only the body of the response of the host.
    //! \param msg String that contains the request that is sent to the specified address
    //! \param adr String that contains an ip or hostname in dotted decimal notation like "192.168.2.1"
    //! \param port Optional integer that specifies the port to which the request is sent. Default is 80
    //! \return String containing the body of the response of the host
    std::string sendRequestGetBody(const std::string &msg, const std::string &adr, int port = 80);

    //! \brief Function that sends a multicast request with the specified message.
    //!
    //! It returns a vector containing all responses the multicast request got back
    //! \param msg String that contains the request that is sent to the specified address
    //! \param adr Optional String that contains an ip or hostname in dotted decimal notation, default is "239.255.255.250"
    //! \param port Optional integer that specifies the port to which the request is sent. Default is 1900
    //! \param timeout Optional Integer that specifies the timeout of the request in seconds. Default is 5
    //! \return Vector containing strings of each answer received
    std::vector<std::string> sendMulticast(const std::string &msg, const std::string &adr = "239.255.255.250", int port = 1900, int timeout = 5);
};

#endif
