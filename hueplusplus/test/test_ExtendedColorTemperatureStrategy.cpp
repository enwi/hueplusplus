#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../include/ExtendedColorTemperatureStrategy.h"
#include "../include/json/json.h"
#include "mocks/mock_HttpHandler.h"
#include "mocks/mock_HueLight.h"
#include "testhelper.h"

#include <memory>
#include <string>

TEST(ExtendedColorTemperatureStrategy, setColorTemperature)
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
    Json::Value prep_ret;
    prep_ret = Json::Value(Json::arrayValue);
    prep_ret[0] = Json::Value(Json::objectValue);
    prep_ret[0]["success"] = Json::Value(Json::objectValue);
    prep_ret[0]["success"]["/lights/1/state/transitiontime"] = 6;
    prep_ret[1] = Json::Value(Json::objectValue);
    prep_ret[1]["success"] = Json::Value(Json::objectValue);
    prep_ret[1]["success"]["/lights/1/state/on"] = true;
    prep_ret[2] = Json::Value(Json::objectValue);
    prep_ret[2]["success"] = Json::Value(Json::objectValue);
    prep_ret[2]["success"]["/lights/1/state/ct"] = 155;
    EXPECT_CALL(test_light, SendPutRequest(_, "/state"))
        .Times(1)
        .WillOnce(Return(prep_ret));

    test_light.getState()["state"]["colormode"] = "ct";
    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["ct"] = 200;
    EXPECT_EQ(true, ExtendedColorTemperatureStrategy().setColorTemperature(200, 4, test_light));

    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, ExtendedColorTemperatureStrategy().setColorTemperature(155, 6, test_light));

    prep_ret[2]["success"]["/lights/1/state/ct"] = 153;
    EXPECT_CALL(test_light, SendPutRequest(_, "/state"))
        .Times(1)
        .WillOnce(Return(prep_ret));
    EXPECT_EQ(true, ExtendedColorTemperatureStrategy().setColorTemperature(0, 6, test_light));

    prep_ret[2]["success"]["/lights/1/state/ct"] = 500;
    EXPECT_CALL(test_light, SendPutRequest(_, "/state"))
        .Times(1)
        .WillOnce(Return(prep_ret));
    EXPECT_EQ(true, ExtendedColorTemperatureStrategy().setColorTemperature(600, 6, test_light));
}

TEST(ExtendedColorTemperatureStrategy, alertTemperature)
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
    EXPECT_EQ(false, ExtendedColorTemperatureStrategy().alertTemperature(400, test_light));

    EXPECT_CALL(test_light, setColorTemperature(_, _))
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["colormode"] = "hs";
    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["ct"] = 200;
    test_light.getState()["state"]["sat"] = 100;
    test_light.getState()["state"]["hue"] = 200;
    EXPECT_EQ(false, ExtendedColorTemperatureStrategy().alertTemperature(400, test_light));

    EXPECT_CALL(test_light, alert())
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, ExtendedColorTemperatureStrategy().alertTemperature(400, test_light));

    EXPECT_CALL(test_light, setColorHueSaturation(_, _, 1))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(true, ExtendedColorTemperatureStrategy().alertTemperature(400, test_light));

    EXPECT_CALL(test_light, OffNoRefresh(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, ExtendedColorTemperatureStrategy().alertTemperature(400, test_light));

    EXPECT_CALL(test_light, setColorTemperature(_, _))
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["colormode"] = "xy";
    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["xy"][0] = 0.1;
    test_light.getState()["state"]["xy"][1] = 0.1;
    EXPECT_EQ(false, ExtendedColorTemperatureStrategy().alertTemperature(400, test_light));

    EXPECT_CALL(test_light, alert())
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, ExtendedColorTemperatureStrategy().alertTemperature(400, test_light));

    EXPECT_CALL(test_light, setColorXY(_, _, 1))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(true, ExtendedColorTemperatureStrategy().alertTemperature(400, test_light));

    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, ExtendedColorTemperatureStrategy().alertTemperature(400, test_light));

    EXPECT_CALL(test_light, setColorTemperature(_, _))
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["colormode"] = "ct";
    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["ct"] = 200;
    EXPECT_EQ(false, ExtendedColorTemperatureStrategy().alertTemperature(400, test_light));

    EXPECT_CALL(test_light, alert())
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, ExtendedColorTemperatureStrategy().alertTemperature(400, test_light));

    EXPECT_EQ(true, ExtendedColorTemperatureStrategy().alertTemperature(400, test_light));

    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, ExtendedColorTemperatureStrategy().alertTemperature(400, test_light));
}
