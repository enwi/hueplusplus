#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../include/json/json.h"
#include "mocks/mock_HttpHandler.h"
#include "testhelper.h"
#include "../include/UPnP.h"

#include "iostream"

const std::vector<std::pair<std::string, std::string>> expected_uplug_dev = {
    { "http://192.168.2.1:1900/gatedesc.xml", "Linux/2.6.36, UPnP/1.0, Portable SDK for UPnP devices/1.6.19" },
    { "http://192.168.2.116:80/description.xml", "Linux/3.14.0 UPnP/1.0 IpBridge/1.21.0" }
};

TEST(UPnP, getDevices)
{
    std::shared_ptr<MockHttpHandler> handler = std::make_shared<MockHttpHandler>();
    EXPECT_CALL(*handler, sendMulticast("M-SEARCH * HTTP/1.1\r\nHOST: 239.255.255.250:1900\r\nMAN: \"ssdp:discover\"\r\nMX: 5\r\nST: ssdp:all\r\n\r\n", "239.255.255.250", 1900, 5))
        .Times(1)
        .WillRepeatedly(::testing::Return(getMulticastReply()));

    UPnP uplug;
    std::vector<std::pair<std::string, std::string>> foundDevices = uplug.getDevices(handler);

    EXPECT_EQ(foundDevices, expected_uplug_dev);
}
