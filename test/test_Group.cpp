/**
    \file test_Group.cpp
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

#include "testhelper.h"

#include "hueplusplus/Group.h"
#include "mocks/mock_HttpHandler.h"

using namespace hueplusplus;
using namespace testing;

class GroupTest : public Test
{
protected:
    const std::string groupName = "Group 1";
    const std::string type = "Room";
    const std::string roomType = "Bedroom";
    const bool on = true;
    const int bri = 254;
    const int hue = 10000;
    const int sat = 254;
    const std::string effect = "none";
    const float x = 0.5f;
    const float y = 0.6f;
    const int ct = 250;
    const std::string alert = "none";
    const std::string colormode = "ct";
    const bool any_on = true;
    const bool all_on = false;

    std::shared_ptr<MockHttpHandler> handler;
    HueCommandAPI commands;
    nlohmann::json groupState;

protected:
    GroupTest()
        : handler(std::make_shared<MockHttpHandler>()),
          commands(getBridgeIp(), getBridgePort(), getBridgeUsername(), handler),
          groupState({{"name", groupName}, {"type", type}, {"class", roomType}, {"lights", {"1", "2", "4"}},
              {"action",
                  {{"on", on}, {"bri", bri}, {"hue", hue}, {"sat", sat}, {"effect", effect},
                      {"xy", nlohmann::json::array({x, y})}, {"ct", ct}, {"alert", alert}, {"colormode", colormode}}},
              {"state", {{"any_on", any_on}, {"all_on", all_on}}}})
    {}

    void expectGetState(int id)
    {
        EXPECT_CALL(*handler,
            GETJson("/api/" + getBridgeUsername() + "/groups/" + std::to_string(id), nlohmann::json::object(),
                getBridgeIp(), getBridgePort()))
            .WillOnce(Return(groupState));
    }
};

TEST_F(GroupTest, Construtor)
{
    {
        const int id = 12;
        expectGetState(id);
        Group group(id, commands, std::chrono::seconds(0));
        EXPECT_EQ(id, group.getId());
        Mock::VerifyAndClearExpectations(handler.get());
    }
    {
        const int id = 0;
        expectGetState(id);
        Group group(id, commands, std::chrono::seconds(0));
        EXPECT_EQ(id, group.getId());
        Mock::VerifyAndClearExpectations(handler.get());
    }
}

TEST_F(GroupTest, getName)
{
    const int id = 1;
    expectGetState(id);
    Group group(id, commands, std::chrono::seconds(0));
    EXPECT_EQ(groupName, Const(group).getName());
}

TEST_F(GroupTest, getType)
{
    const int id = 1;
    expectGetState(id);
    Group group(id, commands, std::chrono::seconds(0));
    EXPECT_EQ(type, Const(group).getType());
}

TEST_F(GroupTest, getLightIds)
{
    const int id = 1;
    expectGetState(id);
    Group group(id, commands, std::chrono::seconds(0));
    EXPECT_EQ(std::vector<int>({1, 2, 4}), Const(group).getLightIds());
}

TEST_F(GroupTest, getRoomType)
{
    const int id = 1;
    expectGetState(id);
    Group group(id, commands, std::chrono::seconds(0));
    EXPECT_EQ(roomType, Const(group).getRoomType());
}

TEST_F(GroupTest, getAllOn)
{
    const int id = 1;
    expectGetState(id);
    Group group(id, commands, std::chrono::steady_clock::duration::max());
    EXPECT_EQ(all_on, group.getAllOn());
    EXPECT_EQ(all_on, Const(group).getAllOn());
}

TEST_F(GroupTest, getAnyOn)
{
    const int id = 1;
    expectGetState(id);
    Group group(id, commands, std::chrono::steady_clock::duration::max());
    EXPECT_EQ(any_on, group.getAnyOn());
    EXPECT_EQ(any_on, Const(group).getAnyOn());
}

TEST_F(GroupTest, getActionOn)
{
    const int id = 1;
    expectGetState(id);
    Group group(id, commands, std::chrono::steady_clock::duration::max());
    EXPECT_EQ(on, group.getActionOn());
    EXPECT_EQ(on, Const(group).getActionOn());
}

TEST_F(GroupTest, getActionHueSaturation)
{
    const int id = 1;
    expectGetState(id);
    Group group(id, commands, std::chrono::steady_clock::duration::max());
    std::pair<uint16_t, uint8_t> hueSat{hue, sat};
    EXPECT_EQ(hueSat, group.getActionHueSaturation());
    EXPECT_EQ(hueSat, Const(group).getActionHueSaturation());
}

TEST_F(GroupTest, getActionBrightness)
{
    const int id = 1;
    expectGetState(id);
    Group group(id, commands, std::chrono::steady_clock::duration::max());
    EXPECT_EQ(bri, group.getActionBrightness());
    EXPECT_EQ(bri, Const(group).getActionBrightness());
}

TEST_F(GroupTest, getActionColorTemperature)
{
    const int id = 1;
    expectGetState(id);
    Group group(id, commands, std::chrono::steady_clock::duration::max());
    EXPECT_EQ(ct, group.getActionColorTemperature());
    EXPECT_EQ(ct, Const(group).getActionColorTemperature());
}

TEST_F(GroupTest, getActionColorXY)
{
    const int id = 1;
    expectGetState(id);
    Group group(id, commands, std::chrono::steady_clock::duration::max());
    std::pair<float, float> xy{x, y};
    EXPECT_EQ(xy, group.getActionColorXY());
    EXPECT_EQ(xy, Const(group).getActionColorXY());
}

TEST_F(GroupTest, getActionColorMode)
{
    const int id = 1;
    expectGetState(id);
    Group group(id, commands, std::chrono::steady_clock::duration::max());
    EXPECT_EQ(colormode, group.getActionColorMode());
    EXPECT_EQ(colormode, Const(group).getActionColorMode());
}
