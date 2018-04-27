#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../include/SimpleBrightnessStrategy.h"
#include "../include/json/json.h"
#include "mocks/mock_HttpHandler.h"
#include "mocks/mock_HueLight.h"
#include "testhelper.h"

#include <iostream>
#include <memory>
#include <string>

TEST(SimpleBrightnessStrategy, setBrightness)
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
    EXPECT_CALL(test_light, OffNoRefresh(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
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
    prep_ret[2]["success"]["/lights/1/state/bri"] = 50;
    EXPECT_CALL(test_light, SendPutRequest(_, "/state"))
        .Times(1)
        .WillOnce(Return(prep_ret));

    test_light.getState()["state"]["on"] = true;
    EXPECT_EQ(true, SimpleBrightnessStrategy().setBrightness(0, 4, test_light));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, SimpleBrightnessStrategy().setBrightness(0, 4, test_light));

    test_light.getState()["state"]["bri"] = 0;
    EXPECT_EQ(true, SimpleBrightnessStrategy().setBrightness(50, 6, test_light));
    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["bri"] = 50;
    EXPECT_EQ(true, SimpleBrightnessStrategy().setBrightness(50, 6, test_light));

    prep_ret[2]["success"]["/lights/1/state/bri"] = 254;
    EXPECT_CALL(test_light, SendPutRequest(_, "/state"))
        .Times(1)
        .WillOnce(Return(prep_ret));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, SimpleBrightnessStrategy().setBrightness(255, 6, test_light));
}

TEST(SimpleBrightnessStrategy, getBrightness)
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

    test_light.getState()["state"]["bri"] = 200;
    EXPECT_EQ(200, SimpleBrightnessStrategy().getBrightness(test_light));
    test_light.getState()["state"]["bri"] = 0;
    EXPECT_EQ(0, SimpleBrightnessStrategy().getBrightness(static_cast<const HueLight>(test_light)));
}
