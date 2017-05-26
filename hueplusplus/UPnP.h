#ifndef _UPNP_H
#define _UPNP_H

#include <string> 
#include <vector>

class UPnP
{
public:
	std::vector<std::pair<std::string, std::string>> getDevices();
};

#endif