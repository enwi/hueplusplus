/**
	\file IHttpHandler.h
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

#ifndef _IHTTPHANDLER_H
#define _IHTTPHANDLER_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "json/json.h"

//! Abstract class for classes that handle http requests and multicast requests
class IHttpHandler
{
public:
    //! \brief Virtual dtor
    virtual ~IHttpHandler() = default;

    //! \brief Virtual function that should send a given message to a specified host and return the response.
    //!
    //! \param msg String that contains the message that should be sent to the specified address
    //! \param adr String that contains an ip or hostname in dotted decimal notation like "192.168.2.1"
    //! \param port Optional integer that specifies the port to which the request should be sent to. Default is 80
    //! \return String containing the response of the host
    virtual std::string send(const std::string &msg, const std::string &adr, int port=80) const = 0;

    //! \brief Virtual function that should given message to a specified host and return the body of the response.
    //!
    //! Note if no body is found a runtime error is thrown!
    //! \param msg String that contains the message that should sent to the specified address
    //! \param adr String that contains an ip or hostname in dotted decimal notation like "192.168.2.1"
    //! \param port Optional integer that specifies the port to which the request should be sent. Default is 80
    //! \return String containing the body of the response of the host
    virtual std::string sendGetHTTPBody(const std::string &msg, const std::string &adr, int port = 80) const = 0;

    //! \brief Virtual function that should send a multicast request with a specified message.
    //!
    //! \param msg String that contains the request that should be sent to the specified address
    //! \param adr Optional String that contains an ip or hostname in dotted decimal notation, default is "239.255.255.250"
    //! \param port Optional integer that specifies the port to which the request should be sent. Default is 1900
    //! \param timeout Optional Integer that specifies the timeout of the request in seconds. Default is 5
    //! \return Vector containing strings of each answer received
    virtual std::vector<std::string> sendMulticast(const std::string &msg, const std::string &adr = "239.255.255.250", int port = 1900, int timeout = 5) const = 0;

    //! \brief Virtual function that should send a HTTP request with the given method to the specified host and return the body of the response.
    //!
    //! Note body can also be left empty!
    //! \param method String that contains the HTTP method type e.g. GET, HEAD, POST, PUT, DELETE, ...
    //! \param uri String that contains the uniform resource identifier
    //! \param content_type String that contains the type(MIME) of the body data e.g. "text/html", "application/json", ...
    //! \param body String that contains the data of the request
    //! \param adr String that contains an ip or hostname in dotted decimal notation like "192.168.2.1"
    //! \param port Optional integer that specifies the port to which the request is sent to. Default is 80
    //! \return String containing the body of the response of the host
    virtual std::string sendHTTPRequest(std::string method, std::string uri, std::string content_type, std::string body, const std::string &adr, int port=80) const = 0;

    //! \brief Virtual function that should send a HTTP GET request to the specified host and return the body of the response.
    //!
    //! Note body can also be left empty!
    //! \param uri String that contains the uniform resource identifier
    //! \param content_type String that contains the type(MIME) of the body data e.g. "text/html", "application/json", ...
    //! \param body String that contains the data of the request
    //! \param adr String that contains an ip or hostname in dotted decimal notation like "192.168.2.1"
    //! \param port Optional integer that specifies the port to which the request is sent to. Default is 80
    //! \return String containing the body of the response of the host
    virtual std::string GETString(std::string uri, std::string content_type, std::string body, const std::string &adr, int port=80) const = 0;

    //! \brief Virtual function that should send a HTTP POST request to the specified host and returns the body of the response.
    //!
    //! Note body can also be left empty!
    //! \param uri String that contains the uniform resource identifier
    //! \param content_type String that contains the type(MIME) of the body data e.g. "text/html", "application/json", ...
    //! \param body String that contains the data of the request
    //! \param adr String that contains an ip or hostname in dotted decimal notation like "192.168.2.1"
    //! \param port Optional integer that specifies the port to which the request is sent to. Default is 80
    //! \return String containing the body of the response of the host
    virtual std::string POSTString(std::string uri, std::string content_type, std::string body, const std::string &adr, int port=80) const = 0;

    //! \brief Virtual function that should send a HTTP PUT request to the specified host and return the body of the response.
    //!
    //! Note body can also be left empty!
    //! \param uri String that contains the uniform resource identifier
    //! \param content_type String that contains the type(MIME) of the body data e.g. "text/html", "application/json", ...
    //! \param body String that contains the data of the request
    //! \param adr String that contains an ip or hostname in dotted decimal notation like "192.168.2.1"
    //! \param port Optional integer that specifies the port to which the request is sent to. Default is 80
    //! \return String containing the body of the response of the host
    virtual std::string PUTString(std::string uri, std::string content_type, std::string body, const std::string &adr, int port=80) const = 0;

    //! \brief Virtual function that should send a HTTP DELETE request to the specified host and return the body of the response.
    //!
    //! Note body can also be left empty!
    //! \param uri String that contains the uniform resource identifier
    //! \param content_type String that contains the type(MIME) of the body data e.g. "text/html", "application/json", ...
    //! \param body String that contains the data of the request
    //! \param adr String that contains an ip or hostname in dotted decimal notation like "192.168.2.1"
    //! \param port Optional integer that specifies the port to which the request is sent to. Default is 80
    //! \return String containing the body of the response of the host
    virtual std::string DELETEString(std::string uri, std::string content_type, std::string body, const std::string &adr, int port=80) const = 0;

    //! \brief Virtual function that should send a HTTP GET request to the specified host and return the body of the response.
    //!
    //! Note body can also be left empty!
    //! \param uri String that contains the uniform resource identifier
    //! \param body Json::Value that contains the data of the request
    //! \param adr String that contains an ip or hostname in dotted decimal notation like "192.168.2.1"
    //! \param port Optional integer that specifies the port to which the request is sent to. Default is 80
    //! \return Json::Value containing the parsed body of the response of the host
    virtual Json::Value GETJson(std::string uri, const Json::Value& body, const std::string &adr, int port=80) const = 0;

    //! \brief Virtual function that should send a HTTP POST request to the specified host and return the body of the response.
    //!
    //! Note body can also be left empty!
    //! \param uri String that contains the uniform resource identifier
    //! \param body Json::Value that contains the data of the request
    //! \param adr String that contains an ip or hostname in dotted decimal notation like "192.168.2.1"
    //! \param port Optional integer that specifies the port to which the request is sent to. Default is 80
    //! \return Json::Value containing the parsed body of the response of the host
    virtual Json::Value POSTJson(std::string uri, const Json::Value& body, const std::string &adr, int port=80) const = 0;

    //! \brief Virtual function that should send a HTTP PUT request to the specified host and return the body of the response.
    //!
    //! Note body can also be left empty!
    //! \param uri String that contains the uniform resource identifier
    //! \param body Json::Value that contains the data of the request
    //! \param adr String that contains an ip or hostname in dotted decimal notation like "192.168.2.1"
    //! \param port Optional integer that specifies the port to which the request is sent to. Default is 80
    //! \return Json::Value containing the parsed body of the response of the host
    virtual Json::Value PUTJson(std::string uri, const Json::Value& body, const std::string &adr, int port=80) const = 0;

    //! \brief Virtual function that should send a HTTP DELETE request to the specified host and return the body of the response.
    //!
    //! Note body can also be left empty!
    //! \param uri String that contains the uniform resource identifier
    //! \param body Json::Value that contains the data of the request
    //! \param adr String that contains an ip or hostname in dotted decimal notation like "192.168.2.1"
    //! \param port Optional integer that specifies the port to which the request is sent to. Default is 80
    //! \return Json::Value containing the parsed body of the response of the host
    virtual Json::Value DELETEJson(std::string uri, const Json::Value& body, const std::string &adr, int port=80) const = 0;

};

#endif
