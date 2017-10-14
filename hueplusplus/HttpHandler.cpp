/**
	\file HttpHandler.cpp
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

#include "include/HttpHandler.h"
#include <iostream>
#include <memory>
#include <stdexcept>
#include <stdio.h>		// printf, sprintf
#include <stdlib.h>		// exit
#include <unistd.h>		// read, write, close
#include <sys/socket.h> // socket, connect
#include <netinet/in.h> // struct sockaddr_in, struct sockaddr
#include <arpa/inet.h>
#include <netdb.h>		// struct hostent, gethostbyname
#include <string.h>
#include <chrono>

class SocketCloser {
	public: SocketCloser(int sockFd) :s(sockFd) {}
	~SocketCloser() { close(s); }
	private: int s;
};

std::string HttpHandler::send(const std::string & msg, const std::string & adr, int port)
{
	// create socket
	int socketFD = socket(AF_INET, SOCK_STREAM, 0);

	SocketCloser closeMySocket(socketFD);
	if (socketFD < 0)
	{
		std::cerr << "Failed to open socket\n";
		throw(std::runtime_error("Failed to open socket"));
	}

	// lookup ip address
	hostent	*server;
	server = gethostbyname(adr.c_str());
	if (server == NULL)
	{
		std::cerr << "Failed to find host with address " << adr << "\n";
		throw(std::runtime_error("Failed to find host"));
	}

	// fill in the structure
	sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

	// connect the socket
	if (connect(socketFD, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		std::cerr << "Failed to connect socket\n";
		throw(std::runtime_error("Failed to connect socket"));
	}

	// send the request
	size_t total = msg.length();
	ssize_t sent = 0;
	do
	{
		ssize_t bytes = write(socketFD, msg.c_str() + sent, total - sent);
		if (bytes < 0)
		{
			std::cerr << "Failed to write message to socket\n";
			throw(std::runtime_error("Failed to write message to socket"));
		}
		if (bytes == 0)
		{
			break;
		}
		if (bytes)
		{
			sent += bytes;
		}
	} while (sent < total);

	// receive the response
	std::string response;
	total = sizeof(response) - 1;
	int received = 0;
	char buffer[128] = {};
	do
	{
		ssize_t bytes = read(socketFD, buffer, 127);
		if (bytes < 0)
		{
			std::cerr << "Failed to read response from socket: " << errno << std::endl;
			throw(std::runtime_error("Failed to read response from socket"));
		}
		if (bytes == 0)
		{
			break;
		}
		if (bytes)
		{
			received += bytes;
			response.append(buffer, bytes);
		}
	} while (true);

	if (received == total)
	{
		std::cerr << "Failed to store complete response from socket\n";
		throw(std::runtime_error("Failed to store complete response from socket"));
	}

	return response;
}

std::string HttpHandler::sendGetHTTPBody(const std::string & msg, const std::string & adr, int port)
{
	std::string response = send(msg, adr, port);
	size_t start = response.find("\r\n\r\n");
	if (start == std::string::npos)
	{
		std::cerr << "Failed to find body in response\n";
		throw(std::runtime_error("Failed to find body in response"));
	}
	response.erase(0, start + 4);
	return response;
}

std::vector<std::string> HttpHandler::sendMulticast(const std::string & msg, const std::string & adr, int port, int timeout)
{
	hostent *server;			// host information
	sockaddr_in server_addr;	// server address

	//fill in the server's address and data
	memset((char*)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	// look up the address of the server given its name
	server = gethostbyname(adr.c_str());
	if (!server)
	{
		std::cerr << "Failed to obtain address of " << msg << "\n";
		throw(std::runtime_error("Failed to obtain address of host"));
	}

	// put the host's address into the server address structure
	memcpy( (void *)&server_addr.sin_addr, server->h_addr_list[0], server->h_length );

	// create the socket
	int socketFD = socket(AF_INET, SOCK_DGRAM, 0);
	SocketCloser closeMySendSocket(socketFD);
	if (socketFD < 0)
	{
		std::cerr << "Failed to open socket\n";
		throw(std::runtime_error("Failed to open socket"));
	}

	// send a message to the server
	if (sendto(socketFD, msg.c_str(), strlen(msg.c_str()), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		std::cerr << "Failed to send message\n";
		throw(std::runtime_error("Failed to send message"));
	}

	std::string response;
	char buffer[2048] = {};		// receive buffer
	ssize_t bytesReceived = 0;

	std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
	while (std::chrono::steady_clock::now() - start < std::chrono::seconds(timeout))
	{
		bytesReceived = recv(socketFD, &buffer, 2048, MSG_DONTWAIT);
		if (bytesReceived < 0)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
			{
				std::cerr << "Failed to read response from socket\n";
				throw(std::runtime_error("Failed to read response from socket"));
			}
			continue;
		}
		if (bytesReceived == 0)
		{
			break;
		}
		if (bytesReceived)
		{
			response.append(buffer, bytesReceived);
		}
	}

	// construct return vector
	std::vector<std::string> returnString;
	size_t pos = response.find("\r\n\r\n");
	size_t prevpos = 0;
	while (pos != std::string::npos)
	{
		returnString.push_back(response.substr(prevpos, pos-prevpos));
		pos += 4;
		prevpos = pos;
		pos = response.find("\r\n\r\n", pos);
	}

	return returnString;
}

std::string HttpHandler::sendHTTPRequest(std::string method, std::string uri, std::string content_type, std::string body, const std::string &adr, int port)
{
	std::string request;
	// Protocol reference: https://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
	// Request-Line
	request.append(method);				// Method
	request.append(" ");					// Separation
	request.append(uri);					// Request-URI
	request.append(" ");					// Separation
	request.append("HTTP/1.0");		// HTTP-Version
	request.append("\r\n");				// Ending
	// Entities
	request.append("Content-Type:");		// entity-header
	request.append(" ");								// Separation
	request.append(content_type);				// media-type
	request.append("\r\n");							// Entity ending
	request.append("Content-Length:");	// entity-header
	request.append(" ");								// Separation
	request.append(std::to_string(body.size()));	// length
	request.append("\r\n\r\n");		// Entity ending & Request-Line ending
	request.append(body);					// message-body
	request.append("\r\n\r\n");		// Ending

	return sendGetHTTPBody(request.c_str(), adr, port);
}

std::string HttpHandler::GETString(std::string uri, std::string content_type, std::string body, const std::string &adr, int port)
{
	return sendHTTPRequest("GET", uri, content_type, body, adr, port);
}

std::string HttpHandler::POSTString(std::string uri, std::string content_type, std::string body, const std::string &adr, int port)
{
	return sendHTTPRequest("POST", uri, content_type, body, adr, port);
}

std::string HttpHandler::PUTString(std::string uri, std::string content_type, std::string body, const std::string &adr, int port)
{
	return sendHTTPRequest("PUT", uri, content_type, body, adr, port);
}

Json::Value HttpHandler::GETJson(std::string uri, const Json::Value& body, const std::string &adr, int port)
{
	std::string response = GETString(uri, "application/json", body.toStyledString(), adr, port);

	std::string error;
	Json::Value result;
	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;
	std::unique_ptr<Json::CharReader> reader = std::unique_ptr<Json::CharReader>(builder.newCharReader());
	if (!reader->parse(response.c_str(), response.c_str() + response.length(), &result, &error))
	{
		std::cout << "Error while parsing JSON in function GETJson() of HttpHandler: " << error << std::endl;
		throw(std::runtime_error("Error while parsing JSON in function GETJson() of HttpHandler"));
	}
	return result;
}

Json::Value HttpHandler::POSTJson(std::string uri, const Json::Value& body, const std::string &adr, int port)
{
	std::string response = POSTString(uri, "application/json", body.toStyledString(), adr, port);

	std::string error;
	Json::Value result;
	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;
	std::unique_ptr<Json::CharReader> reader = std::unique_ptr<Json::CharReader>(builder.newCharReader());
	if (!reader->parse(response.c_str(), response.c_str() + response.length(), &result, &error))
	{
		std::cout << "Error while parsing JSON in function POSTJson() of HttpHandler: " << error << std::endl;
		throw(std::runtime_error("Error while parsing JSON in function POSTJson() of HttpHandler"));
	}
	return result;
}

Json::Value HttpHandler::PUTJson(std::string uri, const Json::Value& body, const std::string &adr, int port)
{
	std::string response = PUTString(uri, "application/json", body.toStyledString(), adr, port);

	std::string error;
	Json::Value result;
	Json::CharReaderBuilder builder;
	builder["collectComments"] = false;
	std::unique_ptr<Json::CharReader> reader = std::unique_ptr<Json::CharReader>(builder.newCharReader());
	if (!reader->parse(response.c_str(), response.c_str() + response.length(), &result, &error))
	{
		std::cout << "Error while parsing JSON in function PUTJson() of HttpHandler: " << error << std::endl;
		throw(std::runtime_error("Error while parsing JSON in function PUTJson() of HttpHandler"));
	}
	return result;
}
