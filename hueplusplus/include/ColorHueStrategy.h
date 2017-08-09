/**
	\file ColorHueStrategy.h
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

#ifndef _COLOR_HUE_STRATEGY_H
#define _COLOR_HUE_STRATEGY_H

#include <stdint.h>

class HueLight;

//! Virtual base class for all ColorHueStrategys
class ColorHueStrategy
{
    public:
        //! \brief Function for changing a lights color in hue with a specified transition.
        //!
        //! The hue ranges from 0 to 65535, whereas 65535 and 0 are red, 25500 is green and 46920 is blue.
        //! \param hue The hue of the color
        //! \param transition The time it takes to fade to the new color in multiples of 100ms, 4 = 400ms and should be seen as the default
        //! \param light A reference of the light
        virtual bool setColorHue(uint16_t hue, uint8_t transition, HueLight& light) const = 0;
        //! \brief Function for changing a lights color in saturation with a specified transition.
        //!
        //! The saturation ranges from 0 to 254, whereas 0 is least saturated (white) and 254 is most saturated (vibrant).
        //! \param sat The saturation of the color
        //! \param transition The time it takes to fade to the new color in multiples of 100ms, 4 = 400ms and should be seen as the default
        //! \param light A reference of the light
        virtual bool setColorSaturation(uint8_t sat, uint8_t transition, HueLight& light) const = 0;
        //! \brief Function for changing a lights color in hue and saturation format with a specified transition.
        //!
        //! The hue ranges from 0 to 65535, whereas 65535 and 0 are red, 25500 is green and 46920 is blue.
        //! The saturation ranges from 0 to 254, whereas 0 is least saturated (white) and 254 is most saturated (vibrant).
        //! \param hue The hue of the color
        //! \param sat The saturation of the color
        //! \param transition The time it takes to fade to the new color in multiples of 100ms, 4 = 400ms and should be seen as the default
        //! \param light A reference of the light
        virtual bool setColorHueSaturation(uint16_t hue, uint8_t sat, uint8_t transition, HueLight& light) const = 0;
        //! \brief Function for changing a lights color in CIE format with a specified transition.
        //!
        //! \param x The x coordinate in CIE, ranging from 0 to 1
        //! \param y The y coordinate in CIE, ranging from 0 to 1
        //! \param transition The time it takes to fade to the new color in multiples of 100ms, 4 = 400ms and should be seen as the default
        //! \param light A reference of the light
        virtual bool setColorXY(float x, float y, uint8_t transition, HueLight& light) const = 0;
        //! \brief Function for changing a lights color in rgb format with a specified transition.
        //!
        //! Red, green and blue are ranging from 0 to 255.
        //! \param r The red portion of the color
        //! \param g The green portion of the color
        //! \param b The blue portion of the color
        //! \param transition The time it takes to fade to the new color in multiples of 100ms, 4 = 400ms and should be seen as the default
        //! \param light A reference of the light
        virtual bool setColorRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t transition, HueLight& light) const = 0;
        //! \brief Function for turning on/off the color loop feature of a light.
        //!
        //! Can be theoretically set for any light, but it only works for lights that support this feature.
        //! When this feature is activated the light will fade through every color on the current hue and saturation settings.
        //! Notice that none of the setter functions check whether this feature is enabled and
        //! the colorloop can only be disabled with this function or by simply calling Off()/OffNoRefresh()
        //! and then On()/OnNoRefresh(), so you could alternatively call Off() and
        //! then use any of the setter functions.
        //! \param on Boolean to turn this feature on or off, true/1 for on and false/0 for off
        //! \param light A reference of the light
        virtual bool setColorLoop(bool on, HueLight& light) const = 0;
        //! \brief Function that lets the light perform one breath cycle in the specified color.
        //!
        //! The hue ranges from 0 to 65535, whereas 65535 and 0 are red, 25500 is green and 46920 is blue.
        //! The saturation ranges from 0 to 254, whereas 0 is least saturated (white) and 254 is most saturated (vibrant).
        //! \param hue The hue of the color
        //! \param sat The saturation of the color
        //! \param light A reference of the light
        virtual bool alertHueSaturation(uint16_t hue, uint8_t sat, HueLight& light) const = 0;
        //! \brief Function that lets the light perform one breath cycle in the specified color.
        //!
        //! \param x The x coordinate in CIE, ranging from 0 to 1
        //! \param y The y coordinate in CIE, ranging from 0 to 1
        //! \param light A reference of the light
        virtual bool alertXY(float x, float y, HueLight& light) const = 0;
        //! \brief Function that lets the light perform one breath cycle in the specified color.
        //!
        //! Red, green and blue are ranging from 0 to 255.
        //! \param r The red portion of the color
        //! \param g The green portion of the color
        //! \param b The blue portion of the color
        //! \param light A reference of the light
        virtual bool alertRGB(uint8_t r, uint8_t g, uint8_t b, HueLight& light) const = 0;
        //! \brief Virtual dtor
        virtual ~ColorHueStrategy() = default;
};

#endif
