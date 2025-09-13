#include <gtest/gtest.h>

#include <cstring>

extern "C" {
#include "http/http.h"
}

namespace
{

TEST(HttpRequestParseTest, ValidWebSocketUpgradeRequest)
{
    const char* request_text =
        "GET /chat HTTP/1.1\r\n"
        "Host: server.example.com\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
        "Origin: http://example.com\r\n"
        "Sec-WebSocket-Protocol: chat, superchat\r\n"
        "Sec-WebSocket-Version: 13\r\n";

    const size_t request_len = static_cast<size_t>(strlen(request_text));

    HTTPRequest request{};

    const bool ok = extract_http_request(request_text, request_len, &request);
    ASSERT_TRUE(ok);

    EXPECT_STREQ(request.line.method, "GET");
    EXPECT_STREQ(request.line.target, "/chat");
    EXPECT_STREQ(request.line.http_version, "HTTP/1.1");

    ASSERT_EQ(request.header_size, static_cast<size_t>(7));

    ASSERT_STREQ(request.headers[0].key, "Host");
    ASSERT_STREQ(request.headers[0].value, "server.example.com");

    ASSERT_STREQ(request.headers[1].key, "Upgrade");
    ASSERT_STREQ(request.headers[1].value, "websocket");

    ASSERT_STREQ(request.headers[2].key, "Connection");
    ASSERT_STREQ(request.headers[2].value, "Upgrade");

    ASSERT_STREQ(request.headers[3].key, "Sec-WebSocket-Key");
    ASSERT_STREQ(request.headers[3].value, "dGhlIHNhbXBsZSBub25jZQ==");

    ASSERT_STREQ(request.headers[4].key, "Origin");
    ASSERT_STREQ(request.headers[4].value, "http://example.com");

    ASSERT_STREQ(request.headers[5].key, "Sec-WebSocket-Protocol");
    ASSERT_STREQ(request.headers[5].value, "chat, superchat");

    ASSERT_STREQ(request.headers[6].key, "Sec-WebSocket-Version");
    ASSERT_STREQ(request.headers[6].value, "13");
}

TEST(HttpRequestParseTest, MissingColonInHeaderFails)
{
    const char* request_text =
        "GET / HTTP/1.1\r\n"
        "Host server.example.com\r\n";

    const size_t request_len = static_cast<size_t>(strlen(request_text));

    HTTPRequest request{};
    const bool  ok = extract_http_request(request_text, request_len, &request);
    EXPECT_FALSE(ok);
}

TEST(HttpRequestParseTest, OnlyRequestLineFails)
{
    const char*  request_text = "GET / HTTP/1.1";
    const size_t request_len  = static_cast<size_t>(strlen(request_text));

    HTTPRequest request{};
    const bool  ok = extract_http_request(request_text, request_len, &request);
    EXPECT_FALSE(ok);
}

}  // namespace
