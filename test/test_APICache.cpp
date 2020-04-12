/**
    \file test_Hue.cpp
    Copyright Notice\n
    Copyright (C) 2017  Jan Rogall		- developer\n
    Copyright (C) 2017  Moritz Wirger	- developer\n

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

#include <gtest/gtest.h>

#include "testhelper.h"

#include "hueplusplus/APICache.h"
#include "mocks/mock_HttpHandler.h"

using namespace hueplusplus;

TEST(APICache, GetRefreshDuration)
{
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    {
        std::chrono::steady_clock::duration refresh = std::chrono::seconds(20);
        APICache cache("", commands, refresh);
        EXPECT_EQ(refresh, cache.GetRefreshDuration());
    }
    {
        std::chrono::steady_clock::duration refresh = std::chrono::seconds(0);
        APICache cache("", commands, refresh);
        EXPECT_EQ(refresh, cache.GetRefreshDuration());
    }
    {
        std::chrono::steady_clock::duration refresh = std::chrono::steady_clock::duration::max();
        APICache cache("", commands, refresh);
        EXPECT_EQ(refresh, cache.GetRefreshDuration());
    }
}

TEST(APICache, Refresh)
{
    using namespace ::testing;
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);

    {
        std::string path = "/test/abc";
        APICache cache(path, commands, std::chrono::seconds(10));
        EXPECT_CALL(*handler,
            GETJson("/api/" + getBridgeUsername() + path, nlohmann::json::object(), getBridgeIp(), getBridgePort()))
            .WillOnce(Return(nlohmann::json::object()));
        cache.Refresh();
        Mock::VerifyAndClearExpectations(handler.get());
    }
    {
        std::string path = "";
        APICache cache(path, commands, std::chrono::seconds(10));
        EXPECT_CALL(*handler,
            GETJson(
                "/api/" + getBridgeUsername() + path, nlohmann::json::object(), getBridgeIp(), getBridgePort()))
            .Times(2)
            .WillRepeatedly(Return(nlohmann::json::object()));
        cache.Refresh();
        cache.Refresh();
        Mock::VerifyAndClearExpectations(handler.get());
    }
}

TEST(APICache, GetValue)
{
    using namespace ::testing;
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);

    // Always refresh
    {
        std::string path = "/test/abc";
        APICache cache(path, commands, std::chrono::seconds(0));
        nlohmann::json value = { {"a", "b"} };
        EXPECT_CALL(*handler,
            GETJson("/api/" + getBridgeUsername() + path, nlohmann::json::object(), getBridgeIp(), getBridgePort()))
            .Times(2)
            .WillRepeatedly(Return(value));
        EXPECT_EQ(value, cache.GetValue());
        EXPECT_EQ(value, cache.GetValue());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Only refresh once
    {
        std::string path = "/test/abc";
        APICache cache(path, commands, std::chrono::steady_clock::duration::max());
        nlohmann::json value = { {"a", "b"} };
        EXPECT_CALL(*handler,
            GETJson("/api/" + getBridgeUsername() + path, nlohmann::json::object(), getBridgeIp(), getBridgePort()))
            .WillOnce(Return(value));
        EXPECT_EQ(value, cache.GetValue());
        EXPECT_EQ(value, cache.GetValue());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // No refresh with const
    {
        std::string path = "/test/abc";
        const APICache cache(path, commands, std::chrono::steady_clock::duration::max());
        nlohmann::json value = { {"a", "b"} };
        EXPECT_CALL(*handler,
            GETJson("/api/" + getBridgeUsername() + path, nlohmann::json::object(), getBridgeIp(), getBridgePort()))
            .Times(0);
        EXPECT_EQ(nullptr, cache.GetValue());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}