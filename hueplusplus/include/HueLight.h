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

#include <memory>

#include "BrightnessStrategy.h"
#include "ColorHueStrategy.h"
#include "ColorTemperatureStrategy.h"
#include "IHttpHandler.h"

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

//! enum that specifies the color type of all HueLights
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

//!
//! Class for Hue Light fixtures
//!
class HueLight
{
	friend class Hue;
	friend class SimpleBrightnessStrategy;
	friend class SimpleColorHueStrategy;
	friend class ExtendedColorHueStrategy;
	friend class SimpleColorTemperatureStrategy;
	friend class ExtendedColorTemperatureStrategy;

public:
	//! \brief std dtor
	~HueLight() = default;

	//! \brief Function that turns the light on.
	//!
	//! \param transition Optional parameter to set the transition from current state to new, standard is 4 = 400ms
	//! \return Bool that is true on success
	bool On(uint8_t transition = 4);

	//! \brief Function that turns the light off.
	//!
	//! \param transition Optional parameter to set the transition from current state to new, standard is 4 = 400ms
	//! \return Bool that is true on success
	bool Off(uint8_t transition = 4);

	//! \brief Function that returns the name of the light.
	//!
	//! \return String containig the name of the light
	std::string getName();

	//! \brief Function that returns the color type of the light.
	//!
	//! \return ColorType containig the color type of the light
	ColorType getColorType();

	//! \brief Function that converts Kelvin to Mired.
	//!
	//! \param kelvin Unsigned integer value in Kelvin
	//! \return Unsigned integer value in Mired
	unsigned int KelvinToMired(unsigned int kelvin);

	//! \brief Function that converts Mired to Kelvin.
	//!
	//! \param mired Unsigned integer value in Mired
	//! \return Unsigned integer value in Kelvin
	unsigned int MiredToKelvin(unsigned int mired);

	//! \brief Function that sets the brightness of this light.
	//!
	//! Notice the brightness will only be set if the light has a reference to a specific \ref BrightnessStrategy.
	//! The brightness can range from 0 = off to 255 = fully lit.
	//! \param bri Unsigned int that specifies the brightness
	//! \param transition Optional parameter to set the transition from current state to new, standard is 4 = 400ms
	//! \return Bool that is true on success
	bool setBrightness(unsigned int bri, uint8_t transition = 4)
	{
		if (brightnessStrategy)
		{
			return brightnessStrategy->setBrightness(bri, transition, *this);
	 	}
		return false;
	};

	//! \brief Fucntion that sets the color temperature of this light in mired.
	//!
	//! Notice the color temperature will only be set if the light has a reference to a specific \ref ColorTemperatureStrategy.
	//! The color temperature can range from 153 to 500.
	//! \param mired Unsigned int that specifies the color temperature in Mired
	//! \param transition Optional parameter to set the transition from current state to new, standard is 4 = 400ms
	//! \return Bool that is true on success
	bool setColorTemperature(unsigned int mired, uint8_t transition = 4)
	{
		if (colorTemperatureStrategy)
		{
			return colorTemperatureStrategy->setColorTemperature(mired, transition, *this);
		}
		return false;
	};

	//! \brief Function to set the color of this light with specified hue.
	//!
	//! Notice the color will only be set if the light has a reference to a specific \ref ColorHueStrategy.
	//! The hue can range from 0 to 65535, whereas 65535 and 0 are red, 25500 is green and 46920 is blue.
	//! \param hue uint16_t that specifies the hue
	//! \param transition Optional parameter to set the transition from current state to new, standard is 4 = 400ms
	//! \return Bool that is true on success
	bool setColorHue(uint16_t hue, uint8_t transition = 4)
	{
		if(colorHueStrategy)
		{
			return colorHueStrategy->setColorHue(hue, transition, *this);
		}
		return false;
	};

	//! \brief Function to set the color of this light with specified saturation.
	//!
	//! Notice the color will only be set if the light has a reference to a specific \ref ColorHueStrategy.
	//! The saturation can range from 0 to 254, whereas 0 is least saturated (white) and 254 is most saturated.
	//! \param sat uint8_t that specifies the saturation
	//! \param transition Optional parameter to set the transition from current state to new, standard is 4 = 400ms
	//! \return Bool that is true on success
	bool setColorSaturation(uint8_t sat, uint8_t transition = 4)
	{
		if(colorHueStrategy)
		{
			return colorHueStrategy->setColorSaturation(sat, transition, *this);
		}
		return false;
	};

