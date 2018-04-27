#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../include/json/json.h"
#include "mocks/mock_BaseHttpHandler.h"
#include "testhelper.h"

#include <memory>
#include <string>

TEST(BaseHttpHandler, sendGetHTTPBody)
{
    using namespace ::testing;
    MockBaseHttpHandler handler;

    EXPECT_CALL(handler, send("testmsg", "192.168.2.1", 90))
        .Times(AtLeast(2))
        .WillOnce(Return(""))
        .WillRepeatedly(Return("\r\n\r\ntestreply"));

    EXPECT_THROW(handler.sendGetHTTPBody("testmsg", "192.168.2.1", 90), std::runtime_error);
    EXPECT_EQ("testreply", handler.sendGetHTTPBody("testmsg", "192.168.2.1", 90));
}

TEST(BaseHttpHandler, sendHTTPRequest)
{
    using namespace ::testing;
    MockBaseHttpHandler handler;

    EXPECT_CALL(handler, send("GET UrI HTTP/1.0\r\nContent-Type: text/html\r\nContent-Length: 4\r\n\r\nbody\r\n\r\n", "192.168.2.1", 90))
        .Times(AtLeast(2))
        .WillOnce(Return(""))
        .WillRepeatedly(Return("\r\n\r\ntestreply"));

    EXPECT_THROW(handler.sendHTTPRequest("GET", "UrI", "text/html", "body", "192.168.2.1", 90), std::runtime_error);
    EXPECT_EQ("testreply", handler.sendHTTPRequest("GET", "UrI", "text/html", "body", "192.168.2.1", 90));
}

TEST(BaseHttpHandler, GETString)
{
    using namespace ::testing;
    MockBaseHttpHandler handler;

    EXPECT_CALL(handler, send("GET UrI HTTP/1.0\r\nContent-Type: text/html\r\nContent-Length: 4\r\n\r\nbody\r\n\r\n", "192.168.2.1", 90))
        .Times(AtLeast(2))
        .WillOnce(Return(""))
        .WillRepeatedly(Return("\r\n\r\ntestreply"));

    EXPECT_THROW(handler.GETString("UrI", "text/html", "body", "192.168.2.1", 90), std::runtime_error);
    EXPECT_EQ("testreply", handler.GETString("UrI", "text/html", "body", "192.168.2.1", 90));
}

TEST(BaseHttpHandler, POSTString)
{
    using namespace ::testing;
    MockBaseHttpHandler handler;

    EXPECT_CALL(handler, send("POST UrI HTTP/1.0\r\nContent-Type: text/html\r\nContent-Length: 4\r\n\r\nbody\r\n\r\n", "192.168.2.1", 90))
        .Times(AtLeast(2))
        .WillOnce(Return(""))
        .WillRepeatedly(Return("\r\n\r\ntestreply"));

    EXPECT_THROW(handler.POSTString("UrI", "text/html", "body", "192.168.2.1", 90), std::runtime_error);
    EXPECT_EQ("testreply", handler.POSTString("UrI", "text/html", "body", "192.168.2.1", 90));
}

TEST(BaseHttpHandler, PUTString)
{
    using namespace ::testing;
    MockBaseHttpHandler handler;

    EXPECT_CALL(handler, send("PUT UrI HTTP/1.0\r\nContent-Type: text/html\r\nContent-Length: 4\r\n\r\nbody\r\n\r\n", "192.168.2.1", 90))
        .Times(AtLeast(2))
        .WillOnce(Return(""))
        .WillRepeatedly(Return("\r\n\r\ntestreply"));

    EXPECT_THROW(handler.PUTString("UrI", "text/html", "body", "192.168.2.1", 90), std::runtime_error);
    EXPECT_EQ("testreply", handler.PUTString("UrI", "text/html", "body", "192.168.2.1", 90));
}

TEST(BaseHttpHandler, DELETEString)
{
    using namespace ::testing;
    MockBaseHttpHandler handler;

    EXPECT_CALL(handler, send("DELETE UrI HTTP/1.0\r\nContent-Type: text/html\r\nContent-Length: 4\r\n\r\nbody\r\n\r\n", "192.168.2.1", 90))
        .Times(AtLeast(2))
        .WillOnce(Return(""))
        .WillRepeatedly(Return("\r\n\r\ntestreply"));

    EXPECT_THROW(handler.DELETEString("UrI", "text/html", "body", "192.168.2.1", 90), std::runtime_error);
    EXPECT_EQ("testreply", handler.DELETEString("UrI", "text/html", "body", "192.168.2.1", 90));
}

