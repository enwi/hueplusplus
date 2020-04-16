/**
    \file test_StateTransaction.cpp
    Copyright Notice\n
    Copyright (C) 2020  Jan Rogall		- developer\n
    Copyright (C) 2020  Moritz Wirger	- developer\n

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
#include <hueplusplus/StateTransaction.h>

#include "testhelper.h"

#include "mocks/mock_HttpHandler.h"

using namespace hueplusplus;
using namespace testing;

TEST(StateTransaction, commit)
{
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    const std::string requestPath = "/api/" + getBridgeUsername() + "/path";
    {
        EXPECT_CALL(*handler, PUTJson(_, _, getBridgeIp(), getBridgePort())).Times(0);
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Explicit off overrides brightness
    {
        nlohmann::json request = {{"on", false}, {"bri", 100}};
        nlohmann::json response = {{{"success", {{"/path/on", false}}}}, {{"success", {{"/path/bri", 100}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(
            StateTransaction(commands, "/path", nlohmann::json::object()).setOn(false).setBrightness(100).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}

TEST(StateTransaction, setOn)
{
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    const std::string requestPath = "/api/" + getBridgeUsername() + "/path";
    // Set on
    {
        nlohmann::json request = {{"on", true}};
        nlohmann::json response = {{{"success", {{"/path/on", true}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).setOn(true).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Set off
    {
        nlohmann::json request = {{"on", false}};
        nlohmann::json response = {{{"success", {{"/path/on", false}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).setOn(false).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Fail
    {
        nlohmann::json request = {{"on", false}};
        nlohmann::json response = {{{"success", {{"/path/on", true}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_FALSE(StateTransaction(commands, "/path", nlohmann::json::object()).setOn(false).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // No change requested
    {
        EXPECT_CALL(*handler, PUTJson(_, _, getBridgeIp(), getBridgePort())).Times(0);
        EXPECT_TRUE(StateTransaction(commands, "/path", {{"on", false}}).setOn(false).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}

TEST(StateTransaction, setBrightness)
{
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    const std::string requestPath = "/api/" + getBridgeUsername() + "/path";
    // No state
    {
        const int bri = 128;
        nlohmann::json request = {{"on", true}, {"bri", bri}};
        nlohmann::json response = {{{"success", {{"/path/on", true}}}}, {{"success", {{"/path/bri", bri}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).setBrightness(bri).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Clamp to 254
    {
        const int bri = 254;
        nlohmann::json request = {{"on", true}, {"bri", bri}};
        nlohmann::json response = {{{"success", {{"/path/on", true}}}}, {{"success", {{"/path/bri", bri}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).setBrightness(255).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Already off
    {
        const int bri = 0;
        nlohmann::json request = {{"bri", bri}};
        nlohmann::json response = {{{"success", {{"/path/bri", bri}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", {{"on", false}}).setBrightness(bri).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // No change requested
    {
        const int bri = 120;
        EXPECT_CALL(*handler, PUTJson(_, _, getBridgeIp(), getBridgePort())).Times(0);
        EXPECT_TRUE(StateTransaction(commands, "/path", {{"on", true}, {"bri", bri}}).setBrightness(bri).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}

TEST(StateTransaction, setColorHue)
{
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    const std::string requestPath = "/api/" + getBridgeUsername() + "/path";
    // No state
    {
        const int hue = 2159;
        nlohmann::json request = {{"on", true}, {"hue", hue}};
        nlohmann::json response = {{{"success", {{"/path/on", true}}}}, {{"success", {{"/path/hue", hue}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).setColorHue(hue).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Already on
    {
        const int hue = 2159;
        nlohmann::json request = {{"hue", hue}};
        nlohmann::json response = {{{"success", {{"/path/hue", hue}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", {{"on", true}}).setColorHue(hue).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Wrong colormode
    {
        const int hue = 2159;
        nlohmann::json request = {{"hue", hue}};
        nlohmann::json response = {{{"success", {{"/path/hue", hue}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", {{"on", true}, {"hue", hue}, {"colormode", "ct"}})
                        .setColorHue(hue)
                        .commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // No request
    {
        const int hue = 2159;
        EXPECT_CALL(*handler, PUTJson(_, _, getBridgeIp(), getBridgePort())).Times(0);
        EXPECT_TRUE(StateTransaction(commands, "/path", {{"on", true}, {"hue", hue}, {"colormode", "hs"}})
                        .setColorHue(hue)
                        .commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}

TEST(StateTransaction, setColorSaturation)
{
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    const std::string requestPath = "/api/" + getBridgeUsername() + "/path";
    // No state
    {
        const int sat = 125;
        nlohmann::json request = {{"on", true}, {"sat", sat}};
        nlohmann::json response = {{{"success", {{"/path/on", true}}}}, {{"success", {{"/path/sat", sat}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).setColorSaturation(sat).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Clamp to 254
    {
        const int sat = 254;
        nlohmann::json request = {{"on", true}, {"sat", sat}};
        nlohmann::json response = {{{"success", {{"/path/on", true}}}}, {{"success", {{"/path/sat", sat}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).setColorSaturation(255).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Already on
    {
        const int sat = 125;
        nlohmann::json request = {{"sat", sat}};
        nlohmann::json response = {{{"success", {{"/path/sat", sat}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", {{"on", true}}).setColorSaturation(sat).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Wrong colormode
    {
        const int sat = 125;
        nlohmann::json request = {{"sat", sat}};
        nlohmann::json response = {{{"success", {{"/path/sat", sat}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", {{"on", true}, {"sat", sat}, {"colormode", "ct"}})
                        .setColorSaturation(sat)
                        .commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // No request
    {
        const int sat = 125;
        EXPECT_CALL(*handler, PUTJson(_, _, getBridgeIp(), getBridgePort())).Times(0);
        EXPECT_TRUE(StateTransaction(commands, "/path", {{"on", true}, {"sat", sat}, {"colormode", "hs"}})
                        .setColorSaturation(sat)
                        .commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}

TEST(StateTransaction, setColorXY)
{
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    const std::string requestPath = "/api/" + getBridgeUsername() + "/path";
    // No state
    {
        const float x = 0.5f;
        const float y = 0.8f;
        nlohmann::json request = {{"on", true}, {"xy", {x, y}}};
        nlohmann::json response = {{{"success", {{"/path/on", true}}}}, {{"success", {{"/path/xy", {x, y}}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).setColorXY(x, y).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Clamp
    {
        const float x = 1.f;
        const float y = 0.f;
        nlohmann::json request = {{"on", true}, {"xy", {x, y}}};
        nlohmann::json response = {{{"success", {{"/path/on", true}}}}, {{"success", {{"/path/xy", {x, y}}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).setColorXY(2.f, -1.f).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Already on
    {
        const float x = 0.5f;
        const float y = 0.8f;
        nlohmann::json request = {{"xy", {x, y}}};
        nlohmann::json response = {{{"success", {{"/path/xy", {x, y}}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", {{"on", true}}).setColorXY(x, y).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Wrong colormode
    {
        const float x = 0.5f;
        const float y = 0.8f;
        nlohmann::json request = {{"xy", {x, y}}};
        nlohmann::json response = {{{"success", {{"/path/xy", {x, y}}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path",
            {{"on", true},
                {"xy",
                    {
                        x,
                        y,
                    }},
                {"colormode", "hs"}})
                        .setColorXY(x, y)
                        .commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // No request
    {
        const float x = 0.5f;
        const float y = 0.8f;
        EXPECT_CALL(*handler, PUTJson(_, _, getBridgeIp(), getBridgePort())).Times(0);
        EXPECT_TRUE(StateTransaction(commands, "/path",
            {{"on", true},
                {"xy",
                    {
                        x,
                        y,
                    }},
                {"colormode", "xy"}})
                        .setColorXY(x, y)
                        .commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}

TEST(StateTransaction, setColorTemperature)
{
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    const std::string requestPath = "/api/" + getBridgeUsername() + "/path";
    // No state
    {
        const int ct = 240;
        nlohmann::json request = {{"on", true}, {"ct", ct}};
        nlohmann::json response = {{{"success", {{"/path/on", true}}}}, {{"success", {{"/path/ct", ct}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).setColorTemperature(ct).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Clamp
    {
        const int ct = 500;
        nlohmann::json request = {{"ct", ct}};
        nlohmann::json response = {{{"success", {{"/path/ct", ct}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", {{"on", true}}).setColorTemperature(520).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Already on
    {
        const int ct = 240;
        nlohmann::json request = {{"ct", ct}};
        nlohmann::json response = {{{"success", {{"/path/ct", ct}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", {{"on", true}}).setColorTemperature(ct).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Wrong colormode
    {
        const int ct = 240;
        nlohmann::json request = {{"ct", ct}};
        nlohmann::json response = {{{"success", {{"/path/ct", ct}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", {{"on", true}, {"ct", ct}, {"colormode", "hs"}})
                        .setColorTemperature(ct)
                        .commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // No request
    {
        const int ct = 240;
        EXPECT_CALL(*handler, PUTJson(_, _, getBridgeIp(), getBridgePort())).Times(0);
        EXPECT_TRUE(StateTransaction(commands, "/path", {{"on", true}, {"ct", ct}, {"colormode", "ct"}})
                        .setColorTemperature(ct)
                        .commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}

TEST(StateTransaction, setColorLoop)
{
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    const std::string requestPath = "/api/" + getBridgeUsername() + "/path";
    // Set on
    {
        nlohmann::json request = {{"on", true}, {"effect", "colorloop"}};
        nlohmann::json response = {{{"success", {{"/path/on", true}}}}, {{"success", {{"/path/effect", "colorloop"}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).setColorLoop(true).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Set off
    {
        nlohmann::json request = {{"on", true}, {"effect", "none"}};
        nlohmann::json response = {{{"success", {{"/path/on", true}}}}, {{"success", {{"/path/effect", "none"}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).setColorLoop(false).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // No request
    {
        EXPECT_CALL(*handler, PUTJson(_, _, getBridgeIp(), getBridgePort())).Times(0);
        EXPECT_TRUE(
            StateTransaction(commands, "/path", {{"on", true}, {"effect", "colorloop"}}).setColorLoop(true).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}

TEST(StateTransaction, incrementBrightness)
{
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    const std::string requestPath = "/api/" + getBridgeUsername() + "/path";
    {
        const int inc = 20;
        nlohmann::json request = {{"bri_inc", inc}};
        nlohmann::json response = {{{"success", {{"/path/bri_inc", inc}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).incrementBrightness(inc).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Clamp
    {
        const int inc = -254;
        nlohmann::json request = {{"bri_inc", inc}};
        nlohmann::json response = {{{"success", {{"/path/bri_inc", inc}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).incrementBrightness(-300).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}

TEST(StateTransaction, incrementSaturation)
{
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    const std::string requestPath = "/api/" + getBridgeUsername() + "/path";
    {
        const int inc = 20;
        nlohmann::json request = {{"sat_inc", inc}};
        nlohmann::json response = {{{"success", {{"/path/sat_inc", inc}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).incrementSaturation(inc).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Clamp
    {
        const int inc = -254;
        nlohmann::json request = {{"sat_inc", inc}};
        nlohmann::json response = {{{"success", {{"/path/sat_inc", inc}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).incrementSaturation(-300).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}

TEST(StateTransaction, incrementHue)
{
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    const std::string requestPath = "/api/" + getBridgeUsername() + "/path";
    {
        const int inc = 20;
        nlohmann::json request = {{"hue_inc", inc}};
        nlohmann::json response = {{{"success", {{"/path/hue_inc", inc}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).incrementHue(inc).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Clamp
    {
        const int inc = -65534;
        nlohmann::json request = {{"hue_inc", inc}};
        nlohmann::json response = {{{"success", {{"/path/hue_inc", inc}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).incrementHue(-300000).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}

TEST(StateTransaction, incrementColorTemperature)
{
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    const std::string requestPath = "/api/" + getBridgeUsername() + "/path";
    {
        const int inc = 20;
        nlohmann::json request = {{"ct_inc", inc}};
        nlohmann::json response = {{{"success", {{"/path/ct_inc", inc}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(
            StateTransaction(commands, "/path", nlohmann::json::object()).incrementColorTemperature(inc).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Clamp
    {
        const int inc = -65534;
        nlohmann::json request = {{"ct_inc", inc}};
        nlohmann::json response = {{{"success", {{"/path/ct_inc", inc}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(
            StateTransaction(commands, "/path", nlohmann::json::object()).incrementColorTemperature(-300000).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}

TEST(StateTransaction, incrementColorXY)
{
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    const std::string requestPath = "/api/" + getBridgeUsername() + "/path";
    {
        const float incX = 0.2f;
        const float incY = -0.4f;
        nlohmann::json request = {{"xy_inc", {incX, incY}}};
        nlohmann::json response = {{{"success", {{"/path/xy_inc", {incX, incY}}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(
            StateTransaction(commands, "/path", nlohmann::json::object()).incrementColorXY(incX, incY).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Clamp
    {
        const float incX = 0.5f;
        const float incY = -0.5f;
        nlohmann::json request = {{"xy_inc", {incX, incY}}};
        nlohmann::json response = {{{"success", {{"/path/xy_inc", {incX, incY}}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).incrementColorXY(1.f, -1.f).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}

TEST(StateTransaction, setTransition)
{
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    const std::string requestPath = "/api/" + getBridgeUsername() + "/path";
    {
        nlohmann::json request = {{"on", true}, {"transitiontime", 2}};
        nlohmann::json response = {{{"success", {{"/path/on", true}}}}, {{"success", {{"/path/transitiontime", 2}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(
            StateTransaction(commands, "/path", nlohmann::json::object()).setOn(true).setTransition(2).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // No transition time 4
    {
        nlohmann::json request = {{"on", true}};
        nlohmann::json response = {{{"success", {{"/path/on", true}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(
            StateTransaction(commands, "/path", nlohmann::json::object()).setOn(true).setTransition(4).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // No request with only transition
    {
        EXPECT_CALL(*handler, PUTJson(_, _, getBridgeIp(), getBridgePort())).Times(0);
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).setTransition(2).commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}

TEST(StateTransaction, alert)
{
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    const std::string requestPath = "/api/" + getBridgeUsername() + "/path";
    {
        nlohmann::json request = {{"alert", "select"}};
        nlohmann::json response = {{{"success", {{"/path/alert", "select"}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).alert().commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Also alert when in state
    {
        nlohmann::json request = {{"alert", "select"}};
        nlohmann::json response = {{{"success", {{"/path/alert", "select"}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", {{"alert", "select"}}).alert().commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}

TEST(StateTransaction, longAlert)
{
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    const std::string requestPath = "/api/" + getBridgeUsername() + "/path";
    {
        nlohmann::json request = {{"alert", "lselect"}};
        nlohmann::json response = {{{"success", {{"/path/alert", "lselect"}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).longAlert().commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Also alert when in state
    {
        nlohmann::json request = {{"alert", "lselect"}};
        nlohmann::json response = {{{"success", {{"/path/alert", "lselect"}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", {{"alert", "lselect"}}).longAlert().commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}

TEST(StateTransaction, stopAlert)
{
    auto handler = std::make_shared<MockHttpHandler>();
    HueCommandAPI commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    const std::string requestPath = "/api/" + getBridgeUsername() + "/path";
    {
        nlohmann::json request = {{"alert", "none"}};
        nlohmann::json response = {{{"success", {{"/path/alert", "none"}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", nlohmann::json::object()).stopAlert().commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    // Also alert when in state
    {
        nlohmann::json request = {{"alert", "none"}};
        nlohmann::json response = {{{"success", {{"/path/alert", "none"}}}}};
        EXPECT_CALL(*handler, PUTJson(requestPath, request, getBridgeIp(), getBridgePort())).WillOnce(Return(response));
        EXPECT_TRUE(StateTransaction(commands, "/path", {{"alert", "none"}}).stopAlert().commit());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}
