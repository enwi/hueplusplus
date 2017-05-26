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







#endif
