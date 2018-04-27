#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../include/ExtendedColorHueStrategy.h"
#include "../include/json/json.h"
#include "mocks/mock_HttpHandler.h"
#include "mocks/mock_HueLight.h"
#include "testhelper.h"

#include <memory>
#include <string>

TEST(ExtendedColorHueStrategy, alertHueSaturation)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler(std::make_shared<MockHttpHandler>());
    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername() + "/lights/1", Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(Json::Value(Json::objectValue)));
    MockHueLight test_light(handler);
    EXPECT_CALL(test_light, refreshState())
        .Times(AtLeast(1))
        .WillRepeatedly(Return());

    test_light.getState()["state"]["colormode"] = "invalid";
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertHueSaturation(30000, 128, test_light));

    EXPECT_CALL(test_light, setColorHueSaturation(_, _, 1))
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["colormode"] = "hs";
    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["sat"] = 100;
    test_light.getState()["state"]["hue"] = 200;
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertHueSaturation(200, 100, test_light));

    EXPECT_CALL(test_light, alert())
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertHueSaturation(200, 100, test_light));

    EXPECT_EQ(true, ExtendedColorHueStrategy().alertHueSaturation(200, 100, test_light));

    EXPECT_CALL(test_light, OffNoRefresh(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, ExtendedColorHueStrategy().alertHueSaturation(200, 100, test_light));

    EXPECT_CALL(test_light, setColorHueSaturation(_, _, 1))
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["colormode"] = "xy";
    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["xy"][0] = 0.1;
    test_light.getState()["state"]["xy"][1] = 0.1;
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertHueSaturation(200, 100, test_light));

    EXPECT_CALL(test_light, alert())
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertHueSaturation(200, 100, test_light));

    EXPECT_CALL(test_light, setColorXY(_, _, 1))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(true, ExtendedColorHueStrategy().alertHueSaturation(200, 100, test_light));

    EXPECT_CALL(test_light, OffNoRefresh(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, ExtendedColorHueStrategy().alertHueSaturation(200, 100, test_light));

    EXPECT_CALL(test_light, setColorHueSaturation(_, _, 1))
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["colormode"] = "ct";
    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["ct"] = 200;
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertHueSaturation(200, 100, test_light));

    EXPECT_CALL(test_light, alert())
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertHueSaturation(200, 100, test_light));

    EXPECT_CALL(test_light, setColorTemperature(_, 1))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(true, ExtendedColorHueStrategy().alertHueSaturation(200, 100, test_light));

    EXPECT_CALL(test_light, OffNoRefresh(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, ExtendedColorHueStrategy().alertHueSaturation(200, 100, test_light));
}

TEST(ExtendedColorHueStrategy, alertXY)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler(std::make_shared<MockHttpHandler>());
    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername() + "/lights/1", Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(Json::Value(Json::objectValue)));
    MockHueLight test_light(handler);
    EXPECT_CALL(test_light, refreshState())
        .Times(AtLeast(1))
        .WillRepeatedly(Return());

    test_light.getState()["state"]["colormode"] = "invalid";
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_CALL(test_light, setColorXY(_, _, 1))
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["colormode"] = "hs";
    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["xy"][0] = 0.1;
    test_light.getState()["state"]["xy"][1] = 0.1;
    test_light.getState()["state"]["sat"] = 100;
    test_light.getState()["state"]["hue"] = 200;
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_CALL(test_light, alert())
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_CALL(test_light, setColorHueSaturation(_, _, 1))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(true, ExtendedColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_CALL(test_light, OffNoRefresh(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, ExtendedColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_CALL(test_light, setColorXY(_, _, 1))
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["colormode"] = "xy";
    test_light.getState()["state"]["on"] = true;
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_CALL(test_light, alert())
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_EQ(true, ExtendedColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_CALL(test_light, OffNoRefresh(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, ExtendedColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_CALL(test_light, setColorXY(_, _, 1))
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["colormode"] = "ct";
    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["ct"] = 200;
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_CALL(test_light, alert())
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_CALL(test_light, setColorTemperature(_, 1))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(true, ExtendedColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_CALL(test_light, OffNoRefresh(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, ExtendedColorHueStrategy().alertXY(0.1f, 0.1f, test_light));
}

TEST(ExtendedColorHueStrategy, alertRGB)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler(std::make_shared<MockHttpHandler>());
    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername() + "/lights/1", Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(Json::Value(Json::objectValue)));
    MockHueLight test_light(handler);
    EXPECT_CALL(test_light, refreshState())
        .Times(AtLeast(1))
        .WillRepeatedly(Return());

    test_light.getState()["state"]["colormode"] = "invalid";
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, setColorRGB(_, _, _, 1))
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["colormode"] = "hs";
    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["sat"] = 100;
    test_light.getState()["state"]["hue"] = 200;
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, alert())
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, setColorHueSaturation(_, _, 1))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(true, ExtendedColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, OffNoRefresh(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, ExtendedColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, setColorRGB(_, _, _, 1))
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["colormode"] = "xy";
    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["xy"][0] = 0.1;
    test_light.getState()["state"]["xy"][1] = 0.1;
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, alert())
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, setColorXY(_, _, 1))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(true, ExtendedColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, OffNoRefresh(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, ExtendedColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, setColorRGB(_, _, _, 1))
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["colormode"] = "ct";
    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["ct"] = 200;
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, alert())
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, ExtendedColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, setColorTemperature(_, 1))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(true, ExtendedColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, OffNoRefresh(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, ExtendedColorHueStrategy().alertRGB(128, 128, 128, test_light));
}
