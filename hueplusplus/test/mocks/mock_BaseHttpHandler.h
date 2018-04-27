/**
    \file mock_BaseHttpHandler.h
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

#ifndef _MOCK_BASE_HTTPHANDLER_H
#define _MOCK_BASE_HTTPHANDLER_H

#include <string>
#include <vector>

#include <gmock/gmock.h>

#include "../hueplusplus/include/BaseHttpHandler.h"
#include "../hueplusplus/include/json/json.h"

//! Mock Class
class MockBaseHttpHandler : public BaseHttpHandler
{
public:
    MOCK_CONST_METHOD3(send, std::string(const std::string &msg, const std::string &adr, int port));

    MOCK_CONST_METHOD4(sendMulticast, std::vector<std::string>(const std::string &msg, const std::string &adr, int port, int timeout));
};

#endif