	//! \brief Function to set the color of this light with specified hue and saturation.
	//!
	//! Notice the color will only be set if the light has a reference to a specific \ref ColorHueStrategy.
	//! \param hue uint16_t that specifies the hue
	//! \param sat uint8_t that specifies the saturation
	//! \param transition Optional parameter to set the transition from current state to new, standard is 4 = 400ms.
	//! \return Bool that is true on success
	bool setColorHueSaturation(uint16_t hue, uint8_t sat, uint8_t transition = 4)
	{
		if(colorHueStrategy)
		{
			return colorHueStrategy->setColorHueSaturation(hue, sat, transition, *this);
		}
		return false;
	};

	//! \brief Function to set the color of this light in CIE with specified x y.
	//!
	//! Notice the color will only be set if the light has a reference to a specific \ref ColorHueStrategy.
	//! The values of x and y are ranging from 0 to 1.
	//! \param x float that specifies the x coordinate in CIE
	//! \param y float that specifies the y coordinate in CIE
	//! \param transition Optional parameter to set the transition from current state to new, standard is 4 = 400ms
	//! \return Bool that is true on success
	bool setColorXY(float x, float y, uint8_t transition = 4)
	{
		if(colorHueStrategy)
		{
			return colorHueStrategy->setColorXY(x, y, transition, *this);
		}
		return false;
	};

	//! \brief Function to set the color of this light with red green and blue values.
	//!
	//! Notice the color will only be set if the light has a reference to a specific \ref ColorHueStrategy.
	//! The values of red, green and blue are ranging from 0 to 255.
	//! \param r uint8_t that specifies the red color value
	//! \param g uint8_t that specifies the green color value
	//! \param b uint8_t that specifies the blue color value
	//! \param transition Optional parameter to set the transition from current state to new, standard is 4 = 400ms
	//! \return Bool that is true on success
	bool setColorRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t transition = 4)
	{
		if(colorHueStrategy)
		{
			return colorHueStrategy->setColorRGB(r, g, b, transition, *this);
		}
		return false;
	};

	//! \brief Function that lets the light perform one breath cycle.
	//!
	//! Can be used for locating a light.
	//! \return bool that is true on success
	virtual bool alert();

	//! \brief Function that lets the light perform one breath cycle in specified color temperature.
	//!
	//! Notice the breath cylce will only be performed if the light has a reference to a specific \ref ColorTemperatureStrategy.
	//! \param mired Color temperature in mired
	//! \return Bool that is true on success
	bool alertTemperature(unsigned int mired)
	{
		if(colorTemperatureStrategy)
		{
			return colorTemperatureStrategy->alertTemperature(mired, *this);
		}
		return false;
	};

	//! \brief Function that lets the light perform one breath cycle in specified color.
	//!
	//! Notice the breath cylce will only be performed if the light has a reference to a specific \ref ColorHueStrategy.
	//! \param hue uint16_t that specifies the hue
	//! \param sat uint8_t that specifies the saturation
	//! \return Bool that is true on success
	bool alertHueSaturation(uint16_t hue, uint8_t sat)
	{
		if(colorHueStrategy)
		{
			return colorHueStrategy->alertHueSaturation(hue, sat, *this);
		}
		return false;
	};

	//! \brief Function that lets the light perform one breath cycle in specified color.
	//!
	//! Notice the breath cylce will only be performed if the light has a reference to a specific \ref ColorHueStrategy.
	//! The values of x and y are ranging from 0 to 1.
	//! \param x float that specifies the x coordinate in CIE
	//! \param y float that specifies the y coordinate in CIE
	//! \return Bool that is true on success
	bool alertXY(float x, float y)
	{
		if(colorHueStrategy)
		{
			return colorHueStrategy->alertXY(x, y, *this);
		}
		return false;
	};

	//! \brief Function that lets the light perform one breath cycle in specified color.
	//!
	//! Notice the breath cylce will only be performed if the light has a reference to a specific \ref ColorHueStrategy.
	//! The values of red, green and blue are ranging from 0 to 255.
	//! \param r uint8_t that specifies the red color value
	//! \param g uint8_t that specifies the green color value
	//! \param b uint8_t that specifies the blue color value
	//! \return Bool that is true on success
	bool alertRGB(uint8_t r, uint8_t g, uint8_t b)
	{
		if(colorHueStrategy)
		{
			return colorHueStrategy->alertRGB(r, g, b, *this);
		}
		return false;
	};

	//! \brief Function to turn colorloop effect on/off.
	//!
	//! Notice this function will only be performed light has a reference to a specific \ref ColorHueStrategy.
	//! The colorloop effect will loop through all colors on current hue and saturation levels.
	//! Notice that none of the setter functions check whether this feature is enabled and
	//! the colorloop can only be disabled with this function or by simply calling Off()/OffNoRefresh()
	//! and then On()/OnNoRefresh(), so you could alternatively call Off() and
	//! then use any of the setter functions.
	//! \param on bool that enables this feature when true and disables it when false
	//! \return Bool that is true on success
	bool setColorLoop(bool on)
	{
		if(colorHueStrategy)
		{
			return colorHueStrategy->setColorLoop(on, *this);
		}
		return false;
	};