TEST(BaseHttpHandler, GETJson)
{
    using namespace ::testing;
    MockBaseHttpHandler handler;

    Json::Value testval;
    testval["test"] = 100;
    std::string expected_call = "GET UrI HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: ";
    expected_call.append(std::to_string(testval.toStyledString().size()));
    expected_call.append("\r\n\r\n");
    expected_call.append(testval.toStyledString());
    expected_call.append("\r\n\r\n");

    EXPECT_CALL(handler, send(expected_call, "192.168.2.1", 90))
        .Times(AtLeast(2))
        .WillOnce(Return(""))
        .WillOnce(Return("\r\n\r\n"))
        .WillRepeatedly(Return("\r\n\r\n{\"test\" : \"whatever\"}"));
    Json::Value expected;
    expected["test"] = "whatever";

    EXPECT_THROW(handler.GETJson("UrI", testval, "192.168.2.1", 90), std::runtime_error);
    EXPECT_THROW(handler.GETJson("UrI", testval, "192.168.2.1", 90), std::runtime_error);
    EXPECT_EQ(expected, handler.GETJson("UrI", testval, "192.168.2.1", 90));
}

TEST(BaseHttpHandler, POSTJson)
{
    using namespace ::testing;
    MockBaseHttpHandler handler;

    Json::Value testval;
    testval["test"] = 100;
    std::string expected_call = "POST UrI HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: ";
    expected_call.append(std::to_string(testval.toStyledString().size()));
    expected_call.append("\r\n\r\n");
    expected_call.append(testval.toStyledString());
    expected_call.append("\r\n\r\n");

    EXPECT_CALL(handler, send(expected_call, "192.168.2.1", 90))
        .Times(AtLeast(2))
        .WillOnce(Return(""))
        .WillOnce(Return("\r\n\r\n"))
        .WillRepeatedly(Return("\r\n\r\n{\"test\" : \"whatever\"}"));
    Json::Value expected;
    expected["test"] = "whatever";

    EXPECT_THROW(handler.POSTJson("UrI", testval, "192.168.2.1", 90), std::runtime_error);
    EXPECT_THROW(handler.POSTJson("UrI", testval, "192.168.2.1", 90), std::runtime_error);
    EXPECT_EQ(expected, handler.POSTJson("UrI", testval, "192.168.2.1", 90));
}

TEST(BaseHttpHandler, PUTJson)
{
    using namespace ::testing;
    MockBaseHttpHandler handler;

    Json::Value testval;
    testval["test"] = 100;
    std::string expected_call = "PUT UrI HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: ";
    expected_call.append(std::to_string(testval.toStyledString().size()));
    expected_call.append("\r\n\r\n");
    expected_call.append(testval.toStyledString());
    expected_call.append("\r\n\r\n");

    EXPECT_CALL(handler, send(expected_call, "192.168.2.1", 90))
        .Times(AtLeast(2))
        .WillOnce(Return(""))
        .WillOnce(Return("\r\n\r\n"))
        .WillRepeatedly(Return("\r\n\r\n{\"test\" : \"whatever\"}"));
    Json::Value expected;
    expected["test"] = "whatever";

    EXPECT_THROW(handler.PUTJson("UrI", testval, "192.168.2.1", 90), std::runtime_error);
    EXPECT_THROW(handler.PUTJson("UrI", testval, "192.168.2.1", 90), std::runtime_error);
    EXPECT_EQ(expected, handler.PUTJson("UrI", testval, "192.168.2.1", 90));
}

TEST(BaseHttpHandler, DELETEJson)
{
    using namespace ::testing;
    MockBaseHttpHandler handler;

    Json::Value testval;
    testval["test"] = 100;
    std::string expected_call = "DELETE UrI HTTP/1.0\r\nContent-Type: application/json\r\nContent-Length: ";
    expected_call.append(std::to_string(testval.toStyledString().size()));
    expected_call.append("\r\n\r\n");
    expected_call.append(testval.toStyledString());
    expected_call.append("\r\n\r\n");

    EXPECT_CALL(handler, send(expected_call, "192.168.2.1", 90))
        .Times(AtLeast(2))
        .WillOnce(Return(""))
        .WillOnce(Return("\r\n\r\n"))
        .WillRepeatedly(Return("\r\n\r\n{\"test\" : \"whatever\"}"));
    Json::Value expected;
    expected["test"] = "whatever";

    EXPECT_THROW(handler.DELETEJson("UrI", testval, "192.168.2.1", 90), std::runtime_error);
    EXPECT_THROW(handler.DELETEJson("UrI", testval, "192.168.2.1", 90), std::runtime_error);
    EXPECT_EQ(expected, handler.DELETEJson("UrI", testval, "192.168.2.1", 90));
}
