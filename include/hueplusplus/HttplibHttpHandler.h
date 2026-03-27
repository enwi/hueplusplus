/**
    \file HttplibHttpHandler.h
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

#ifndef INCLUDE_HUEPLUSPLUS_HTTPLIBHTTPHANDLER_H
#define INCLUDE_HUEPLUSPLUS_HTTPLIBHTTPHANDLER_H

#include <string>
#include <string_view>
#include <vector>

#include "BaseHttpHandler.h"

namespace hueplusplus
{
//! Class to handle http requests and multicast requests by using the cpp-httplib library by yhirose
class HttplibHttpHandler : public BaseHttpHandler
{
public:
    HttplibHttpHandler();
    ~HttplibHttpHandler() override;

    //! \brief Set server root certificates in PEM format
    //!
    //! This can be used in case your bridge does not use the Signify root certificates.
    //! It overwrites the pre-configured root certificates, so concatenate all root certificates you want to allow.
    //! \param certificates String containing the certificate bundle in PEM format.
    void setCACertificates(std::string certificates);

    //! \brief Initialize connection to the specified bridge
    //!
    //! This is required to setup the SSL name verification and also to open a keepalive connection.
    //! For now, only one connection can be active at a time.
    //! \param adr Ip or hostname of the bridge
    //! \param port Port number of the service on the bridge, also used to detect https (port 443)
    //! \param bridgeId Bridge id of the specified bridge
    void connectBridge(const std::string& adr, int port, const std::string& bridgeId) const override;

    //! \brief Send a HTTP request with the given method to the specified host and return the body of the response.
    //!
    //! \param method HTTP method type e.g. GET, HEAD, POST, PUT, DELETE, ...
    //! \param uri Uniform Resource Identifier in the request
    //! \param contentType MIME type of the body data e.g. "text/html", "application/json", ...
    //! \param body Request body, may be empty
    //! \param adr Ip or hostname in dotted decimal notation like "192.168.2.1"
    //! \param port Optional port the request is sent to, default is 80
    //! \return Body of the response of the host
    //! \throws std::system_error when system or socket operations fail
    //! \throws HueException when response contained no body
    std::string sendHTTPRequest(const std::string& method, const std::string& uri, const std::string& contentType,
                                const std::string& body, const std::string& adr, int port = 80) const override;

    //! \brief Function that sends a given message to the specified host and
    //! returns the response.
    //!
    //! \param msg String that contains the message that is sent to the specified
    //! address \param adr String that contains an ip or hostname in dotted
    //! decimal notation like "192.168.2.1" \param port Optional integer that
    //! specifies the port to which the request is sent to. Default is 80 \return
    //! String containing the response of the host
    virtual std::string send(const std::string& msg, const std::string& adr, int port = 80) const override;

    //! \brief Function that sends a multicast request with the specified message.
    //!
    //! \param msg String that contains the request that is sent to the specified
    //! address \param adr Optional String that contains an ip or hostname in
    //! dotted decimal notation, default is "239.255.255.250" \param port Optional
    //! integer that specifies the port to which the request is sent. Default is
    //! 1900 \param timeout Optional The timeout of the
    //! request. Default is 5 seconds \return Vector containing strings of each
    //! answer received
    std::vector<std::string>
    sendMulticast(const std::string& msg, const std::string& adr = "239.255.255.250", int port = 1900,
                  std::chrono::steady_clock::duration timeout = std::chrono::seconds(5)) const override;

private:
    std::unique_ptr<struct HttplibData> pData;
};
} // namespace hueplusplus

#endif
