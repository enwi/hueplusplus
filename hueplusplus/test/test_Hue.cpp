#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../include/Hue.h"
#include "../include/json/json.h"
#include "mocks/mock_HttpHandler.h"
#include "testhelper.h"

#include <iostream>
#include <memory>
#include <string>


class HueFinderTest : public ::testing::Test
{
protected:
    std::shared_ptr<MockHttpHandler> handler;
protected:
    HueFinderTest()
        : handler(std::make_shared<MockHttpHandler>())
    {
        using namespace ::testing;

        EXPECT_CALL(*handler, sendMulticast("M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nMX: 5\r\nST: ssdp:all\r\n\r\n", "239.255.255.250", 1900, 5))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(getMulticastReply()));

        EXPECT_CALL(*handler, GETString("/description.xml", "application/xml", "", "192.168.2.1", 80))
            .Times(0);

        EXPECT_CALL(*handler, GETString("/description.xml", "application/xml", "", getBridgeIp(), 80))
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
    bridge_to_comp.mac = getBridgeMac();

    EXPECT_EQ(bridges.size(), 1) << "HueFinder found more than one Bridge";
    EXPECT_EQ(bridges[0].ip, bridge_to_comp.ip) << "HueIdentification ip does not match";
    EXPECT_EQ(bridges[0].mac, bridge_to_comp.mac) << "HueIdentification mac does not match";
}

TEST_F(HueFinderTest, GetBridge)
{
    using namespace ::testing;
    Json::Value request;
    request["devicetype"] = "HuePlusPlus#User";

    Json::Value user_ret_uns;
    user_ret_uns = Json::Value(Json::arrayValue);
    user_ret_uns[0] = Json::Value(Json::objectValue);
    user_ret_uns[0]["error"] = Json::Value(Json::objectValue);
    user_ret_uns[0]["error"]["type"] = 101;
    user_ret_uns[0]["error"]["address"] = "";
    user_ret_uns[0]["error"]["description"] = "link button not pressed";

    EXPECT_CALL(*handler, POSTJson("/api", request, getBridgeIp(), 80))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(user_ret_uns));

    HueFinder finder(handler);
    std::vector<HueFinder::HueIdentification> bridges = finder.FindBridges();

    ASSERT_THROW(finder.GetBridge(bridges[0]), std::runtime_error);

    Json::Value user_ret_suc;
    user_ret_suc = Json::Value(Json::arrayValue);
    user_ret_suc[0] = Json::Value(Json::objectValue);
    user_ret_suc[0]["success"] = Json::Value(Json::objectValue);
    user_ret_suc[0]["success"]["username"] = getBridgeUsername();

    EXPECT_CALL(*handler, POSTJson("/api", request, getBridgeIp(), 80))
        .Times(1)
        .WillOnce(Return(user_ret_suc));

    finder = HueFinder(handler);
    bridges = finder.FindBridges();

    Hue test_bridge = finder.GetBridge(bridges[0]);

    EXPECT_EQ(test_bridge.getBridgeIP(), getBridgeIp()) << "Bridge IP not matching";
    EXPECT_EQ(test_bridge.getUsername(), getBridgeUsername()) << "Bridge username not matching";

    //Verify that username is correctly set in api requests
    Json::Value hue_bridge_state;
    hue_bridge_state["lights"] = Json::objectValue;
    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername(), Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(1).WillOnce(Return(hue_bridge_state));

    test_bridge.getAllLights();

    Mock::VerifyAndClearExpectations(handler.get());
}

TEST_F(HueFinderTest, AddUsername)
{
    HueFinder finder(handler);
    std::vector<HueFinder::HueIdentification> bridges = finder.FindBridges();

    finder.AddUsername(bridges[0].mac, getBridgeUsername());
    Hue test_bridge = finder.GetBridge(bridges[0]);

    EXPECT_EQ(test_bridge.getBridgeIP(), getBridgeIp()) << "Bridge IP not matching";
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
    Hue test_bridge(getBridgeIp(), getBridgeUsername(), handler);

    EXPECT_EQ(test_bridge.getBridgeIP(), getBridgeIp()) << "Bridge IP not matching";
    EXPECT_EQ(test_bridge.getUsername(), getBridgeUsername()) << "Bridge username not matching";
}

