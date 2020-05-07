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

#include <memory>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "testhelper.h"

#include "hueplusplus/Hue.h"
#include "json/json.hpp"
#include "mocks/mock_HttpHandler.h"

using namespace hueplusplus;

class HueFinderTest : public ::testing::Test
{
protected:
    std::shared_ptr<MockHttpHandler> handler;

protected:
    HueFinderTest() : handler(std::make_shared<MockHttpHandler>())
    {
        using namespace ::testing;

        EXPECT_CALL(*handler,
            sendMulticast("M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: "
                          "\"ssdp:discover\"\r\nMX: 5\r\nST: ssdp:all\r\n\r\n",
                "239.255.255.250", 1900, 5))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(getMulticastReply()));

        EXPECT_CALL(*handler, GETString("/description.xml", "application/xml", "", "192.168.2.1", getBridgePort()))
            .Times(0);

        EXPECT_CALL(*handler, GETString("/description.xml", "application/xml", "", getBridgeIp(), getBridgePort()))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(getBridgeXml()));
    }
    ~HueFinderTest() {};
};

TEST_F(HueFinderTest, FindBridges)
{
    HueFinder finder(handler);
    std::vector<HueFinder::HueIdentification> bridges = finder.FindBridges();

    HueFinder::HueIdentification bridge_to_comp;
    bridge_to_comp.ip = getBridgeIp();
    bridge_to_comp.port = getBridgePort();
    bridge_to_comp.mac = getBridgeMac();

    EXPECT_EQ(bridges.size(), 1) << "HueFinder found more than one Bridge";
    EXPECT_EQ(bridges[0].ip, bridge_to_comp.ip) << "HueIdentification ip does not match";
    EXPECT_EQ(bridges[0].port, bridge_to_comp.port) << "HueIdentification port does not match";
    EXPECT_EQ(bridges[0].mac, bridge_to_comp.mac) << "HueIdentification mac does not match";

    // Test invalid description
    EXPECT_CALL(*handler, GETString("/description.xml", "application/xml", "", getBridgeIp(), getBridgePort()))
        .Times(1)
        .WillOnce(::testing::Return("invalid stuff"));
    bridges = finder.FindBridges();
    EXPECT_TRUE(bridges.empty());
}

TEST_F(HueFinderTest, GetBridge)
{
    using namespace ::testing;
    nlohmann::json request {{"devicetype", "HuePlusPlus#User"}};

    nlohmann::json errorResponse
        = {{{"error", {{"type", 101}, {"address", ""}, {"description", "link button not pressed"}}}}};

    EXPECT_CALL(*handler, POSTJson("/api", request, getBridgeIp(), getBridgePort()))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(errorResponse));

    HueFinder finder(handler);
    std::vector<HueFinder::HueIdentification> bridges = finder.FindBridges();

    ASSERT_THROW(finder.GetBridge(bridges[0]), HueException);

    nlohmann::json successResponse = {{{"success", {{"username", getBridgeUsername()}}}}};

    EXPECT_CALL(*handler, POSTJson("/api", request, getBridgeIp(), getBridgePort()))
        .Times(1)
        .WillOnce(Return(successResponse));

    finder = HueFinder(handler);
    bridges = finder.FindBridges();

    Hue test_bridge = finder.GetBridge(bridges[0]);

    EXPECT_EQ(test_bridge.getBridgeIP(), getBridgeIp()) << "Bridge IP not matching";
    EXPECT_EQ(test_bridge.getBridgePort(), getBridgePort()) << "Bridge Port not matching";
    EXPECT_EQ(test_bridge.getUsername(), getBridgeUsername()) << "Bridge username not matching";

    Mock::VerifyAndClearExpectations(handler.get());
}

TEST_F(HueFinderTest, AddUsername)
{
    HueFinder finder(handler);
    std::vector<HueFinder::HueIdentification> bridges = finder.FindBridges();

    finder.AddUsername(bridges[0].mac, getBridgeUsername());
    Hue test_bridge = finder.GetBridge(bridges[0]);

    EXPECT_EQ(test_bridge.getBridgeIP(), getBridgeIp()) << "Bridge IP not matching";
    EXPECT_EQ(test_bridge.getBridgePort(), getBridgePort()) << "Bridge Port not matching";
    EXPECT_EQ(test_bridge.getUsername(), getBridgeUsername()) << "Bridge username not matching";
}

