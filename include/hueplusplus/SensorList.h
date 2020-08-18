/**
    \file SensorList.h
    Copyright Notice\n
    Copyright (C) 2020  Jan Rogall		- developer\n

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

#ifndef INCLUDE_HUEPLUSPLUS_SENSOR_LIST_H
#define INCLUDE_HUEPLUSPLUS_SENSOR_LIST_H

#include "ResourceList.h"
#include "Sensor.h"

namespace hueplusplus
{
class SensorList : public CreateableResourceList<Sensor, int, CreateSensor>
{
public:
    using CreateableResourceList::CreateableResourceList;

    template <typename T>
    T getAsType(int id)
    {
        return get(id).asSensorType<T>();
    }
    template <typename T>
    std::vector<T> getAllByType()
    {
        nlohmann::json state = this->stateCache.getValue();
        std::vector<T> result;
        for (auto it = state.begin(); it != state.end(); ++it)
        {
            // Only parse the sensors with the correct type
            if (it->value("type", "") == T::typeStr)
            {
                result.push_back(get(maybeStoi(it.key())).asSensorType<T>());
            }
        }
        return result;
    }

protected:
    SensorList(SensorList&&) = default;
    SensorList& operator=(SensorList&&) = default;
};
} // namespace hueplusplus

#endif
