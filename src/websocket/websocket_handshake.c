#include "websocket.h"
#include "../http/http.h"
#include "../crypt/base64.h"
#include <stdbool.h>
#include <string.h>

static inline bool is_valid_host(const char* restrict host)
{
    return true;
}

static inline bool is_valid_upgrade(const char* restrict value)
{
    if (strcmp(value, "websocket") == 0) {
        return true;
    }

    fprintf(stderr, "Invalid Header: key: Upgrade value:%s\n", value);
    return false;
}

static inline bool is_valid_connection(const char* restrict value)
{
    if (strcmp(value, "Upgrade") == 0) {
        return true;
    }

    fprintf(stderr, "Invalid Header: key: Connection value:%s\n", value);
    return false;
}

static inline bool is_valid_version(const char* restrict value)
{
    if (strcmp(value, "13") == 0) {
        return true;
    }

    fprintf(stderr, "Invalid Header: key: Sec-WebSocket-Version value:%s\n", value);
    return false;
}

static inline bool is_valid_websocket_key(const char* restrict value)
{
    if (strlen(value) < 16) {
        fprintf(stderr, "Invalid Header: key: Sec-WebSocket-Key size<16\n");
        return false;
    }

    if (!is_base64(value)) {
        fprintf(stderr, "Invalid Header: key: Sec-WebSocket-Key not base64\n");
        return false;
    }

    return true;
}

static inline bool is_valid_websocket_request_line(PHTTPRequestLine restrict line)
{
    if (strcmp(line->method, "GET") != 0) {
        fprintf(stderr, "Invalid request line: method is not GET\n");
        return false;
    }

    if (strlen(line->target) <= 0) {
        fprintf(stderr, "Invalid request line: target.size <= 0\n");
        return false;
    }

    if (strcmp(line->http_version, "HTTP/1.1") != 0) {
        fprintf(stderr, "Invalid request line: Invalid HTTP version\n");
        return false;
    }

    return true;
}

static inline bool is_valid_websocket_request_header_line(PHTTPRequestHeaderLine restrict line)
{
    if (strcmp(line->key, "Host") == 0) {
        if (is_valid_host(line->value)) {
            return true;
        }
        return false;
    } else if (strcmp(line->key, "Upgrade") == 0) {
        if (is_valid_upgrade(line->value)) {
            return true;
        }
        return false;
    } else if (strcmp(line->key, "Connection") == 0) {
        if (is_valid_connection(line->value)) {
            return true;
        }
        return false;
    } else if (strcmp(line->key, "Sec-WebSocket-Key") == 0) {
        if (is_valid_websocket_key(line->value)) {
            return true;
        }
        return false;
    } else if (strcmp(line->key, "Sec-WebSocket-Version") == 0) {
        if (is_valid_version(line->value)) {
            return true;
        }
        return false;
    }

    return true;
}

static inline bool is_valid_websocket_request_header(PHTTPRequestHeaderLine restrict headers, size_t header_size)
{
    for (size_t i = 0; i < header_size; i++) {
        if (!is_valid_websocket_request_header_line(&headers[i])) {
            return false;
        }
    }

    return true;
}

bool is_valid_websocket_request(PHTTPRequest restrict request)
{
    // sample
    //   GET /chat HTTP/1.1\r\n"
    //   Host: server.example.com\r\n
    //   Upgrade: websocket\r\n
    //   Connection: Upgrade\r\n
    //   Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n
    //   Origin: http://example.com\r\n
    //   Sec-WebSocket-Protocol: chat, superchat\r\n
    //   Sec-WebSocket-Version: 13\r\n
    //
    if (!is_valid_websocket_request_line(&request->line)) {
        return false;
    }

    if (!is_valid_websocket_request_header(request->headers, request->header_size)) {
        return false;
    }

    return true;
}

char* select_websocket_client_key(PHTTPRequest restrict request)
{
    for (size_t i = 0; i < request->header_size; i++) {
        PHTTPRequestHeaderLine line = &request->headers[i];
        if (strcmp(line->key, "Sec-WebSocket-Key") == 0) {
            return line->value;
        }
    }

    return NULL;
}

bool create_handshake_ok_flame(const char* restrict accept_key, const size_t capacity, char* restrict buffer)
{
    const char* ok_message =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: ";
    const size_t ok_message_len = strlen(ok_message);
    const size_t accept_key_len = strlen(accept_key);

    if (capacity <= ok_message_len + accept_key_len + 5) {
        return false;
    }

    char* ptr = &buffer[0];
    memcpy(ptr, ok_message, ok_message_len);
    ptr += ok_message_len;
    memcpy(ptr, accept_key, accept_key_len);
    ptr += accept_key_len;
    memcpy(ptr, "\r\n\r\n\0", 5);
    ptr += 5;

    return true;
}
