/**
\file winHttpHandler.cpp
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

#include "include/winHttpHandler.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

winHttpHandler::winHttpHandler()
{
	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cerr << "winHttpHandler: Failed to open socket: " << WSAGetLastError() << std::endl;
		throw(std::runtime_error("winHttpHandler: Failed to open socket"));
	}
}

winHttpHandler::~winHttpHandler()
{
	WSACleanup();
}

std::string winHttpHandler::send(const std::string & msg, const std::string & adr, int port) const
{
	struct addrinfo *result = nullptr, *ptr = nullptr, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	int res = getaddrinfo(adr.c_str(), std::to_string(port).c_str(), &hints, &result);
	if (res != 0)
	{
		std::cerr << "winHttpHandler: getaddrinfo failed: " << res << std::endl;
		throw(std::runtime_error("winHttpHandler: getaddrinfo failed"));
	}

	SOCKET connect_socket = INVALID_SOCKET;


	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server
	connect_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

	if (connect_socket == INVALID_SOCKET)
	{
		freeaddrinfo(result);
		std::cerr << "winHttpHandler: Error at socket(): " << WSAGetLastError() << std::endl;
		throw(std::runtime_error("winHttpHandler: Error at socket()"));
	}

	// Connect to server.
	res = connect(connect_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
	if (res == SOCKET_ERROR)
	{
		closesocket(connect_socket);
		connect_socket = INVALID_SOCKET;
	}

	// Should really try the next address returned by getaddrinfo
	// if the connect call failed
	// But for this simple example we just free the resources
	// returned by getaddrinfo and print an error message

	freeaddrinfo(result);

	if (connect_socket == INVALID_SOCKET)
	{
		std::cerr << "winHttpHandler: Unable to connect to server!" << std::endl;
		throw(std::runtime_error("winHttpHandler: Unable to connect to server!"));
	}

	// Send an initial buffer
	res = ::send(connect_socket, msg.c_str(), msg.size(), 0);
	if (res == SOCKET_ERROR)
	{
		std::cerr << "winHttpHandler: send failed: " << WSAGetLastError() << std::endl;
		throw(std::runtime_error("winHttpHandler: send failed"));
	}

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	res = shutdown(connect_socket, SD_SEND);
	if (res == SOCKET_ERROR)
	{
		closesocket(connect_socket);
		std::cerr << "winHttpHandler: shutdown failed: " << WSAGetLastError() << std::endl;
		throw(std::runtime_error("winHttpHandler: shutdown failed"));
	}

	const int recvbuflen = 128;
	char recvbuf[recvbuflen];

	// Receive data until the server closes the connection
	std::string response;
	int received = 0;
	do
	{
		res = recv(connect_socket, recvbuf, recvbuflen, 0);
		if (res > 0)
		{
			std::cout << "winHttpHandler: Bytes received: " << res << std::endl;
			response.append(recvbuf, res);
		}
		else if (res == 0)
		{
			std::cout << "winHttpHandler: Connection closed " << std::endl;
		}
		else
		{
			std::cerr << "winHttpHandler: recv failed: " << WSAGetLastError() << std::endl;
			throw(std::runtime_error("winHttpHandler: recv failed"));
		}
	} while (res > 0);

	return response;
}

std::vector<std::string> winHttpHandler::sendMulticast(const std::string & msg, const std::string & adr, int port, int timeout) const
{
	struct addrinfo *result = nullptr, *ptr = nullptr, hints;
	SOCKADDR_IN source_sin, dest_sin;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	int res = getaddrinfo(adr.c_str(), std::to_string(port).c_str(), &hints, &result);
	if (res != 0)
	{
		std::cerr << "winHttpHandler: sendMulticast: getaddrinfo failed: " << res << std::endl;
		throw(std::runtime_error("winHttpHandler: sendMulticast: getaddrinfo failed"));
	}

	SOCKET connect_socket = INVALID_SOCKET;


	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr = result;

	// Create a SOCKET for connecting to server
	if ((connect_socket = socket(ptr->ai_family, ptr->ai_socktype, 0)) == INVALID_SOCKET)
	{
		freeaddrinfo(result);
		std::cerr << "winHttpHandler: sendMulticast: Error at socket(): " << WSAGetLastError() << std::endl;
		throw(std::runtime_error("winHttpHandler: sendMulticast: Error at socket()"));
	}

	// Fill out source socket's address information.
	source_sin.sin_family = AF_INET;
	source_sin.sin_port = htons(0);
	source_sin.sin_addr.s_addr = htonl(INADDR_ANY);

	// Associate the source socket's address with the socket, Sock.
	if (bind(connect_socket, (struct sockaddr FAR *) &source_sin, sizeof(source_sin)) == SOCKET_ERROR)
	{
		closesocket(connect_socket);
		std::cerr << "winHttpHandler: sendMulticast: Binding socket failed: " << WSAGetLastError() << std::endl;
		throw(std::runtime_error("winHttpHandler: sendMulticast: Binding socket failed"));
	}

	u_long sock_mode = 1;
	ioctlsocket(connect_socket, FIONBIO, &sock_mode);

	BOOL bOptVal = TRUE;
	setsockopt(connect_socket, SOL_SOCKET, SO_BROADCAST, (char *)&bOptVal, sizeof(bOptVal));

	// Set the Time-to-Live of the multicast.
	int iOptVal = 64;
	if (setsockopt(connect_socket, IPPROTO_IP, IP_MULTICAST_TTL, (char FAR *)&iOptVal, sizeof(int)) == SOCKET_ERROR)
	{
		closesocket(connect_socket);
		std::cerr << "winHttpHandler: sendMulticast: setsockopt failed: " << WSAGetLastError() << std::endl;
		throw(std::runtime_error("winHttpHandler: sendMulticast: setsockopt failed"));
	}

	// Fill out the desination socket's address information.
	dest_sin.sin_family = AF_INET;
	dest_sin.sin_port = htons(port);
	dest_sin.sin_addr.s_addr = inet_addr((const char*)ptr->ai_addr);

	// Send a message to the multicasting address.
	if (sendto(connect_socket, msg.c_str(), msg.size(), 0, (struct sockaddr FAR *) &dest_sin, sizeof(dest_sin)) == SOCKET_ERROR)
	{
		std::cerr << "winHttpHandler: sendMulticast: sendto failed: " << WSAGetLastError() << std::endl;
		closesocket(connect_socket);
		throw(std::runtime_error("winHttpHandler: sendMulticast: sendto failed"));
	}

	// shutdown the connection for sending since no more data will be sent
	// the client can still use the ConnectSocket for receiving data
	res = shutdown(connect_socket, SD_SEND);
	if (res == SOCKET_ERROR)
	{
		closesocket(connect_socket);
		std::cerr << "winHttpHandler: sendMulticast: shutdown failed: " << WSAGetLastError() << std::endl;
		throw(std::runtime_error("winHttpHandler: sendMulticast: shutdown failed"));
	}

	std::string response;
	const int recvbuflen = 2048;
	char recvbuf[recvbuflen] = {};
	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	while (std::chrono::steady_clock::now() - start < std::chrono::seconds(timeout))
	{
		res = recv(connect_socket, recvbuf, recvbuflen, 0);
		if (res > 0)
		{
			std::cout << "winHttpHandler: sendMulticast: Bytes received: " << res << std::endl;
			response.append(recvbuf, res);
		}
		else if (res == 0)
		{
			std::cout << "winHttpHandler: sendMulticast: Connection closed " << std::endl;
		}
		else
		{
			// No exception here due to non blocking socket
			//std::cerr << "sendMulticast: recv failed: " << WSAGetLastError() << std::endl;
			//throw(std::runtime_error("recv failed"));
		}
	}
	closesocket(connect_socket);	// Is this needed?

	// construct return vector
	std::vector<std::string> returnString;
	size_t pos = response.find("\r\n\r\n");
	size_t prevpos = 0;
	while (pos != std::string::npos)
	{
		returnString.push_back(response.substr(prevpos, pos - prevpos));
		pos += 4;
		prevpos = pos;
		pos = response.find("\r\n\r\n", pos);
	}

	return returnString;
}