TEST_F(HueFinderTest, GetAllUsernames)
{
    HueFinder finder(handler);
    std::vector<HueFinder::HueIdentification> bridges = finder.FindBridges();

    finder.AddUsername(bridges[0].mac, getBridgeUsername());

    std::map<std::string, std::string> users = finder.GetAllUsernames();
    EXPECT_EQ(users[getBridgeMac()], getBridgeUsername()) << "Username of MAC:" << getBridgeMac() << "not matching";
}

TEST(Hue, Constructor)
{
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    Hue test_bridge(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);

    EXPECT_EQ(test_bridge.getBridgeIP(), getBridgeIp()) << "Bridge IP not matching";
    EXPECT_EQ(test_bridge.getBridgePort(), getBridgePort()) << "Bridge Port not matching";
    EXPECT_EQ(test_bridge.getUsername(), getBridgeUsername()) << "Bridge username not matching";
}

TEST(Hue, requestUsername)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    nlohmann::json request {{"devicetype", "HuePlusPlus#User"}};

    {
        nlohmann::json errorResponse
            = {{{"error", {{"type", 101}, {"address", ""}, {"description", "link button not pressed"}}}}};

        EXPECT_CALL(*handler, POSTJson("/api", request, getBridgeIp(), getBridgePort()))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(errorResponse));

        Hue test_bridge(getBridgeIp(), getBridgePort(), "", handler);

        std::string username = test_bridge.requestUsername();
        EXPECT_EQ(username, "") << "Returned username not matching";
        EXPECT_EQ(test_bridge.getUsername(), "") << "Bridge username not matching";
    }

    {
        // Other error code causes exception
        int otherError = 1;
        nlohmann::json exceptionResponse
            = {{{"error", {{"type", otherError}, {"address", ""}, {"description", "some error"}}}}};
        Hue testBridge(getBridgeIp(), getBridgePort(), "", handler);

        EXPECT_CALL(*handler, POSTJson("/api", request, getBridgeIp(), getBridgePort()))
            .WillOnce(Return(exceptionResponse));

        try
        {
            testBridge.requestUsername();
            FAIL() << "requestUsername did not throw";
        }
        catch (const HueAPIResponseException& e)
        {
            EXPECT_EQ(e.GetErrorNumber(), otherError);
        }
        catch (const std::exception& e)
        {
            FAIL() << "wrong exception: " << e.what();
        }
    }

    {
        nlohmann::json successResponse = {{{"success", {{"username", getBridgeUsername()}}}}};
        EXPECT_CALL(*handler, POSTJson("/api", request, getBridgeIp(), getBridgePort()))
            .Times(1)
            .WillRepeatedly(Return(successResponse));

        Hue test_bridge(getBridgeIp(), getBridgePort(), "", handler);

        std::string username = test_bridge.requestUsername();

        EXPECT_EQ(username, test_bridge.getUsername()) << "Returned username not matching";
        EXPECT_EQ(test_bridge.getBridgeIP(), getBridgeIp()) << "Bridge IP not matching";
        EXPECT_EQ(test_bridge.getUsername(), getBridgeUsername()) << "Bridge username not matching";

        // Verify that username is correctly set in api requests
        nlohmann::json hue_bridge_state {{"lights", {}}};
        EXPECT_CALL(
            *handler, GETJson("/api/" + getBridgeUsername(), nlohmann::json::object(), getBridgeIp(), getBridgePort()))
            .Times(1)
            .WillOnce(Return(hue_bridge_state));

        test_bridge.getAllLights();
    }
}

TEST(Hue, setIP)
{
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    Hue test_bridge(getBridgeIp(), getBridgePort(), "", handler);
    EXPECT_EQ(test_bridge.getBridgeIP(), getBridgeIp()) << "Bridge IP not matching after initialization";
    test_bridge.setIP("192.168.2.112");
    EXPECT_EQ(test_bridge.getBridgeIP(), "192.168.2.112") << "Bridge IP not matching after setting it";
}

TEST(Hue, setPort)
{
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    Hue test_bridge = Hue(getBridgeIp(), getBridgePort(), "", handler);
    EXPECT_EQ(test_bridge.getBridgePort(), getBridgePort()) << "Bridge Port not matching after initialization";
    test_bridge.setPort(81);
    EXPECT_EQ(test_bridge.getBridgePort(), 81) << "Bridge Port not matching after setting it";
}

