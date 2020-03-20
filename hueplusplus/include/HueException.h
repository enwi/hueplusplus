/**
    \file HueException.h
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

#ifndef _HUE_EXCEPTION_H
#define _HUE_EXCEPTION_H

#include <exception>
#include <string>

struct FileInfo
{
    std::string filename;
    int line = -1;

    std::string ToString() const;
};

class HueException : public std::exception
{
public:
    //! \brief Creates HueException with information about the thrown location
    HueException(FileInfo fileInfo, const std::string& message);

    const char* what() const override;

    //! \brief Filename and line where the exception was thrown or caused by
    const FileInfo& GetFile() const noexcept;

protected:
    //! \brief Creates HueException with child class name
    //!
    //! Should be used by subclasses which can append additional information to the end of whatMessage.
    HueException(const char* exceptionName, FileInfo fileInfo, const std::string& message);

private:
    std::string whatMessage;
    FileInfo fileInfo;
};

class HueAPIResponseException : public HueException
{
public:
    HueAPIResponseException(FileInfo fileInfo, int error, std::string address, std::string description);

    int GetErrorNumber() const noexcept;
    const std::string& GetAddress() const noexcept;
    const std::string& GetDescription() const noexcept;

private:
    static std::string GetMessage(int error, const std::string& addr, const std::string& description);

private:
    int error;
    std::string address;
    std::string description;
};

#endif