protected:
	//! \brief Protected ctor that is used by \ref Hue class.
	//!
	//! \param ip String that specifies the ip of the Hue bridge
	//! \param username String that specifies the username used to control the bridge
	//! \param id Integer that specifies the id of this light
	//! \param handler HttpHandler of type \ref IHttpHandler for communication with the bridge
	//!
	//! leaves strategies unset
	HueLight(const std::string& ip, const std::string& username, int id, std::shared_ptr<const IHttpHandler> handler);

	//! \brief Protected ctor that is used by \ref Hue class, also sets strategies.
	//!
	//! \param ip String that specifies the ip of the Hue bridge
	//! \param username String that specifies the username used to control the bridge
	//! \param id Integer that specifies the id of this light
	//! \param brightnessStrategy Strategy for brightness. May be nullptr.
	//! \param colorTempStrategy Strategy for color temperature. May be nullptr.
	//! \param colorHueStrategy Strategy for color hue/saturation. May be nullptr.
	//! \param handler HttpHandler of type \ref IHttpHandler for communication with the bridge
	HueLight(const std::string& ip, const std::string& username, int id, std::shared_ptr<const BrightnessStrategy> brightnessStrategy, std::shared_ptr<const ColorTemperatureStrategy> colorTempStrategy, std::shared_ptr<const ColorHueStrategy> colorHueStrategy, std::shared_ptr<const IHttpHandler> handler);

	//! \brief Protected function that sets the brightness strategy.
	//!
	//! The strategy defines how specific commands that deal with brightness control are executed
	//! \param strat a strategy of type \ref BrightnessStrategy
	void setBrightnessStrategy(std::shared_ptr<const BrightnessStrategy> strat)  { brightnessStrategy = std::move(strat); };

	//! \brief Protected function that sets the colorTemperature strategy.
	//!
	//! The strategy defines how specific commands that deal with colortemperature control are executed
	//! \param strat a strategy of type \ref ColorTemperatureStrategy
	void setColorTemperatureStrategy(std::shared_ptr<const ColorTemperatureStrategy> strat)  { colorTemperatureStrategy = std::move(strat); };

	//! \brief Protected function that sets the colorHue strategy.
	//!
	//! The strategy defines how specific commands that deal with color control are executed
	//! \param strat a strategy of type \ref ColorHueStrategy
	void setColorHueStrategy(std::shared_ptr<const ColorHueStrategy> strat)  { colorHueStrategy = std::move(strat); };

	//! \brief Protected function that sets the HttpHandler.
	//!
	//! The HttpHandler defines how specific commands that deal with bridge communication are executed
	//! \param handler a HttpHandler of type \ref IHttpHandler
	void setHttpHandler(std::shared_ptr<const IHttpHandler> handler)  { http_handler = std::move(handler); };

	//! \brief Function that turns the light on without refreshing its state.
	//!
	//! \param transition Optional parameter to set the transition from current state to new standard is 4 = 400ms
	//! \return Bool that is true on success
	bool OnNoRefresh(uint8_t transition = 4);

	//! \brief Function that turns the light off without refreshing its state.
	//!
	//! \param transition Optional parameter to set the transition from current state to new standard is 4 = 400ms
	//! \return Bool that is true on success
	bool OffNoRefresh(uint8_t transition = 4);

	//! \brief Utility function to send a put request to the light.
	//!
	//! \throws std::runtime_error if the reply could not be parsed
	//! \return The parsed reply
	Json::Value SendPutRequest(const Json::Value& request);

	//! \brief Virtual function that refreshes the \ref state of the light.
	virtual void refreshState();

protected:
	std::string ip;			//!< holds the ip of the light's base
	std::string username;	//!< holds the username of the light's base
	int id;					//!< holds the id of the light
	Json::Value state;		//!< holds the current state of the light updated by \ref refreshState
	ColorType colorType;	//!< holds the \ref ColorType of the light

	std::shared_ptr<const BrightnessStrategy>		brightnessStrategy;			//!< holds a reference to the strategy that handles brightness commands
	std::shared_ptr<const ColorTemperatureStrategy> colorTemperatureStrategy;	//!< holds a reference to the strategy that handles colortemperature commands
	std::shared_ptr<const ColorHueStrategy>			colorHueStrategy;			//!< holds a reference to the strategy that handles all color commands
	std::shared_ptr<const IHttpHandler> http_handler;
};

#endif