TEST(Hue, getLight)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    EXPECT_CALL(
        *handler, GETJson("/api/" + getBridgeUsername(), nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(1);

    Hue test_bridge(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);

    // Test exception
    ASSERT_THROW(test_bridge.getLight(1), HueException);

    nlohmann::json hue_bridge_state {{"lights",
        {{"1",
            {{"state",
                 {{"on", true}, {"bri", 254}, {"ct", 366}, {"alert", "none"}, {"colormode", "ct"},
                     {"reachable", true}}},
                {"swupdate", {{"state", "noupdates"}, {"lastinstall", nullptr}}}, {"type", "Color temperature light"},
                {"name", "Hue ambiance lamp 1"}, {"modelid", "LTW001"}, {"manufacturername", "Philips"},
                {"uniqueid", "00:00:00:00:00:00:00:00-00"}, {"swversion", "5.50.1.19085"}}}}}};

    EXPECT_CALL(
        *handler, GETJson("/api/" + getBridgeUsername(), nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(1)
        .WillOnce(Return(hue_bridge_state));

    EXPECT_CALL(*handler,
        GETJson("/api/" + getBridgeUsername() + "/lights/1", nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(hue_bridge_state["lights"]["1"]));

    // Refresh cache
    test_bridge = Hue(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);

    // Test when correct data is sent
    HueLight test_light_1 = test_bridge.getLight(1);
    EXPECT_EQ(test_light_1.getName(), "Hue ambiance lamp 1");
    EXPECT_EQ(test_light_1.getColorType(), ColorType::TEMPERATURE);

    // Test again to check whether light is returned directly -> interesting for
    // code coverage test
    test_light_1 = test_bridge.getLight(1);
    EXPECT_EQ(test_light_1.getName(), "Hue ambiance lamp 1");
    EXPECT_EQ(test_light_1.getColorType(), ColorType::TEMPERATURE);
}

TEST(Hue, removeLight)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    nlohmann::json hue_bridge_state {{"lights",
        {{"1",
            {{"state",
                 {{"on", true}, {"bri", 254}, {"ct", 366}, {"alert", "none"}, {"colormode", "ct"},
                     {"reachable", true}}},
                {"swupdate", {{"state", "noupdates"}, {"lastinstall", nullptr}}}, {"type", "Color temperature light"},
                {"name", "Hue ambiance lamp 1"}, {"modelid", "LTW001"}, {"manufacturername", "Philips"},
                {"uniqueid", "00:00:00:00:00:00:00:00-00"}, {"swversion", "5.50.1.19085"}}}}}};
    EXPECT_CALL(
        *handler, GETJson("/api/" + getBridgeUsername(), nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(1)
        .WillOnce(Return(hue_bridge_state));

    Hue test_bridge(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);

    EXPECT_CALL(*handler,
        GETJson("/api/" + getBridgeUsername() + "/lights/1", nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(1)
        .WillRepeatedly(Return(hue_bridge_state["lights"]["1"]));

    nlohmann::json return_answer;
    return_answer = nlohmann::json::array();
    return_answer[0] = nlohmann::json::object();
    return_answer[0]["success"] = "/lights/1 deleted";
    EXPECT_CALL(*handler,
        DELETEJson(
            "/api/" + getBridgeUsername() + "/lights/1", nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(2)
        .WillOnce(Return(return_answer))
        .WillOnce(Return(nlohmann::json()));

    // Test when correct data is sent
    HueLight test_light_1 = test_bridge.getLight(1);

    EXPECT_EQ(test_bridge.removeLight(1), true);

    EXPECT_EQ(test_bridge.removeLight(1), false);
}

TEST(Hue, getAllLights)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    nlohmann::json hue_bridge_state {{"lights",
        {{"1",
            {{"state",
                 {{"on", true}, {"bri", 254}, {"ct", 366}, {"alert", "none"}, {"colormode", "ct"},
                     {"reachable", true}}},
                {"swupdate", {{"state", "noupdates"}, {"lastinstall", nullptr}}}, {"type", "Color temperature light"},
                {"name", "Hue ambiance lamp 1"}, {"modelid", "LTW001"}, {"manufacturername", "Philips"},
                {"uniqueid", "00:00:00:00:00:00:00:00-00"}, {"swversion", "5.50.1.19085"}}}}}};

    EXPECT_CALL(
        *handler, GETJson("/api/" + getBridgeUsername(), nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(hue_bridge_state));

    EXPECT_CALL(*handler,
        GETJson("/api/" + getBridgeUsername() + "/lights/1", nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(hue_bridge_state["lights"]["1"]));

    Hue test_bridge(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);

    std::vector<std::reference_wrapper<HueLight>> test_lights = test_bridge.getAllLights();
    ASSERT_EQ(1, test_lights.size());
    EXPECT_EQ(test_lights[0].get().getName(), "Hue ambiance lamp 1");
    EXPECT_EQ(test_lights[0].get().getColorType(), ColorType::TEMPERATURE);
}

TEST(Hue, lightExists)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    nlohmann::json hue_bridge_state {{"lights",
        {{"1",
            {{"state",
                 {{"on", true}, {"bri", 254}, {"ct", 366}, {"alert", "none"}, {"colormode", "ct"},
                     {"reachable", true}}},
                {"swupdate", {{"state", "noupdates"}, {"lastinstall", nullptr}}}, {"type", "Color temperature light"},
                {"name", "Hue ambiance lamp 1"}, {"modelid", "LTW001"}, {"manufacturername", "Philips"},
                {"uniqueid", "00:00:00:00:00:00:00:00-00"}, {"swversion", "5.50.1.19085"}}}}}};
    EXPECT_CALL(
        *handler, GETJson("/api/" + getBridgeUsername(), nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(hue_bridge_state));

    Hue test_bridge(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);

    test_bridge.refresh();

    EXPECT_TRUE(Const(test_bridge).lightExists(1));
    EXPECT_FALSE(Const(test_bridge).lightExists(2));
}

TEST(Hue, getGroup)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    EXPECT_CALL(
        *handler, GETJson("/api/" + getBridgeUsername(), nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(1);

    Hue test_bridge(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);

    // Test exception
    ASSERT_THROW(test_bridge.getGroup(1), HueException);

    nlohmann::json hue_bridge_state {{"groups",
        {{"1",
            {{"name", "Group 1"}, {"type", "LightGroup"}, {"lights", {"1", "2", "3"}},
                {"action",
                    {{"on", true}, {"bri", 254}, {"ct", 366}, {"alert", "none"}, {"colormode", "ct"}, {"hue", 200},
                        {"sat", 254}, {"effect", "none"}, {"xy", {0.f, 0.f}}}},
                {"state", {{"any_on", true}, {"all_on", true}}}}}}}};

    EXPECT_CALL(
        *handler, GETJson("/api/" + getBridgeUsername(), nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(1)
        .WillOnce(Return(hue_bridge_state));

    EXPECT_CALL(*handler,
        GETJson("/api/" + getBridgeUsername() + "/groups/1", nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(hue_bridge_state["groups"]["1"]));

    // Refresh cache
    test_bridge = Hue(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);

    // Test when correct data is sent
    Group test_group_1 = test_bridge.getGroup(1);
    EXPECT_EQ(test_group_1.getName(), "Group 1");
    EXPECT_EQ(test_group_1.getType(), "LightGroup");

    // Test again to check whether group is returned directly
    test_group_1 = test_bridge.getGroup(1);
    EXPECT_EQ(test_group_1.getName(), "Group 1");
    EXPECT_EQ(test_group_1.getType(), "LightGroup");
}

TEST(Hue, removeGroup)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    nlohmann::json hue_bridge_state {{"groups",
        {{"1",
            {{"name", "Group 1"}, {"type", "LightGroup"}, {"lights", {"1", "2", "3"}},
                {"action",
                    {{"on", true}, {"bri", 254}, {"ct", 366}, {"alert", "none"}, {"colormode", "ct"}, {"hue", 200},
                        {"sat", 254}, {"effect", "none"}, {"xy", {0.f, 0.f}}}},
                {"state", {{"any_on", true}, {"all_on", true}}}}}}}};
    EXPECT_CALL(
        *handler, GETJson("/api/" + getBridgeUsername(), nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(1)
        .WillOnce(Return(hue_bridge_state));

    Hue test_bridge(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);

    EXPECT_CALL(*handler,
        GETJson("/api/" + getBridgeUsername() + "/groups/1", nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(1)
        .WillRepeatedly(Return(hue_bridge_state["groups"]["1"]));

    nlohmann::json return_answer;
    return_answer = nlohmann::json::array();
    return_answer[0] = nlohmann::json::object();
    return_answer[0]["success"] = "/groups/1 deleted";
    EXPECT_CALL(*handler,
        DELETEJson(
            "/api/" + getBridgeUsername() + "/groups/1", nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(2)
        .WillOnce(Return(return_answer))
        .WillOnce(Return(nlohmann::json()));

    // Test when correct data is sent
    Group test_group_1 = test_bridge.getGroup(1);

    EXPECT_EQ(test_bridge.removeGroup(1), true);

    EXPECT_EQ(test_bridge.removeGroup(1), false);
}

TEST(Hue, groupExists)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    nlohmann::json hue_bridge_state {{"groups",
        {{"1",
            {{"name", "Group 1"}, {"type", "LightGroup"}, {"lights", {"1", "2", "3"}},
                {"action",
                    {{"on", true}, {"bri", 254}, {"ct", 366}, {"alert", "none"}, {"colormode", "ct"}, {"hue", 200},
                        {"sat", 254}, {"effect", "none"}, {"xy", {0.f, 0.f}}}},
                {"state", {{"any_on", true}, {"all_on", true}}}}}}}};
    EXPECT_CALL(
        *handler, GETJson("/api/" + getBridgeUsername(), nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(hue_bridge_state));

    Hue test_bridge(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);

    test_bridge.refresh();

    EXPECT_EQ(true, Const(test_bridge).groupExists(1));
    EXPECT_EQ(false, Const(test_bridge).groupExists(2));
}

TEST(Hue, getAllGroups)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    nlohmann::json hue_bridge_state {{"groups",
        {{"1",
            {{"name", "Group 1"}, {"type", "LightGroup"}, {"lights", {"1", "2", "3"}},
                {"action",
                    {{"on", true}, {"bri", 254}, {"ct", 366}, {"alert", "none"}, {"colormode", "ct"}, {"hue", 200},
                        {"sat", 254}, {"effect", "none"}, {"xy", {0.f, 0.f}}}},
                {"state", {{"any_on", true}, {"all_on", true}}}}}}}};

    EXPECT_CALL(
        *handler, GETJson("/api/" + getBridgeUsername(), nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(hue_bridge_state));

    EXPECT_CALL(*handler,
        GETJson("/api/" + getBridgeUsername() + "/groups/1", nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(hue_bridge_state["groups"]["1"]));

    Hue test_bridge(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);

    std::vector<std::reference_wrapper<Group>> test_groups = test_bridge.getAllGroups();
    ASSERT_EQ(1, test_groups.size());
    EXPECT_EQ(test_groups[0].get().getName(), "Group 1");
    EXPECT_EQ(test_groups[0].get().getType(), "LightGroup");
}

TEST(Hue, createGroup)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    EXPECT_CALL(
        *handler, GETJson("/api/" + getBridgeUsername(), nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(AtLeast(1));
    Hue test_bridge(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);
    CreateGroup create = CreateGroup::Room({2, 3}, "Nice room", "LivingRoom");
    nlohmann::json request = create.getRequest();
    const int id = 4;
    nlohmann::json response = {{{"success", {{"id", std::to_string(id)}}}}};
    EXPECT_CALL(*handler, POSTJson("/api/" + getBridgeUsername() + "/groups", request, getBridgeIp(), getBridgePort()))
        .WillOnce(Return(response));
    EXPECT_EQ(id, test_bridge.createGroup(create));

    response = {};
    EXPECT_CALL(*handler, POSTJson("/api/" + getBridgeUsername() + "/groups", request, getBridgeIp(), getBridgePort()))
        .WillOnce(Return(response));
    EXPECT_EQ(0, test_bridge.createGroup(create));
}

TEST(Hue, getPictureOfLight)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    nlohmann::json hue_bridge_state {{"lights",
        {{"1",
            {{"state",
                 {{"on", true}, {"bri", 254}, {"ct", 366}, {"alert", "none"}, {"colormode", "ct"},
                     {"reachable", true}}},
                {"swupdate", {{"state", "noupdates"}, {"lastinstall", nullptr}}}, {"type", "Color temperature light"},
                {"name", "Hue ambiance lamp 1"}, {"modelid", "LTW001"}, {"manufacturername", "Philips"},
                {"uniqueid", "00:00:00:00:00:00:00:00-00"}, {"swversion", "5.50.1.19085"}}}}}};

    EXPECT_CALL(
        *handler, GETJson("/api/" + getBridgeUsername(), nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(hue_bridge_state));
    EXPECT_CALL(*handler,
        GETJson("/api/" + getBridgeUsername() + "/lights/1", nlohmann::json::object(), getBridgeIp(), getBridgePort()))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(hue_bridge_state["lights"]["1"]));

    Hue test_bridge(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler);

    test_bridge.getLight(1);

    EXPECT_EQ("", test_bridge.getPictureOfLight(2));

    EXPECT_EQ("e27_waca", test_bridge.getPictureOfLight(1));
}
