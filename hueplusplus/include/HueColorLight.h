#ifndef _HUE_COLOR_LIGHT
#define _HUE_COLOR_LIGHT

#include "HueDimmableLight.h"

// xy beats ct beats hue/sat.
// supports groups, scenes, on / off, dimming and color control(hue / saturation, enhanced hue, color loop and XY)
class HueColorLight : public HueDimmableLight
{
	friend class Hue;

public:
	//! Function to set the color of this light with specified hue. Ranging from 0 to 65535, whereas 65535 and 0 are red, 25500 is green and 46920 is blue.
	//! \param hue uint16_t that specifies the hue
	//! \param transistion Optional parameter to set the transition from current state to new standard is 4 = 400ms
	//! \return Bool that is true on success
	virtual bool setColorHue(uint16_t hue, uint8_t transistion = 4);

	//! Function to set the saturation of color of this light with specified saturation. Ranging from 0 to 254, whereas 0 is least saturated (white) and 254 is most saturated.
	//! \param sat uint8_t that specifies the saturation
	//! \param transistion Optional parameter to set the transition from current state to new standard is 4 = 400ms
	//! \return Bool that is true on success
	virtual bool setColorSaturation(uint8_t sat, uint8_t transistion = 4);

	//! Function to set the color of this light with specified hue and saturation.
	//! \param hue uint16_t that specifies the hue
	//! \param sat uint8_t that specifies the saturation
	//! \param transistion Optional parameter to set the transition from current state to new standard is 4 = 400ms.
	//! \return Bool that is true on success
	virtual bool setColorHueSaturation(uint16_t hue, uint8_t sat, uint8_t transition = 4);

	//! Function to set the color of this light in CIE with specified x y. Where x and y are ranging from 0 to 1.
	//! \param x float that specifies the x coordinate in CIE
	//! \param y float that specifies the y coordinate in CIE
	//! \param transistion Optional parameter to set the transition from current state to new standard is 4 = 400ms
	//! \return Bool that is true on success
	virtual bool setColorXY(float x, float y, uint8_t transistion = 4);

	//! Function to set the color of this light with red green and blue values. Where red, green and blue are ranging from 0 to 255.
	//! \param r uint8_t that specifies the red color percentage
	//! \param g uint8_t that specifies the green color percentage
	//! \param b uint8_t that specifies the blue color percentage
	//! \param transistion Optional parameter to set the transition from current state to new standard is 4 = 400ms
	//! \return Bool that is true on success
	virtual bool setColorRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t transistion = 4);

	//! Function to enable colorloop effect. The colorloop effect will loop through all colors on current hue and saturation levels.
	//! Notice that none of the setter functions check for this and the colorloop can only be disabled via this function.
	//! or by simply calling Off()/OffNoRefresh() and then On()/OnNoRefresh(), 
	//! so you could alternatively call Off() and then use any of the setter functions
	//! \param on bool that enables this feature when true and disables it when false
	//! \return Bool that is true on success
	virtual bool setColorLoop(bool on);

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
	HueColorLight(const std::string& ip, const std::string& username, int id) : HueDimmableLight(ip, username, id) {};

	/*private:
	bool pointInTriangle(float pointx, float pointy, float x0, float y0, float x1, float y1, float x2, float y2); // currently unused because Hue bridge handles this*/
};
#endif