TEST(Hue, requestUsername)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    Json::Value request;
    request["devicetype"] = "HuePlusPlus#User";

    Json::Value user_ret_uns;
    user_ret_uns = Json::Value(Json::arrayValue);
    user_ret_uns[0] = Json::Value(Json::objectValue);
    user_ret_uns[0]["error"] = Json::Value(Json::objectValue);
    user_ret_uns[0]["error"]["type"] = 101;
    user_ret_uns[0]["error"]["address"] = "";
    user_ret_uns[0]["error"]["description"] = "link button not pressed";

    EXPECT_CALL(*handler, POSTJson("/api", request, getBridgeIp(), 80))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(user_ret_uns));

    Hue test_bridge(getBridgeIp(), "", handler);

    test_bridge.requestUsername(test_bridge.getBridgeIP());
    EXPECT_EQ(test_bridge.getUsername(), "") << "Bridge username not matching";

    Json::Value user_ret_suc;
    user_ret_suc = Json::Value(Json::arrayValue);
    user_ret_suc[0] = Json::Value(Json::objectValue);
    user_ret_suc[0]["success"] = Json::Value(Json::objectValue);
    user_ret_suc[0]["success"]["username"] = getBridgeUsername();
    EXPECT_CALL(*handler, POSTJson("/api", request, getBridgeIp(), 80))
        .Times(1)
        .WillRepeatedly(Return(user_ret_suc));

    test_bridge = Hue(getBridgeIp(), "", handler);

    test_bridge.requestUsername(test_bridge.getBridgeIP());

    EXPECT_EQ(test_bridge.getBridgeIP(), getBridgeIp()) << "Bridge IP not matching";
    EXPECT_EQ(test_bridge.getUsername(), getBridgeUsername()) << "Bridge username not matching";

    //Verify that username is correctly set in api requests
    Json::Value hue_bridge_state;
    hue_bridge_state["lights"] = Json::objectValue;
    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername(), Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(1).WillOnce(Return(hue_bridge_state));

    test_bridge.getAllLights();
}

TEST(Hue, setIP)
{
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    Hue test_bridge(getBridgeIp(), "", handler);
    EXPECT_EQ(test_bridge.getBridgeIP(), getBridgeIp()) << "Bridge IP not matching after initialization";
    test_bridge.setIP("192.168.2.112");
    EXPECT_EQ(test_bridge.getBridgeIP(), "192.168.2.112") << "Bridge IP not matching after setting it";
}

