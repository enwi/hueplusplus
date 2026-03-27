/**
    \file MDnsWrapper.cpp
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
#include "hueplusplus/MDnsWrapper.h"

#include <chrono>
#include <iostream>

#include <errno.h>
#include <mdns.h>
#include <signal.h>
#include <stdio.h>

#ifdef _WIN32
#include <iphlpapi.h>
#include <winsock2.h>
#else
#include <ifaddrs.h>
#include <net/if.h>
#include <netdb.h>
#include <sys/time.h>
#endif

#include "hueplusplus/HueExceptionMacro.h"

static mdns_string_t ipv4_address_to_string(char* buffer, size_t capacity, const struct sockaddr_in* addr,
                                            size_t addrlen)
{
    char host[NI_MAXHOST] = {0};
    char service[NI_MAXSERV] = {0};
    int ret = getnameinfo((const struct sockaddr*)addr, (socklen_t)addrlen, host, NI_MAXHOST, service, NI_MAXSERV,
                          NI_NUMERICSERV | NI_NUMERICHOST);
    int len = 0;
    if (ret == 0)
    {
        if (addr->sin_port != 0)
            len = snprintf(buffer, capacity, "%s:%s", host, service);
        else
            len = snprintf(buffer, capacity, "%s", host);
    }
    if (len >= (int)capacity)
        len = (int)capacity - 1;
    mdns_string_t str;
    str.str = buffer;
    str.length = len;
    return str;
}

static mdns_string_t ipv6_address_to_string(char* buffer, size_t capacity, const struct sockaddr_in6* addr,
                                            size_t addrlen)
{
    char host[NI_MAXHOST] = {0};
    char service[NI_MAXSERV] = {0};
    int ret = getnameinfo((const struct sockaddr*)addr, (socklen_t)addrlen, host, NI_MAXHOST, service, NI_MAXSERV,
                          NI_NUMERICSERV | NI_NUMERICHOST);
    int len = 0;
    if (ret == 0)
    {
        if (addr->sin6_port != 0)
            len = snprintf(buffer, capacity, "[%s]:%s", host, service);
        else
            len = snprintf(buffer, capacity, "%s", host);
    }
    if (len >= (int)capacity)
        len = (int)capacity - 1;
    mdns_string_t str;
    str.str = buffer;
    str.length = len;
    return str;
}

static mdns_string_t ip_address_to_string(char* buffer, size_t capacity, const struct sockaddr* addr, size_t addrlen)
{
    if (addr->sa_family == AF_INET6)
        return ipv6_address_to_string(buffer, capacity, (const struct sockaddr_in6*)addr, addrlen);
    return ipv4_address_to_string(buffer, capacity, (const struct sockaddr_in*)addr, addrlen);
}

// Handles IPv4 and IPv6, return 0 on success
static int parseSocketAddress(const std::string& addrString, sockaddr_storage& sockaddr)
{
    addrinfo* pAddrInfo = nullptr;
    addrinfo hints {};
    hints.ai_family = AF_UNSPEC;
    // No DNS lookup, only return addresses if the family (IPv4/v6) is configured
    hints.ai_flags = AI_NUMERICHOST | AI_ADDRCONFIG;
    int res = getaddrinfo(addrString.c_str(), nullptr, &hints, &pAddrInfo);

    // Returned list is sorted in a certain order, use the first entry
    if (res != 0)
    {
        return res;
    }
    if (pAddrInfo != nullptr)
    {
        std::copy_n(reinterpret_cast<const char*>(pAddrInfo->ai_addr), pAddrInfo->ai_addrlen,
                    reinterpret_cast<char*>(&sockaddr));
        freeaddrinfo(pAddrInfo);
        return 0;
    }
    return -1;
}

namespace
{
struct MDnsSocket
{
    MDnsSocket(int s) : s(s) { }
    MDnsSocket(MDnsSocket&& rhs) noexcept : s(rhs.s) { rhs.s = 0; }
    ~MDnsSocket()
    {
        if (s > 0)
        {
            mdns_socket_close(s);
        }
    }
    int s;
};
} // namespace

static std::vector<MDnsSocket> openClientSockets(const std::vector<std::string>& interfaces)
{
    // Open one socket for each network interface and address family
    std::vector<MDnsSocket> sockets;

    int port = 0;

    if (!interfaces.empty())
    {
        for (const std::string& addrString : interfaces)
        {
            sockaddr_storage addrStorage;
            int res = parseSocketAddress(addrString, addrStorage);
            if (res == 0)
            {
                if (addrStorage.ss_family == AF_INET)
                {
                    const sockaddr_in* saddr = reinterpret_cast<sockaddr_in*>(&addrStorage);
                    int sock = mdns_socket_open_ipv4(saddr);
                    if (sock >= 0)
                    {
                        sockets.push_back(sock);
#ifdef HUEPLUSPLUS_DEBUG_MDNS
                        char buffer[128];
                        mdns_string_t addr
                            = ipv4_address_to_string(buffer, sizeof(buffer), saddr, sizeof(struct sockaddr_in));
                        printf("Local IPv4 address: %.*s\n", MDNS_STRING_FORMAT(addr));
#endif
                    }
                }
                else if (addrStorage.ss_family == AF_INET6)
                {
                    const sockaddr_in6* saddr = reinterpret_cast<sockaddr_in6*>(&addrStorage);
                    int sock = mdns_socket_open_ipv6(saddr);
                    if (sock >= 0)
                    {
                        sockets.push_back(sock);
#ifdef HUEPLUSPLUS_DEBUG_MDNS
                        char buffer[128];
                        mdns_string_t addr
                            = ipv6_address_to_string(buffer, sizeof(buffer), saddr, sizeof(struct sockaddr_in6));
                        printf("Local IPv6 address: %.*s\n", MDNS_STRING_FORMAT(addr));
#endif
                    }
                }
            }
            else
            {
                std::cerr << "Could not parse interface address: " << addrString << "\n";
            }
        }
        return sockets;
    }

    // No interface specified,
#if _WIN32
    // On Windows, binding to the any address does not seem to work, so we need to create a socket for each interface
    IP_ADAPTER_ADDRESSES* adapter_address = 0;
    ULONG address_size = 8000;
    std::vector<uint8_t> buffer;
    unsigned int ret;
    unsigned int retries = 4;
    do
    {
        buffer.resize(address_size);
        adapter_address = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(buffer.data());
        ret = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_ANYCAST, nullptr, adapter_address,
                                   &address_size);
        if (ret == ERROR_BUFFER_OVERFLOW)
        {
            buffer.clear();
            // address_size is updated to the required amount
        }
        else
        {
            break;
        }
    } while (retries-- > 0);

    if (ret != NO_ERROR)
    {
        std::cerr << "Failed to get network adapter addresses\n";
        return sockets;
    }

    for (PIP_ADAPTER_ADDRESSES adapter = adapter_address; adapter; adapter = adapter->Next)
    {
        if (adapter->TunnelType == TUNNEL_TYPE_TEREDO)
            continue;
        if (adapter->OperStatus != IfOperStatusUp)
            continue;

        for (IP_ADAPTER_UNICAST_ADDRESS* unicast = adapter->FirstUnicastAddress; unicast; unicast = unicast->Next)
        {
            if (unicast->Address.lpSockaddr->sa_family == AF_INET)
            {
                // Ignore localhost
                struct sockaddr_in* saddr = (struct sockaddr_in*)unicast->Address.lpSockaddr;
                if ((saddr->sin_addr.S_un.S_un_b.s_b1 != 127) || (saddr->sin_addr.S_un.S_un_b.s_b2 != 0)
                    || (saddr->sin_addr.S_un.S_un_b.s_b3 != 0) || (saddr->sin_addr.S_un.S_un_b.s_b4 != 1))
                {
                    saddr->sin_port = htons((unsigned short)port);
                    int sock = mdns_socket_open_ipv4(saddr);
                    if (sock >= 0)
                    {
                        sockets.push_back(sock);
#ifdef HUEPLUSPLUS_DEBUG_MDNS
                        char buffer[128];
                        mdns_string_t addr
                            = ipv4_address_to_string(buffer, sizeof(buffer), saddr, sizeof(struct sockaddr_in));
                        printf("Local IPv4 address: %.*s\n", MDNS_STRING_FORMAT(addr));
#endif
                    }
                }
            }
            else if (unicast->Address.lpSockaddr->sa_family == AF_INET6)
            {
                struct sockaddr_in6* saddr = (struct sockaddr_in6*)unicast->Address.lpSockaddr;
                // Ignore link-local addresses and localhost
                if (saddr->sin6_scope_id)
                    continue;
                static const unsigned char localhost[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
                static const unsigned char localhost_mapped[]
                    = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 0x7f, 0, 0, 1};
                if ((unicast->DadState == NldsPreferred) && memcmp(saddr->sin6_addr.s6_addr, localhost, 16)
                    && memcmp(saddr->sin6_addr.s6_addr, localhost_mapped, 16))
                {
                    saddr->sin6_port = htons((unsigned short)port);
                    int sock = mdns_socket_open_ipv6(saddr);
                    if (sock >= 0)
                    {
                        sockets.push_back(sock);
#ifdef HUEPLUSPLUS_DEBUG_MDNS
                        char buffer[128];
                        mdns_string_t addr
                            = ipv6_address_to_string(buffer, sizeof(buffer), saddr, sizeof(struct sockaddr_in6));
                        printf("Local IPv6 address: %.*s\n", MDNS_STRING_FORMAT(addr));
#endif
                    }
                }
            }
        }
    }

    return sockets;

#endif

    // For now, open one IPv6 and one IPv4 socket
    sockaddr_in saddr {};
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(0);
    saddr.sin_addr.s_addr = INADDR_ANY;
    int s = mdns_socket_open_ipv4(&saddr);
    if (s >= 0)
    {
        sockets.push_back(s);
    }

    sockaddr_in6 saddr6 {};
    saddr6.sin6_family = AF_INET;
    saddr6.sin6_port = htons(0);
    saddr6.sin6_addr = in6addr_any;
    s = mdns_socket_open_ipv6(&saddr6);
    if (s >= 0)
    {
        sockets.push_back(s);
    }

    return sockets;
}

// Callback handling parsing answers to queries sent
static int queryCallback(int sock, const struct sockaddr* from, size_t addrlen, mdns_entry_type_t entry,
                         uint16_t query_id, uint16_t rtype, uint16_t rclass, uint32_t ttl, const void* data,
                         size_t size, size_t name_offset, size_t name_length, size_t record_offset,
                         size_t record_length, void* user_data)
{
    (void)sizeof(sock);
    (void)sizeof(name_length);
    auto& serviceInfo = *static_cast<std::vector<hueplusplus::MDnsWrapper::ServiceInfo>*>(user_data);
    static char addrbuffer[64];
    static char entrybuffer[256];
    static char namebuffer[256];
    static mdns_record_txt_t txtbuffer[128];
    mdns_string_t fromaddrstr = ip_address_to_string(addrbuffer, sizeof(addrbuffer), from, addrlen);
    const char* entrytype = (entry == MDNS_ENTRYTYPE_ANSWER)
        ? "answer"
        : ((entry == MDNS_ENTRYTYPE_AUTHORITY) ? "authority" : "additional");
    mdns_string_t entrystr = mdns_string_extract(data, size, &name_offset, entrybuffer, sizeof(entrybuffer));
    if (serviceInfo.size() <= query_id)
    {
        serviceInfo.reserve(2 * serviceInfo.size());
        serviceInfo.resize(query_id + 1);
    }

    if (rtype == MDNS_RECORDTYPE_SRV)
    {
        // Record describes a service (reachable port)
        mdns_record_srv_t srv
            = mdns_record_parse_srv(data, size, record_offset, record_length, namebuffer, sizeof(namebuffer));
        serviceInfo[query_id].name = std::string(srv.name.str, srv.name.length);
        serviceInfo[query_id].port = srv.port;
        serviceInfo[query_id].priority = srv.priority;
        serviceInfo[query_id].weight = srv.weight;
    }
    else if (rtype == MDNS_RECORDTYPE_A)
    {
        // IPv4 address of a service
        struct sockaddr_in addr;
        mdns_record_parse_a(data, size, record_offset, record_length, &addr);
        mdns_string_t addrstr = ipv4_address_to_string(namebuffer, sizeof(namebuffer), &addr, sizeof(addr));
        serviceInfo[query_id].aRecords.push_back(std::string(addrstr.str, addrstr.length));
    }
    else if (rtype == MDNS_RECORDTYPE_AAAA)
    {
        // IPv6 address of a service
        struct sockaddr_in6 addr;
        mdns_record_parse_aaaa(data, size, record_offset, record_length, &addr);
        mdns_string_t addrstr = ipv6_address_to_string(namebuffer, sizeof(namebuffer), &addr, sizeof(addr));
        serviceInfo[query_id].aaaaRecords.push_back(std::string(addrstr.str, addrstr.length));
    }
    else if (rtype == MDNS_RECORDTYPE_TXT)
    {
        // Additional text record sent by the service
        size_t parsed = mdns_record_parse_txt(data, size, record_offset, record_length, txtbuffer,
                                              sizeof(txtbuffer) / sizeof(mdns_record_txt_t));
        for (size_t itxt = 0; itxt < parsed; ++itxt)
        {
            serviceInfo[query_id].textRecords[std::string(txtbuffer[itxt].key.str, txtbuffer[itxt].key.length)]
                = std::string(txtbuffer[itxt].value.str, txtbuffer[itxt].value.length);
        }
    }
    return 0;
}

// Send a mDNS query, based on mdns.c example
static std::vector<hueplusplus::MDnsWrapper::ServiceInfo> sendMdnsQuery(const mdns_query_t& query,
                                                                        std::chrono::steady_clock::duration timeout,
                                                                        const std::vector<std::string>& interfaces)
{
    auto sockets = openClientSockets(interfaces);
    int num_sockets = sockets.size();
    if (num_sockets <= 0)
    {
        throw hueplusplus::HueException(CURRENT_FILE_INFO, "Failed to open any client sockets\n");
    }
    std::vector<int> query_id(num_sockets);

    size_t capacity = 2048;
    std::vector<uint8_t> buffer = std::vector<uint8_t>(capacity);
    std::vector<hueplusplus::MDnsWrapper::ServiceInfo> data;
    void* user_data = &data;

    for (int isock = 0; isock < num_sockets; ++isock)
    {
        query_id[isock] = mdns_multiquery_send(sockets[isock].s, &query, 1, buffer.data(), capacity, 0);
        if (query_id[isock] < 0)
            std::cerr << "Failed to send mDNS query: " << strerror(errno) << "\n";
    }

    // Wait at most until the specified timeout
    auto finishTime = std::chrono::steady_clock::now() + timeout;
    int res;
    int records = 0;
    do
    {
        auto waitDuration = finishTime - std::chrono::steady_clock::now();
        struct timeval cTimeout;
        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(timeout);
        cTimeout.tv_sec = seconds.count();
        cTimeout.tv_usec = std::chrono::duration_cast<std::chrono::microseconds>(timeout - seconds).count();

        int nfds = 0;
        fd_set readfs;
        FD_ZERO(&readfs);
        for (int isock = 0; isock < num_sockets; ++isock)
        {
            if (sockets[isock].s >= nfds)
                nfds = sockets[isock].s + 1;
            FD_SET(sockets[isock].s, &readfs);
        }

        res = select(nfds, &readfs, 0, 0, &cTimeout);
        if (res > 0)
        {
            for (int isock = 0; isock < num_sockets; ++isock)
            {
                if (FD_ISSET(sockets[isock].s, &readfs))
                {
                    size_t rec = mdns_query_recv(sockets[isock].s, buffer.data(), buffer.size(), queryCallback,
                                                 user_data, query_id[isock]);
                    if (rec > 0)
                        records += rec;
                }
                FD_SET(sockets[isock].s, &readfs);
            }
        }
    } while (res > 0);

    for (int isock = 0; isock < num_sockets; ++isock)
        mdns_socket_close(sockets[isock].s);

    return data;
}

namespace hueplusplus
{

MDnsWrapper::MDnsWrapper(std::vector<std::string> interfaces, std::chrono::steady_clock::duration searchTimeout)
    : interfaces(std::move(interfaces)), searchTimeout(searchTimeout)
{ }

std::vector<MDnsWrapper::ServiceInfo> MDnsWrapper::getDevices(const std::string& serviceName)
{
    mdns_query_t query {};
    query.name = serviceName.c_str();
    query.type = MDNS_RECORDTYPE_PTR;
    query.length = serviceName.length();

    return sendMdnsQuery(query, searchTimeout, interfaces);
}
} // namespace hueplusplus