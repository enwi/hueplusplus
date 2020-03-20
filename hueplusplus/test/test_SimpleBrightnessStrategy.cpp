#include <iostream>
#include <memory>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "testhelper.h"

#include "../include/SimpleBrightnessStrategy.h"
#include "../include/json/json.hpp"
#include "mocks/mock_HttpHandler.h"
#include "mocks/mock_HueLight.h"


TEST(SimpleBrightnessStrategy, setBrightness)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler(std::make_shared<MockHttpHandler>());
    EXPECT_CALL(
        *handler, GETJson("/api/" + getBridgeUsername() + "/lights/1", nlohmann::json::object(), getBridgeIp(), 80))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(nlohmann::json::object()));
    MockHueLight test_light(handler);
    EXPECT_CALL(test_light, refreshState()).Times(AtLeast(1)).WillRepeatedly(Return());
    EXPECT_CALL(test_light, OffNoRefresh(_)).Times(AtLeast(1)).WillRepeatedly(Return(true));
    nlohmann::json prep_ret;
    prep_ret = nlohmann::json::array();
    prep_ret[0] = nlohmann::json::object();
    prep_ret[0]["success"] = nlohmann::json::object();
    prep_ret[0]["success"]["/lights/1/state/transitiontime"] = 6;
    prep_ret[1] = nlohmann::json::object();
    prep_ret[1]["success"] = nlohmann::json::object();
    prep_ret[1]["success"]["/lights/1/state/on"] = true;
    prep_ret[2] = nlohmann::json::object();
    prep_ret[2]["success"] = nlohmann::json::object();
    prep_ret[2]["success"]["/lights/1/state/bri"] = 50;
    EXPECT_CALL(test_light, SendPutRequest(_, "/state")).Times(1).WillOnce(Return(prep_ret));

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
    EXPECT_CALL(test_light, SendPutRequest(_, "/state")).Times(1).WillOnce(Return(prep_ret));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, SimpleBrightnessStrategy().setBrightness(255, 6, test_light));
}

TEST(SimpleBrightnessStrategy, getBrightness)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler(std::make_shared<MockHttpHandler>());
    EXPECT_CALL(
        *handler, GETJson("/api/" + getBridgeUsername() + "/lights/1", nlohmann::json::object(), getBridgeIp(), 80))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(nlohmann::json::object()));
    MockHueLight test_light(handler);
    EXPECT_CALL(test_light, refreshState()).Times(AtLeast(1)).WillRepeatedly(Return());

    test_light.getState()["state"]["bri"] = 200;
    EXPECT_EQ(200, SimpleBrightnessStrategy().getBrightness(test_light));
    test_light.getState()["state"]["bri"] = 0;
    EXPECT_EQ(0, SimpleBrightnessStrategy().getBrightness(static_cast<const HueLight>(test_light)));
}