TEST(Hue, getLight)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername(), Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(1);

    Hue test_bridge(getBridgeIp(), getBridgeUsername(), handler);

    // Test exception
    ASSERT_THROW(test_bridge.getLight(1), std::runtime_error);

    Json::Value hue_bridge_state;
    hue_bridge_state["lights"] = Json::Value(Json::objectValue);
    hue_bridge_state["lights"]["1"] = Json::Value(Json::objectValue);
    hue_bridge_state["lights"]["1"]["state"] = Json::Value(Json::objectValue);
    hue_bridge_state["lights"]["1"]["state"]["on"] = true;
    hue_bridge_state["lights"]["1"]["state"]["bri"] = 254;
    hue_bridge_state["lights"]["1"]["state"]["ct"] = 366;
    hue_bridge_state["lights"]["1"]["state"]["alert"] = "none";
    hue_bridge_state["lights"]["1"]["state"]["colormode"] = "ct";
    hue_bridge_state["lights"]["1"]["state"]["reachable"] = true;
    hue_bridge_state["lights"]["1"]["swupdate"] = Json::Value(Json::objectValue);
    hue_bridge_state["lights"]["1"]["swupdate"]["state"] = "noupdates";
    hue_bridge_state["lights"]["1"]["swupdate"]["lastinstall"] = Json::nullValue;
    hue_bridge_state["lights"]["1"]["type"] = "Color temperature light";
    hue_bridge_state["lights"]["1"]["name"] = "Hue ambiance lamp 1";
    hue_bridge_state["lights"]["1"]["modelid"] = "LTW001";
    hue_bridge_state["lights"]["1"]["manufacturername"] = "Philips";
    hue_bridge_state["lights"]["1"]["uniqueid"] = "00:00:00:00:00:00:00:00-00";
    hue_bridge_state["lights"]["1"]["swversion"] = "5.50.1.19085";
    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername(), Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(1)
        .WillOnce(Return(hue_bridge_state));

    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername() + "/lights/1", Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(hue_bridge_state["lights"]["1"]));

    // Test when correct data is sent
    HueLight test_light_1 = test_bridge.getLight(1);
    EXPECT_EQ(test_light_1.getName(), "Hue ambiance lamp 1");
    EXPECT_EQ(test_light_1.getColorType(), ColorType::TEMPERATURE);

    // Test again to check whether light is returned directly -> interesting for code coverage test
    test_light_1 = test_bridge.getLight(1);
    EXPECT_EQ(test_light_1.getName(), "Hue ambiance lamp 1");
    EXPECT_EQ(test_light_1.getColorType(), ColorType::TEMPERATURE);



    // more coverage stuff
    hue_bridge_state["lights"]["1"]["modelid"] = "LCT001";
    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername(), Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(1)
        .WillOnce(Return(hue_bridge_state));

    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername() + "/lights/1", Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(hue_bridge_state["lights"]["1"]));
    test_bridge = Hue(getBridgeIp(), getBridgeUsername(), handler);

    // Test when correct data is sent
    test_light_1 = test_bridge.getLight(1);
    EXPECT_EQ(test_light_1.getName(), "Hue ambiance lamp 1");
    EXPECT_EQ(test_light_1.getColorType(), ColorType::GAMUT_B);


    hue_bridge_state["lights"]["1"]["modelid"] = "LCT010";
    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername(), Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(1)
        .WillOnce(Return(hue_bridge_state));

    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername() + "/lights/1", Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(hue_bridge_state["lights"]["1"]));
    test_bridge = Hue(getBridgeIp(), getBridgeUsername(), handler);

    // Test when correct data is sent
    test_light_1 = test_bridge.getLight(1);
    EXPECT_EQ(test_light_1.getName(), "Hue ambiance lamp 1");
    EXPECT_EQ(test_light_1.getColorType(), ColorType::GAMUT_C);


    hue_bridge_state["lights"]["1"]["modelid"] = "LST001";
    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername(), Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(1)
        .WillOnce(Return(hue_bridge_state));

    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername() + "/lights/1", Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(hue_bridge_state["lights"]["1"]));
    test_bridge = Hue(getBridgeIp(), getBridgeUsername(), handler);

    // Test when correct data is sent
    test_light_1 = test_bridge.getLight(1);
    EXPECT_EQ(test_light_1.getName(), "Hue ambiance lamp 1");
    EXPECT_EQ(test_light_1.getColorType(), ColorType::GAMUT_A);


    hue_bridge_state["lights"]["1"]["modelid"] = "LWB004";
    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername(), Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(1)
        .WillOnce(Return(hue_bridge_state));

    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername() + "/lights/1", Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(hue_bridge_state["lights"]["1"]));
    test_bridge = Hue(getBridgeIp(), getBridgeUsername(), handler);

    // Test when correct data is sent
    test_light_1 = test_bridge.getLight(1);
    EXPECT_EQ(test_light_1.getName(), "Hue ambiance lamp 1");
    EXPECT_EQ(test_light_1.getColorType(), ColorType::NONE);


    hue_bridge_state["lights"]["1"]["modelid"] = "ABC000";
    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername(), Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(1)
        .WillOnce(Return(hue_bridge_state));
    test_bridge = Hue(getBridgeIp(), getBridgeUsername(), handler);

    ASSERT_THROW(test_bridge.getLight(1), std::runtime_error);
}

