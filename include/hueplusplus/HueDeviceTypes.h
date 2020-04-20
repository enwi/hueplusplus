/**
    \file HueDeviceTypes.h
    Copyright Notice\n
    Copyright (C) 2020  Jan Rogall      - developer\n
    Copyright (C) 2020  Moritz Wirger   - developer\n

    This file is part of hueplusplus.

    hueplusplus is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    hueplusplus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with hueplusplus.  If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef INCLUDE_HUEPLUSPLUS_HUEDEVICETYPES_H
#define INCLUDE_HUEPLUSPLUS_HUEDEVICETYPES_H

#include <memory>
#include <string>

#include "HueLight.h"

namespace hueplusplus
{
class HueLightFactory
{
public:
    HueLightFactory(const HueCommandAPI& commands);

    HueLight createLight(const std::string& type, int id);

private:
    HueCommandAPI commands;
    std::shared_ptr<BrightnessStrategy> simpleBrightness;
    std::shared_ptr<ColorTemperatureStrategy> simpleColorTemperature;
    std::shared_ptr<ColorTemperatureStrategy> extendedColorTemperature;
    std::shared_ptr<ColorHueStrategy> simpleColorHue;
    std::shared_ptr<ColorHueStrategy> extendedColorHue;
};
} // namespace hueplusplus

#endif
