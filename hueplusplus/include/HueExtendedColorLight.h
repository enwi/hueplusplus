#ifndef _HUE_EXTENDED_COLOR_LIGHT
#define _HUE_EXTENDED_COLOR_LIGHT

#include "HueColorLight.h"
// supports same as Color light, but which supports additional setting of color temperature
class HueExtendedColorLight : public HueColorLight
{
	friend class Hue;

public:
	//! Fucntion that sets the color temperature of this light in mired. Ranging from 153 to 500.
	//! \param mired Unsigned int that specifies the color temperature in Mired
	//! \param transistion Optional parameter to set the transition from current state to new standard is 4 = 400ms
	//! \return Bool that is true on success
	virtual bool setColorTemperature(unsigned int mired, uint8_t transistion = 4);

	//! Function that lets the light perform one breath cycle in specified color temperature.
	//! It uses this_thread::sleep_for to accomodate for the time an \ref alert() needs
	//! \param mired Color temperature in mired
	//! \return Bool that is true on success
	bool alertTemperature(unsigned int mired);

	//! Function that lets the light perform one breath cycle in specified color.
	//! It uses this_thread::sleep_for to accomodate for the time an \ref alert() needs
	//! \param hue uint16_t that specifies the hue
	//! \param sat uint8_t that specifies the saturation
	//! \return Bool that is true on success
	bool alertHueSaturation(uint16_t hue, uint8_t sat);

	//! Function that lets the light perform one breath cycle in specified color.
	//! It uses this_thread::sleep_for to accomodate for the time an \ref alert() needs
	//! \param x float that specifies the x coordinate in CIE
	//! \param y float that specifies the y coordinate in CIE
	//! \return Bool that is true on success
	bool alertXY(float x, float y);

	//! Function that lets the light perform one breath cycle in specified color.
	//! It uses this_thread::sleep_for to accomodate for the time an \ref alert() needs
	//! \param r uint8_t that specifies the red color percentage
	//! \param g uint8_t that specifies the green color percentage
	//! \param b uint8_t that specifies the blue color percentage
	//! \return Bool that is true on success
	bool alertRGB(uint8_t r, uint8_t g, uint8_t b);

protected:
	//! protected ctor that is used by \ref Hue class.
	//! \param ip String that specifies the ip of the Hue bridge
	//! \param username String that specifies the username used to control the bridge
	//! \param id Integer that specifies the id of this light
	HueExtendedColorLight(const std::string& ip, const std::string& username, int id) : HueColorLight(ip, username, id) {};
};
#endif
