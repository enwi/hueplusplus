/**
    \file HueException.cpp
    Copyright Notice\n
    Copyright (C) 2020  Jan Rogall		- developer\n
    Copyright (C) 2020  Moritz Wirger	- developer\n

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

#include "HueException.h"

HueException::HueException(FileInfo fileInfo) : HueException("Hue exception", std::move(fileInfo)) {}

const char* HueException::what() const
{
    return whatMessage.c_str();
}

const FileInfo& HueException::GetFile() const noexcept
{
    return fileInfo;
}

HueException::HueException(const char* exceptionName, FileInfo fileInfo) : fileInfo(std::move(fileInfo))
{
    whatMessage = exceptionName;
    whatMessage.append(" from");
    whatMessage.append(this->fileInfo.ToString());
}

HueAPIResponseException::HueAPIResponseException(
    FileInfo fileInfo, int error, std::string address, std::string description)
    : HueException("Hue api response exception", std::move(fileInfo)),
      error(error),
      address(std::move(address)),
      description(std::move(description))
{
    whatMessage.append(" ");
    whatMessage.append(std::to_string(error));
    whatMessage.append(" ");
    whatMessage.append(this->address);
    whatMessage.append(" ");
    whatMessage.append(this->description);
}

int HueAPIResponseException::GetErrorNumber() const noexcept
{
    return error;
}

const std::string& HueAPIResponseException::GetAddress() const noexcept
{
    return address;
}

const std::string& HueAPIResponseException::GetDescription() const noexcept
{
    return description;
}

std::string FileInfo::ToString() const
{
    if (filename.empty() || line < 0)
    {
        return "Unknown file";
    }
    std::string result = filename;
    result.append(":");
    result.append(std::to_string(line));
    return result;
}
