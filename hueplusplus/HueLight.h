/**
	\file HueLight.h
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

#ifndef _HUE_LIGHT_H
#define _HUE_LIGHT_H

#include "json/json.h"

/*enum ModelType
{
UNDEFINED,	// undefined model
LCT001,		// Hue bulb A19, Color Gamut B, ECL
LCT007,		// Hue bulb A19, Color Gamut B, ECL
LCT010,		// Hue bulb A19, Color Gamut C, ECL
LCT014,		// Hue bulb A19, Color Gamut C, ECL

LCT002,		// Hue Spot BR30, Color Gamut B, ECL
LCT003,		// Hue Spot GU10, Color Gamut B, ECL

LCT011,		// Hue BR30, Color Gamut C, ECL

LST001,		// Hue LightStrips, Color Gamut A, CL
LST002,		// Hue LightStrips Plus, Color Gamut C, ECL

LLC010,		// Hue Living Colors Iris, Color Gamut A, CL
LLC011,		// Hue Living Colors Bloom, Color Gamut A, CL
LLC012,		// Hue Living Colors Bloom, Color Gamut A, CL
LLC006,		// Living Colors Gen3 Iris, Color Gamut A, CL,				NO HUE FRIEND
LLC007,		// Living Colors Gen3 Bloom, Aura, Color Gamut A, CL,		NO HUE FRIEND
LLC013,		// Disney Living Colors, Color Gamut A, CL

LWB004,		// Hue A19 Lux, Color Gamut -, DL
LWB006,		// Hue A19 Lux, Color Gamut -, DL
LWB007,		// Hue A19 Lux, Color Gamut -, DL
LWB010,		// Hue A19 Lux, Color Gamut -, DL
LWB014,		// Hue A19 Lux, Color Gamut -, DL

LLM001,		// Color Light Module, Color Gamut B, ECL
LLM010,		// Color Temperature Module, Color Gamut 2200K-6500K, CTL
LLM011,		// Color Temperature Module, Color Gamut 2200K-6500K, CTL
LLM012,		// Color Temperature Module, Color Gamut 2200K-6500K, CTL

LTW001,		// Hue Spot BR30, Color Gamut 2200K-6500K, CTL
LTW004,		// Hue Spot BR30, Color Gamut 2200K-6500K, CTL
LTW013,		// Hue Spot BR30, Color Gamut 2200K-6500K, CTL
LTW014,		// Hue Spot BR30, Color Gamut 2200K-6500K, CTL

LLC020		// Hue Go, Color Gamut C, ECL
};*/

//! enum that specifies the color type of all Hue lights
enum ColorType
{
	UNDEFINED,				//!< ColorType for this light is unknown or undefined
	NONE,					//!< light has no specific ColorType
	GAMUT_A,
	GAMUT_B,
	GAMUT_C,
	TEMPERATURE,
	GAMUT_A_TEMPERATURE,
	GAMUT_B_TEMPERATURE,
	GAMUT_C_TEMPERATURE
};

// supports groups, scenes and on/off control
class HueLight
{
	friend class Hue;

public:
	//! virtual std dtor
	virtual ~HueLight() = default;

	//! virtual function that turns the light on.
	//! \param transistion Optional parameter to set the transition from current state to new standard is 4 = 400ms
	//! \return Bool that is true on success
	virtual bool On(uint8_t transistion = 4);

	//! virtual function that turns the light off.
	//! \param transistion Optional parameter to set the transition from current state to new standard is 4 = 400ms
	//! \return Bool that is true on success
	virtual bool Off(uint8_t transistion = 4);

	//! Function that lets the light perform one breath cycle.
	//! \return Bool that is true on success
	virtual bool alert();

	//! virtual function that returns the name of the light.
	//! \return String containig the name of the light
	virtual std::string getName();

	//! Function that converts Kelvin to Mired.
	//! \param kelvin Unsigned integer value in Kelvin
	//! \return Unsigned integer value in Mired
	unsigned int KelvinToMired(unsigned int kelvin);

	//! Function that converts Mired to Kelvin.
	//! \param mired Unsigned integer value in Mired
	//! \return Unsigned integer value in Kelvin
	unsigned int MiredToKelvin(unsigned int mired);

protected:
	//! protected ctor that is used by \ref Hue class.
	//! \param ip String that specifies the ip of the Hue bridge
	//! \param username String that specifies the username used to control the bridge
	//! \param id Integer that specifies the id of this light
	HueLight(const std::string& ip, const std::string& username, int id);

	//! virtual function that turns the light on without refreshing its state.
	//! \param transistion Optional parameter to set the transition from current state to new standard is 4 = 400ms
	//! \return Bool that is true on success
	virtual bool OnNoRefresh(uint8_t transistion = 4);

	//! virtual function that turns the light off without refreshing its state.
	//! \param transistion Optional parameter to set the transition from current state to new standard is 4 = 400ms
	//! \return Bool that is true on success
	virtual bool OffNoRefresh(uint8_t transistion = 4);

	//! utility function to send a put request to the light.
	//! \throws std::runtime_error if the reply could not be parsed
	//! \return The parsed reply
	Json::Value SendPutRequest(const Json::Value& request);

	//! virtual function that refreshes the \ref state of the light.
	virtual void refreshState();

protected:
	std::string ip;			//!< holds the ip of the light's base
	std::string username;	//!< holds the username of the light's base
	int id;					//!< holds the id of the light
	Json::Value state;		//!< holds the current state of the light updated by \ref refreshState
	ColorType colorType;	//!< holds the \ref ColorType of the light
};

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