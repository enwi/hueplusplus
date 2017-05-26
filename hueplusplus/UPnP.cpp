#include "UPnP.h"
#include "HttpHandler.h"
#include <algorithm>
#include <iostream>

std::vector<std::pair<std::string, std::string>> UPnP::getDevices()
{
	// send UPnP M-Search request
	std::vector<std::string> foundDevices =  HttpHandler().sendMulticast("M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nMX: 5\r\nST: ssdp:all\r\n\r\n", "239.255.255.250", 1900, 5);

	std::vector<std::pair<std::string, std::string>> devices;

	// filter out devices
	for (const std::string &s : foundDevices)
	{
		std::pair<std::string, std::string> device;
		int start = s.find("LOCATION:") + 10;
		std::cout << "Found \"LOCATION:\" at " << start << std::endl;
		device.first = s.substr(start, s.find("\r\n", start)-start);
		start = s.find("SERVER:") + 8;
		device.second = s.substr(start, s.find("\r\n", start) - start);
		devices.push_back(device);
	}

	// remove duplicates -> maybe include this in device filtering without the need of unique and stuff?
	devices.erase(std::unique(devices.begin(), devices.end()), devices.end());

	return devices;
}
