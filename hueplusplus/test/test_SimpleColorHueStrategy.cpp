#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../include/SimpleColorHueStrategy.h"
#include "../include/json/json.h"
#include "mocks/mock_HttpHandler.h"
#include "mocks/mock_HueLight.h"
#include "testhelper.h"

#include <memory>
#include <string>

TEST(SimpleColorHueStrategy, setColorHue)
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
    prep_ret[2]["success"]["/lights/1/state/hue"] = 30500;
    EXPECT_CALL(test_light, SendPutRequest(_, "/state"))
        .Times(1)
        .WillOnce(Return(prep_ret));

    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["hue"] = 200;
    test_light.getState()["state"]["colormode"] = "hs";
    EXPECT_EQ(true, SimpleColorHueStrategy().setColorHue(200, 4, test_light));

    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, SimpleColorHueStrategy().setColorHue(30500, 6, test_light));
}

TEST(SimpleColorHueStrategy, setColorSaturation)
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
    prep_ret[2]["success"]["/lights/1/state/sat"] = 254;
    EXPECT_CALL(test_light, SendPutRequest(_, "/state"))
        .Times(1)
        .WillOnce(Return(prep_ret));

    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["sat"] = 100;
    test_light.getState()["state"]["colormode"] = "hs";
    EXPECT_EQ(true, SimpleColorHueStrategy().setColorSaturation(100, 4, test_light));

    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, SimpleColorHueStrategy().setColorSaturation(255, 6, test_light));
}

TEST(SimpleColorHueStrategy, setColorHueSaturation)
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
    prep_ret[2]["success"]["/lights/1/state/hue"] = 30500;
    prep_ret[3] = Json::Value(Json::objectValue);
    prep_ret[3]["success"] = Json::Value(Json::objectValue);
    prep_ret[3]["success"]["/lights/1/state/sat"] = 254;
    EXPECT_CALL(test_light, SendPutRequest(_, "/state"))
        .Times(1)
        .WillOnce(Return(prep_ret));

    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["sat"] = 100;
    test_light.getState()["state"]["hue"] = 200;
    test_light.getState()["state"]["colormode"] = "hs";
    EXPECT_EQ(true, SimpleColorHueStrategy().setColorHueSaturation(200, 100, 4, test_light));

    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, SimpleColorHueStrategy().setColorHueSaturation(30500, 255, 6, test_light));
}

TEST(SimpleColorHueStrategy, setColorXY)
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
    prep_ret[2]["success"]["/lights/1/state/xy"][0] = 0.2355;
    prep_ret[2]["success"]["/lights/1/state/xy"][1] = 0.1234;
    EXPECT_CALL(test_light, SendPutRequest(_, "/state"))
        .Times(1)
        .WillOnce(Return(prep_ret));

    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["xy"][0] = 0.1;
    test_light.getState()["state"]["xy"][1] = 0.1;
    test_light.getState()["state"]["colormode"] = "xy";
    EXPECT_EQ(true, SimpleColorHueStrategy().setColorXY(0.1f, 0.1f, 4, test_light));

    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, SimpleColorHueStrategy().setColorXY(0.2355f, 0.1234f, 6, test_light));
}

TEST(SimpleColorHueStrategy, setColorRGB)
{
    using namespace ::testing;
    std::shared_ptr<MockHttpHandler> handler(std::make_shared<MockHttpHandler>());
    EXPECT_CALL(*handler, GETJson("/api/" + getBridgeUsername() + "/lights/1", Json::Value(Json::objectValue), getBridgeIp(), 80))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(Json::Value(Json::objectValue)));
    MockHueLight test_light(handler);
    EXPECT_CALL(test_light, setColorXY(_, _, 4))
        .Times(2)
        .WillRepeatedly(Return(true));

    EXPECT_EQ(true, SimpleColorHueStrategy().setColorRGB(128, 128, 128, 4, test_light));

    EXPECT_EQ(true, SimpleColorHueStrategy().setColorRGB(255, 255, 255, 4, test_light));

    EXPECT_CALL(test_light, OffNoRefresh(4))
        .Times(1)
        .WillOnce(Return(true));
    EXPECT_EQ(true, SimpleColorHueStrategy().setColorRGB(0, 0, 0, 4, test_light));
}

TEST(SimpleColorHueStrategy, setColorLoop)
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
    prep_ret[0]["success"]["/lights/1/state/on"] = true;
    prep_ret[1] = Json::Value(Json::objectValue);
    prep_ret[1]["success"] = Json::Value(Json::objectValue);
    prep_ret[1]["success"]["/lights/1/state/effect"] = "colorloop";
    EXPECT_CALL(test_light, SendPutRequest(_, "/state"))
        .Times(1)
        .WillOnce(Return(prep_ret));

    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["effect"] = "colorloop";
    EXPECT_EQ(true, SimpleColorHueStrategy().setColorLoop(true, test_light));

    test_light.getState()["state"]["on"] = false;
    test_light.getState()["state"]["effect"] = "none";
    EXPECT_EQ(true, SimpleColorHueStrategy().setColorLoop(true, test_light));
}

