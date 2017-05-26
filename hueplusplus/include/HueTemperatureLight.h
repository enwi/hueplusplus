#ifndef _HUE_TEMPAERATURE_LIGHT
#define _HUE_TEMPAERATURE_LIGHT

#include "HueDimmableLight.h"

// supports groups, scenes, on/off, dimming, and setting of color temperature
class HueTemperatureLight : public HueDimmableLight
{
	friend class Hue;

public:
	//! Fucntion that sets the color temperature of this light in mired. Ranging from 153 to 500
	//! \param mired Unsigned int that specifies the color temperature in Mired
	//! \param transistion Optional parameter to set the transition from current state to new standard is 4 = 400ms
	//! \return Bool that is true on success
	virtual bool setColorTemperature(unsigned int mired, uint8_t transistion = 4);

	//! Function that lets the light perform one breath cycle in specified color temperature.
	//! It uses this_thread::sleep_for to accomodate for the time an \ref alert() needs
	//! \param mired Color temperature in mired
	//! \return Bool that is true on success
	bool alertTemperature(unsigned int mired);

protected:
	//! protected ctor that is used by \ref Hue class.
	//! \param ip String that specifies the ip of the Hue bridge
	//! \param username String that specifies the username used to control the bridge
	//! \param id Integer that specifies the id of this light
	HueTemperatureLight(const std::string& ip, const std::string& username, int id) : HueDimmableLight(ip, username, id) {};
};
#endif