TEST(Hue, removeLight)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    Json::Value hue_bridge_state;
    hue_bridge_state["lights"] = Json::Value(Json::objectValue);
    hue_bridge_state["lights"]["1"] = Json::Value(Json::objectValue);
    hue_bridge_state["lights"]["1"]["state"] = Json::Value(Json::objectValue);
    hue_bridge_state["lights"]["1"]["state"]["on"] = true;
    hue_bridge_state["lights"]["1"]["state"]["bri"] = 254;
    hue_bridge_state["lights"]["1"]["state"]["ct"] = 366;
    hue_bridge_state["lights"]["1"]["state"]["alert"] = "none";
    hue_bridge_state["lights"]["1"]["state"]["colormode"] = "ct";
    hue_bridge_state["lights"]["1"]["state"]["reachable"] = true;
    hue_bridge_state["lights"]["1"]["swupdate"] = Json::Value(Json::objectValue);
    hue_bridge_state["lights"]["1"]["swupdate"]["state"] = "noupdates";
    hue_bridge_state["lights"]["1"]["swupdate"]["lastinstall"] = Json::nullValue;
    hue_bridge_state["lights"]["1"]["type"] = "Color temperature light";
    hue_bridge_state["lights"]["1"]["name"] = "Hue ambiance lamp 1";
    hue_bridge_state["lights"]["1"]["modelid"] = "LTW001";
    hue_bridge_state["lights"]["1"]["manufacturername"] = "Philips";
    hue_bridge_state["lights"]["1"]["uniqueid"] = "00:00:00:00:00:00:00:00-00";
    hue_bridge_state["lights"]["1"]["swversion"] = "5.50.1.19085";
    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername(), Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(1)
        .WillOnce(Return(hue_bridge_state));

    Hue test_bridge(getBridgeIp(), getBridgeUsername(), handler);

    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername() + "/lights/1", Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(1)
        .WillRepeatedly(Return(hue_bridge_state["lights"]["1"]));

    Json::Value return_answer;
    return_answer = Json::Value(Json::arrayValue);
    return_answer[0] = Json::Value(Json::objectValue);
    return_answer[0]["success"] = "/lights/1 deleted";
    EXPECT_CALL(*handler, DELETEJson("/api/" + getBridgeUsername() + "/lights/1", Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(2)
        .WillOnce(Return(return_answer))
        .WillOnce(Return(Json::Value()));

    // Test when correct data is sent
    HueLight test_light_1 = test_bridge.getLight(1);

    EXPECT_EQ(test_bridge.removeLight(1), true);

    EXPECT_EQ(test_bridge.removeLight(1), false);
}

TEST(Hue, getAllLights)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    Json::Value hue_bridge_state;
    hue_bridge_state["lights"] = Json::Value(Json::objectValue);
    hue_bridge_state["lights"]["1"] = Json::Value(Json::objectValue);
    hue_bridge_state["lights"]["1"]["state"] = Json::Value(Json::objectValue);
    hue_bridge_state["lights"]["1"]["state"]["on"] = true;
    hue_bridge_state["lights"]["1"]["state"]["bri"] = 254;
    hue_bridge_state["lights"]["1"]["state"]["ct"] = 366;
    hue_bridge_state["lights"]["1"]["state"]["alert"] = "none";
    hue_bridge_state["lights"]["1"]["state"]["colormode"] = "ct";
    hue_bridge_state["lights"]["1"]["state"]["reachable"] = true;
    hue_bridge_state["lights"]["1"]["swupdate"] = Json::Value(Json::objectValue);
    hue_bridge_state["lights"]["1"]["swupdate"]["state"] = "noupdates";
    hue_bridge_state["lights"]["1"]["swupdate"]["lastinstall"] = Json::nullValue;
    hue_bridge_state["lights"]["1"]["type"] = "Color temperature light";
    hue_bridge_state["lights"]["1"]["name"] = "Hue ambiance lamp 1";
    hue_bridge_state["lights"]["1"]["modelid"] = "LTW001";
    hue_bridge_state["lights"]["1"]["manufacturername"] = "Philips";
    hue_bridge_state["lights"]["1"]["uniqueid"] = "00:00:00:00:00:00:00:00-00";
    hue_bridge_state["lights"]["1"]["swversion"] = "5.50.1.19085";
    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername(), Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(2)
        .WillRepeatedly(Return(hue_bridge_state));

    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername() + "/lights/1", Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(2)
        .WillRepeatedly(Return(hue_bridge_state["lights"]["1"]));

    Hue test_bridge(getBridgeIp(), getBridgeUsername(), handler);

    std::vector<std::reference_wrapper<HueLight>> test_lights = test_bridge.getAllLights();
    ASSERT_EQ(1, test_lights.size());
    EXPECT_EQ(test_lights[0].get().getName(), "Hue ambiance lamp 1");
    EXPECT_EQ(test_lights[0].get().getColorType(), ColorType::TEMPERATURE);
}

