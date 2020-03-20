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

#include "include/HueException.h"

HueException::HueException(FileInfo fileInfo, const std::string& message)
    : HueException("Hue exception", std::move(fileInfo), message)
{}

const char* HueException::what() const
{
    return whatMessage.c_str();
}

const FileInfo& HueException::GetFile() const noexcept
{
    return fileInfo;
}

HueException::HueException(const char* exceptionName, FileInfo fileInfo, const std::string& message)
    : fileInfo(std::move(fileInfo))
{
    whatMessage = exceptionName;
    whatMessage.append(" from");
    whatMessage.append(this->fileInfo.ToString());
    whatMessage.append(" ");
    whatMessage.append(message);
}

HueAPIResponseException::HueAPIResponseException(
    FileInfo fileInfo, int error, std::string address, std::string description)
    : HueException("Hue api response exception", std::move(fileInfo), GetMessage(error, address, description)),
      error(error),
      address(std::move(address)),
      description(std::move(description))
{}

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

std::string HueAPIResponseException::GetMessage(int error, const std::string& addr, const std::string& description)
{
    std::string result = std::to_string(error);
    result.append(" ");
    result.append(addr);
    result.append(" ");
    result.append(description);
    return result;
}

std::string FileInfo::ToString() const
{
    if (filename.empty() || line < 0)
    {
        return "Unknown file";
    }
    std::string result = func;
    result.append(" in ");
    result.append(filename);
    result.append(":");
    result.append(std::to_string(line));
    return result;
}