TEST(SimpleColorHueStrategy, alertHueSaturation)
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
    EXPECT_EQ(false, SimpleColorHueStrategy().alertHueSaturation(30000, 128, test_light));

    EXPECT_CALL(test_light, setColorHueSaturation(_, _, 1))
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["colormode"] = "hs";
    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["sat"] = 100;
    test_light.getState()["state"]["hue"] = 200;
    EXPECT_EQ(false, SimpleColorHueStrategy().alertHueSaturation(200, 100, test_light));

    EXPECT_CALL(test_light, alert())
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, SimpleColorHueStrategy().alertHueSaturation(200, 100, test_light));

    EXPECT_EQ(true, SimpleColorHueStrategy().alertHueSaturation(200, 100, test_light));

    EXPECT_CALL(test_light, OffNoRefresh(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, SimpleColorHueStrategy().alertHueSaturation(200, 100, test_light));

    EXPECT_CALL(test_light, setColorHueSaturation(_, _, 1))
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["colormode"] = "xy";
    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["xy"][0] = 0.1;
    test_light.getState()["state"]["xy"][1] = 0.1;
    EXPECT_EQ(false, SimpleColorHueStrategy().alertHueSaturation(200, 100, test_light));

    EXPECT_CALL(test_light, alert())
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, SimpleColorHueStrategy().alertHueSaturation(200, 100, test_light));

    EXPECT_CALL(test_light, setColorXY(_, _, 1))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(true, SimpleColorHueStrategy().alertHueSaturation(200, 100, test_light));

    EXPECT_CALL(test_light, OffNoRefresh(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, SimpleColorHueStrategy().alertHueSaturation(200, 100, test_light));
}

TEST(SimpleColorHueStrategy, alertXY)
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
    EXPECT_EQ(false, SimpleColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

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
    EXPECT_EQ(false, SimpleColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_CALL(test_light, alert())
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, SimpleColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_CALL(test_light, setColorHueSaturation(_, _, 1))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(true, SimpleColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_CALL(test_light, OffNoRefresh(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, SimpleColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_CALL(test_light, setColorXY(_, _, 1))
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["colormode"] = "xy";
    test_light.getState()["state"]["on"] = true;
    EXPECT_EQ(false, SimpleColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_CALL(test_light, alert())
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, SimpleColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_EQ(true, SimpleColorHueStrategy().alertXY(0.1f, 0.1f, test_light));

    EXPECT_CALL(test_light, OffNoRefresh(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, SimpleColorHueStrategy().alertXY(0.1f, 0.1f, test_light));
}

TEST(SimpleColorHueStrategy, alertRGB)
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
    EXPECT_EQ(false, SimpleColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, setColorRGB(_, _, _, 1))
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["colormode"] = "hs";
    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["sat"] = 100;
    test_light.getState()["state"]["hue"] = 200;
    EXPECT_EQ(false, SimpleColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, alert())
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, SimpleColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, setColorHueSaturation(_, _, 1))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(true, SimpleColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, OffNoRefresh(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, SimpleColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, setColorRGB(_, _, _, 1))
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["colormode"] = "xy";
    test_light.getState()["state"]["on"] = true;
    test_light.getState()["state"]["xy"][0] = 0.1;
    test_light.getState()["state"]["xy"][1] = 0.1;
    EXPECT_EQ(false, SimpleColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, alert())
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(false, SimpleColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, setColorXY(_, _, 1))
        .Times(AtLeast(2))
        .WillRepeatedly(Return(true));
    EXPECT_EQ(true, SimpleColorHueStrategy().alertRGB(128, 128, 128, test_light));

    EXPECT_CALL(test_light, OffNoRefresh(_))
        .Times(AtLeast(1))
        .WillRepeatedly(Return(true));
    test_light.getState()["state"]["on"] = false;
    EXPECT_EQ(true, SimpleColorHueStrategy().alertRGB(128, 128, 128, test_light));
}

TEST(SimpleColorHueStrategy, getColorHueSaturation)
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

    test_light.getState()["state"]["hue"] = 5000;
    test_light.getState()["state"]["sat"] = 128;
    EXPECT_EQ(std::make_pair(static_cast<uint16_t>(5000), static_cast<uint8_t>(128)), SimpleColorHueStrategy().getColorHueSaturation(test_light));
    test_light.getState()["state"]["hue"] = 50000;
    test_light.getState()["state"]["sat"] = 158;
    EXPECT_EQ(std::make_pair(static_cast<uint16_t>(50000), static_cast<uint8_t>(158)), SimpleColorHueStrategy().getColorHueSaturation(static_cast<const HueLight>(test_light)));
}

TEST(SimpleColorHueStrategy, getColorXY)
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

    test_light.getState()["state"]["xy"][0] = 0.1234;
    test_light.getState()["state"]["xy"][1] = 0.1234;
    EXPECT_EQ(std::make_pair(static_cast<float>(0.1234), static_cast<float>(0.1234)), SimpleColorHueStrategy().getColorXY(test_light));
    test_light.getState()["state"]["xy"][0] = 0.12;
    test_light.getState()["state"]["xy"][1] = 0.6458;
    EXPECT_EQ(std::make_pair(static_cast<float>(0.12), static_cast<float>(0.6458)), SimpleColorHueStrategy().getColorXY(static_cast<const HueLight>(test_light)));
}