TEST(Hue, lightExists)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    Json::Value hue_bridge_state;
    hue_bridge_state["lights"] = Json::Value(Json::objectValue);
    hue_bridge_state["lights"]["1"] = Json::Value(Json::objectValue);
    hue_bridge_state["lights"]["1"]["state"] = Json::Value(Json::objectValue);
    hue_bridge_state["lights"]["1"]["state"]["on"] = true;
    hue_bridge_state["lights"]["1"]["state"]["bri"] = 254;
    hue_bridge_state["lights"]["1"]["state"]["ct"] = 366;
    hue_bridge_state["lights"]["1"]["state"]["alert"] = "none";
    hue_bridge_state["lights"]["1"]["state"]["colormode"] = "ct";
    hue_bridge_state["lights"]["1"]["state"]["reachable"] = true;
    hue_bridge_state["lights"]["1"]["swupdate"] = Json::Value(Json::objectValue);
    hue_bridge_state["lights"]["1"]["swupdate"]["state"] = "noupdates";
    hue_bridge_state["lights"]["1"]["swupdate"]["lastinstall"] = Json::nullValue;
    hue_bridge_state["lights"]["1"]["type"] = "Color temperature light";
    hue_bridge_state["lights"]["1"]["name"] = "Hue ambiance lamp 1";
    hue_bridge_state["lights"]["1"]["modelid"] = "LTW001";
    hue_bridge_state["lights"]["1"]["manufacturername"] = "Philips";
    hue_bridge_state["lights"]["1"]["uniqueid"] = "00:00:00:00:00:00:00:00-00";
    hue_bridge_state["lights"]["1"]["swversion"] = "5.50.1.19085";
    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername(), Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(hue_bridge_state));
    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername() + "/lights/1", Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(hue_bridge_state["lights"]["1"]));

    Hue test_bridge(getBridgeIp(), getBridgeUsername(), handler);

    EXPECT_EQ(true, test_bridge.lightExists(1));
    EXPECT_EQ(false, test_bridge.lightExists(2));

    const Hue const_test_bridge1 = test_bridge;
    EXPECT_EQ(true, const_test_bridge1.lightExists(1));
    EXPECT_EQ(false, const_test_bridge1.lightExists(2));

    test_bridge.getLight(1);
    const Hue const_test_bridge2 = test_bridge;
    EXPECT_EQ(true, test_bridge.lightExists(1));
    EXPECT_EQ(true, const_test_bridge2.lightExists(1));
}

TEST(Hue, refreshState)
{
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    Hue test_bridge(getBridgeIp(), "", handler);    // NULL as username leads to segfault

    std::vector<std::reference_wrapper<HueLight>> test_lights = test_bridge.getAllLights();
    EXPECT_EQ(test_lights.size(), 0);
}
