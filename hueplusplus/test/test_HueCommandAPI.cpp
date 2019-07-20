/**
    \file test_HueCommandAPI.cpp
    Copyright Notice\n
    Copyright (C) 2018  Jan Rogall		- developer\n
    Copyright (C) 2018  Moritz Wirger	- developer\n

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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../include/Hue.h"
#include "../include/json/json.h"
#include "mocks/mock_HttpHandler.h"
#include "testhelper.h"

TEST(HueCommandAPI, PUTRequest)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> httpHandler = std::make_shared<MockHttpHandler>();

    HueCommandAPI api(getBridgeIp(), getBridgeUsername(), httpHandler);
    Json::Value request;
    Json::Value result = Json::objectValue;
    result["ok"] = true;

    //empty path
    {
        EXPECT_CALL(*httpHandler, PUTJson("/api/" + getBridgeUsername(), request, getBridgeIp(), 80)).WillOnce(Return(result));
        EXPECT_EQ(result, api.PUTRequest("", request));
        Mock::VerifyAndClearExpectations(httpHandler.get());
    }
    //not empty path, starting with slash
    {
        const std::string path = "/test";
        EXPECT_CALL(*httpHandler, PUTJson("/api/" + getBridgeUsername() + path, request, getBridgeIp(), 80)).WillOnce(Return(result));
        EXPECT_EQ(result, api.PUTRequest(path, request));
        Mock::VerifyAndClearExpectations(httpHandler.get());
    }
    //not empty path, not starting with slash
    {
        const std::string path = "test";
        EXPECT_CALL(*httpHandler, PUTJson("/api/" + getBridgeUsername() + '/' + path, request, getBridgeIp(), 80)).WillOnce(Return(result));
        EXPECT_EQ(result, api.PUTRequest(path, request));
        Mock::VerifyAndClearExpectations(httpHandler.get());
    }
    //recoverable error
    {
        const std::string path = "/test";
        EXPECT_CALL(*httpHandler, PUTJson("/api/" + getBridgeUsername() + path, request, getBridgeIp(), 80))
            .WillOnce(Throw(std::system_error(std::make_error_code(std::errc::connection_reset))))
            .WillOnce(Return(result));
        EXPECT_EQ(result, api.PUTRequest(path, request));
        Mock::VerifyAndClearExpectations(httpHandler.get());
    }
    //recoverable error x2
    {
        const std::string path = "/test";
        EXPECT_CALL(*httpHandler, PUTJson("/api/" + getBridgeUsername() + path, request, getBridgeIp(), 80))
            .WillOnce(Throw(std::system_error(std::make_error_code(std::errc::connection_reset))))
            .WillOnce(Throw(std::system_error(std::make_error_code(std::errc::connection_reset))));
        EXPECT_THROW(api.PUTRequest(path, request), std::system_error);
        Mock::VerifyAndClearExpectations(httpHandler.get());
    }
    //unrecoverable error
    {
        const std::string path = "/test";
        EXPECT_CALL(*httpHandler, PUTJson("/api/" + getBridgeUsername() + path, request, getBridgeIp(), 80))
            .WillOnce(Throw(std::system_error(std::make_error_code(std::errc::not_enough_memory))));
        EXPECT_THROW(api.PUTRequest(path, request), std::system_error);
        Mock::VerifyAndClearExpectations(httpHandler.get());
    }
}

TEST(HueCommandAPI, GETRequest)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> httpHandler = std::make_shared<MockHttpHandler>();

    HueCommandAPI api(getBridgeIp(), getBridgeUsername(), httpHandler);
    Json::Value request;
    Json::Value result = Json::objectValue;
    result["ok"] = true;

    //empty path
    {
        EXPECT_CALL(*httpHandler, GETJson("/api/" + getBridgeUsername(), request, getBridgeIp(), 80)).WillOnce(Return(result));
        EXPECT_EQ(result, api.GETRequest("", request));
        Mock::VerifyAndClearExpectations(httpHandler.get());
    }
    //not empty path, starting with slash
    {
        const std::string path = "/test";
        EXPECT_CALL(*httpHandler, GETJson("/api/" + getBridgeUsername() + path, request, getBridgeIp(), 80)).WillOnce(Return(result));
        EXPECT_EQ(result, api.GETRequest(path, request));
        Mock::VerifyAndClearExpectations(httpHandler.get());
    }
    //not empty path, not starting with slash
    {
        const std::string path = "test";
        EXPECT_CALL(*httpHandler, GETJson("/api/" + getBridgeUsername() + '/' + path, request, getBridgeIp(), 80)).WillOnce(Return(result));
        EXPECT_EQ(result, api.GETRequest(path, request));
        Mock::VerifyAndClearExpectations(httpHandler.get());
    }
    //recoverable error
    {
        const std::string path = "/test";
        EXPECT_CALL(*httpHandler, GETJson("/api/" + getBridgeUsername() + path, request, getBridgeIp(), 80))
            .WillOnce(Throw(std::system_error(std::make_error_code(std::errc::connection_reset))))
            .WillOnce(Return(result));
        EXPECT_EQ(result, api.GETRequest(path, request));
        Mock::VerifyAndClearExpectations(httpHandler.get());
    }
    //recoverable error x2
    {
        const std::string path = "/test";
        EXPECT_CALL(*httpHandler, GETJson("/api/" + getBridgeUsername() + path, request, getBridgeIp(), 80))
            .WillOnce(Throw(std::system_error(std::make_error_code(std::errc::connection_reset))))
            .WillOnce(Throw(std::system_error(std::make_error_code(std::errc::connection_reset))));
        EXPECT_THROW(api.GETRequest(path, request), std::system_error);
        Mock::VerifyAndClearExpectations(httpHandler.get());
    }
    //unrecoverable error
    {
        const std::string path = "/test";
        EXPECT_CALL(*httpHandler, GETJson("/api/" + getBridgeUsername() + path, request, getBridgeIp(), 80))
            .WillOnce(Throw(std::system_error(std::make_error_code(std::errc::not_enough_memory))));
        EXPECT_THROW(api.GETRequest(path, request), std::system_error);
        Mock::VerifyAndClearExpectations(httpHandler.get());
    }
}

TEST(HueCommandAPI, DELETERequest)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> httpHandler = std::make_shared<MockHttpHandler>();

    HueCommandAPI api(getBridgeIp(), getBridgeUsername(), httpHandler);
    Json::Value request;
    Json::Value result = Json::objectValue;
    result["ok"] = true;

    //empty path
    {
        EXPECT_CALL(*httpHandler, DELETEJson("/api/" + getBridgeUsername(), request, getBridgeIp(), 80)).WillOnce(Return(result));
        EXPECT_EQ(result, api.DELETERequest("", request));
        Mock::VerifyAndClearExpectations(httpHandler.get());
    }
    //not empty path, starting with slash
    {
        const std::string path = "/test";
        EXPECT_CALL(*httpHandler, DELETEJson("/api/" + getBridgeUsername() + path, request, getBridgeIp(), 80)).WillOnce(Return(result));
        EXPECT_EQ(result, api.DELETERequest(path, request));
        Mock::VerifyAndClearExpectations(httpHandler.get());
    }
    //not empty path, not starting with slash
    {
        const std::string path = "test";
        EXPECT_CALL(*httpHandler, DELETEJson("/api/" + getBridgeUsername() + '/' + path, request, getBridgeIp(), 80)).WillOnce(Return(result));
        EXPECT_EQ(result, api.DELETERequest(path, request));
        Mock::VerifyAndClearExpectations(httpHandler.get());
    }
    //recoverable error
    {
        const std::string path = "/test";
        EXPECT_CALL(*httpHandler, DELETEJson("/api/" + getBridgeUsername() + path, request, getBridgeIp(), 80))
            .WillOnce(Throw(std::system_error(std::make_error_code(std::errc::connection_reset))))
            .WillOnce(Return(result));
        EXPECT_EQ(result, api.DELETERequest(path, request));
        Mock::VerifyAndClearExpectations(httpHandler.get());
    }
    //recoverable error x2
    {
        const std::string path = "/test";
        EXPECT_CALL(*httpHandler, DELETEJson("/api/" + getBridgeUsername() + path, request, getBridgeIp(), 80))
            .WillOnce(Throw(std::system_error(std::make_error_code(std::errc::connection_reset))))
            .WillOnce(Throw(std::system_error(std::make_error_code(std::errc::connection_reset))));
        EXPECT_THROW(api.DELETERequest(path, request), std::system_error);
        Mock::VerifyAndClearExpectations(httpHandler.get());
    }
    //unrecoverable error
    {
        const std::string path = "/test";
        EXPECT_CALL(*httpHandler, GETJson("/api/" + getBridgeUsername() + path, request, getBridgeIp(), 80))
            .WillOnce(Throw(std::system_error(std::make_error_code(std::errc::not_enough_memory))));
        EXPECT_THROW(api.GETRequest(path, request), std::system_error);
        Mock::VerifyAndClearExpectations(httpHandler.get());
    }
}
