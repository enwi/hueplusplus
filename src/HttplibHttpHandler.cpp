/**
    \file HttplibHttpHandler.cpp
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

#include "hueplusplus/HttplibHttpHandler.h"

#include "hueplusplus/HueExceptionMacro.h"

// Configuration for httplib (must be before include)
#define CPPHTTPLIB_MBEDTLS_SUPPORT 1
// Prevent duplicate function definitions if openssl is auto-detected
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
#undef CPPHTTPLIB_OPENSSL_SUPPORT
#endif
#include <httplib.h>

#include <mutex>

namespace
{
const char serverCert[] = R"(-----BEGIN CERTIFICATE-----
MIICMjCCAdigAwIBAgIUO7FSLbaxikuXAljzVaurLXWmFw4wCgYIKoZIzj0EAwIw
OTELMAkGA1UEBhMCTkwxFDASBgNVBAoMC1BoaWxpcHMgSHVlMRQwEgYDVQQDDAty
b290LWJyaWRnZTAiGA8yMDE3MDEwMTAwMDAwMFoYDzIwMzgwMTE5MDMxNDA3WjA5
MQswCQYDVQQGEwJOTDEUMBIGA1UECgwLUGhpbGlwcyBIdWUxFDASBgNVBAMMC3Jv
b3QtYnJpZGdlMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEjNw2tx2AplOf9x86
aTdvEcL1FU65QDxziKvBpW9XXSIcibAeQiKxegpq8Exbr9v6LBnYbna2VcaK0G22
jOKkTqOBuTCBtjAPBgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNV
HQ4EFgQUZ2ONTFrDT6o8ItRnKfqWKnHFGmQwdAYDVR0jBG0wa4AUZ2ONTFrDT6o8
ItRnKfqWKnHFGmShPaQ7MDkxCzAJBgNVBAYTAk5MMRQwEgYDVQQKDAtQaGlsaXBz
IEh1ZTEUMBIGA1UEAwwLcm9vdC1icmlkZ2WCFDuxUi22sYpLlwJY81Wrqy11phcO
MAoGCCqGSM49BAMCA0gAMEUCIEBYYEOsa07TH7E5MJnGw557lVkORgit2Rm1h3B2
sFgDAiEA1Fj/C3AN5psFMjo0//mrQebo0eKd3aWRx+pQY08mk48=
-----END CERTIFICATE-----
-----BEGIN CERTIFICATE-----
MIIBzDCCAXOgAwIBAgICEAAwCgYIKoZIzj0EAwIwPDELMAkGA1UEBhMCTkwxFDAS
BgNVBAoMC1NpZ25pZnkgSHVlMRcwFQYDVQQDDA5IdWUgUm9vdCBDQSAwMTAgFw0y
NTAyMjUwMDAwMDBaGA8yMDUwMTIzMTIzNTk1OVowPDELMAkGA1UEBhMCTkwxFDAS
BgNVBAoMC1NpZ25pZnkgSHVlMRcwFQYDVQQDDA5IdWUgUm9vdCBDQSAwMTBZMBMG
ByqGSM49AgEGCCqGSM49AwEHA0IABFfOO0jfSAUXGQ9kjEDzyBrcMQ3ItyA5krE+
cyvb1Y3xFti7KlAad8UOnAx0FBLn7HZrlmIwm1QnX0fK3LPM13mjYzBhMB0GA1Ud
DgQWBBTF1pSpsCASX/z0VHLigxU2CAaqoTAfBgNVHSMEGDAWgBTF1pSpsCASX/z0
VHLigxU2CAaqoTAPBgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBBjAKBggq
hkjOPQQDAgNHADBEAiAk7duT+IHbOGO4UUuGLAEpyYejGZK9Z7V9oSfnvuQ5BQIg
IYSgwwxHXm73/JgcU9lAM6c8Bmu3UE3kBIUwBs1qXFw=
-----END CERTIFICATE-----
)";
} // namespace

namespace hueplusplus
{

struct HttplibData
{
    std::map<std::pair<std::string, int>, httplib::Client> clients;
    std::string certificates {serverCert, std::size(serverCert)};
    std::mutex clientMutex;
};

HttplibHttpHandler::HttplibHttpHandler() : pData(std::make_unique<HttplibData>()) { }
HttplibHttpHandler::~HttplibHttpHandler() { }

void HttplibHttpHandler::setCACertificates(std::string certificates)
{
    pData->certificates = std::move(certificates);
}

void HttplibHttpHandler::connectBridge(const std::string& adr, int port, const std::string& bridgeId) const
{
    std::lock_guard<std::mutex> lock(pData->clientMutex);
    // To function properly with hostname verification, have the bridgeId in the URL
    // Then add a lookup to resolve the underlying IP
    std::string schemeHostPort;
    if (port == 443)
    {
        schemeHostPort = "https://";
        if (bridgeId.empty())
        {
            throw HueException(CURRENT_FILE_INFO,
                               "HTTPS connection requires specifying the bridge ID in the Bridge constructor");
        }
    }
    else
    {
        schemeHostPort = "http://";
    }
    if (!bridgeId.empty())
    {
        schemeHostPort += bridgeId;
    }
    else
    {
        schemeHostPort += adr;
    }
    schemeHostPort += ":";
    schemeHostPort += std::to_string(port);
    httplib::Client client(schemeHostPort);
    client.set_keep_alive(true);
#ifdef CPPHTTPLIB_WINDOWS_AUTOMATIC_ROOT_CERTIFICATES_UPDATE
    client.enable_windows_certificate_verification(false);
#endif
    if (!bridgeId.empty())
    {
        client.set_hostname_addr_map({{bridgeId, adr}});
    }
    client.set_ca_cert_store(httplib::tls::create_ca_store(pData->certificates.data(), pData->certificates.size()));
    client.enable_server_hostname_verification(true);
    auto key = std::make_pair(adr, port);
    pData->clients.erase(key);
    pData->clients.emplace(key, std::move(client));
}

std::string HttplibHttpHandler::sendHTTPRequest(const std::string& method, const std::string& uri,

                                                const std::string& contentType, const std::string& body,
                                                const std::string& adr, int port) const
{
    httplib::Request request;
    request.method = method;
    request.accept_content_types.push_back(contentType);
    request.path = uri;
    request.body = body;

    // TODO: This is not optimal, because it locks during the network request
    std::lock_guard<std::mutex> lock(pData->clientMutex);
    auto& client = pData->clients.at(std::make_pair(adr, port));
    httplib::Result result = client.send(request);
    if (!result)
    {
        // Connection error
        std::string error = httplib::to_string(result.error());
        throw hueplusplus::HueException(CURRENT_FILE_INFO, "Error while executing request: " + error);
    }
    if (result->status != 200)
    {
        std::cerr << "HTTP error: " << result->status << "\n" << result->body << "\n";
    }
    return result->body;
}

std::string HttplibHttpHandler::send(const std::string& msg, const std::string& adr, int port) const
{
    throw hueplusplus::HueException(CURRENT_FILE_INFO, "Not implemented, should not call send() directly");
}

std::vector<std::string> HttplibHttpHandler::sendMulticast(const std::string& msg, const std::string& adr, int port,
                                                           std::chrono::steady_clock::duration timeout) const
{
    throw hueplusplus::HueException(CURRENT_FILE_INFO, "Not implemented");
}
} // namespace hueplusplus
