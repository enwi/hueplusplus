#ifndef _HUE_DIMMABLE_LIGHT_H
#define _HUE_DIMMABLE_LIGHT_H
#include "HueLight.h"

// supports groups, scenes, on/off and dimming
class HueDimmableLight : public HueLight
{
	friend class Hue;

public:
	//! virtual function that sets the brightness of this light. Ranging from 0=off to 255=fully on
	//! \param bri Unsigned int that specifies the brightness
	//! \param transistion Optional parameter to set the transition from current state to new standard is 4 = 400ms
	//! \return Bool that is true on success
	virtual bool setBrightness(unsigned int bri, uint8_t transistion = 4);

protected:
	//! protected ctor that is used by \ref Hue class.
	//! \param ip String that specifies the ip of the Hue bridge
	//! \param username String that specifies the username used to control the bridge
	//! \param id Integer that specifies the id of this light
	HueDimmableLight(const std::string& ip, const std::string& username, int id) : HueLight(ip, username, id) {};
};
#endif
