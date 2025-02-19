#include "http.h"

#include "../util/string.h"

static inline size_t extract_keyword(
    const char* restrict buffer,
    const size_t         buffer_size,
    const char           token,
    char* restrict       output)
{
    int keyword_length = skip_token(buffer, buffer_size, token);
    if (keyword_length == -1) {
        return -1;
    }

    websocket_memcpy(output, buffer, keyword_length);
    output[keyword_length] = '\0';
    return keyword_length;
}

static inline bool extract_http_request_line(
    const char* restrict      buffer,
    const size_t              buffer_size,
    PHTTPRequestLine restrict line)
{
    size_t remain_buffer_size = buffer_size;
    int    keyword_length;

    // method
    keyword_length = extract_keyword(buffer, remain_buffer_size, ' ', line->method);
    if (keyword_length == -1) {
        return false;
    }
    buffer += (keyword_length + 1);
    remain_buffer_size -= (keyword_length + 1);

    // target
    keyword_length = extract_keyword(buffer, remain_buffer_size, ' ', line->target);
    if (keyword_length == -1) {
        return false;
    }
    buffer += (keyword_length + 1);
    remain_buffer_size -= (keyword_length + 1);

    // HTTP version
    keyword_length = extract_keyword(buffer, remain_buffer_size, '\r', line->http_version);
    if (keyword_length == -1) {
        return false;
    }

    return true;
}

static inline bool extract_http_request_header_line(
    const char* restrict            buffer,
    const size_t                    buffer_size,
    PHTTPRequestHeaderLine restrict line)
{
    size_t remain_buffer_size = buffer_size;
    int    keyword_length;

    // Key
    keyword_length = extract_keyword(buffer, remain_buffer_size, ':', line->key);
    if (keyword_length == -1) {
        return false;
    }
    buffer += (keyword_length + 2);
    remain_buffer_size -= (keyword_length + 2);

    // Value
    keyword_length = extract_keyword(buffer, remain_buffer_size, '\r', line->value);
    if (keyword_length == -1) {
        return false;
    }

    return true;
}

bool extract_http_request_header(
    const char* restrict            buffer,
    const size_t                    buffer_size,
    const size_t                    header_capacity,
    size_t*                         header_size,
    PHTTPRequestHeaderLine restrict lines)
{
    size_t buffer_pos         = 0;
    size_t header_pos         = 0;
    size_t remain_buffer_size = buffer_size;

    while ((buffer_pos < buffer_size) && (header_pos < header_capacity)) {
        const char*            current_buffer = &buffer[buffer_pos];
        PHTTPRequestHeaderLine current_line   = &lines[header_pos];

        if (!extract_http_request_header_line(current_buffer, remain_buffer_size, current_line)) {
            return false;
        }

        size_t new_pos = skip_next_line(current_buffer, remain_buffer_size);
        buffer_pos += new_pos;
        remain_buffer_size -= new_pos;
        header_pos++;

        if (buffer_pos + 2 <= buffer_size) {
            if (buffer[buffer_pos] == '\r' && buffer[buffer_pos + 1] == '\n') {
                break;
            }
        }
    }

    if (header_pos <= 0) {
        return false;
    }

    *header_size = header_pos;
    return true;
}

bool extract_http_request(
    const char* restrict  buffer,
    const size_t          buffer_size,
    const size_t          header_capacity,
    PHTTPRequest restrict request)
{
    if (
        is_null(buffer) ||
        is_null((char*)request->headers) ||
        buffer_size <= 0 ||
        header_capacity <= 0) {
        return false;
    }

    size_t remain_buffer_size = buffer_size;
    if (!extract_http_request_line(buffer, remain_buffer_size, &request->line)) {
        return false;
    }

    size_t new_pos = skip_next_line(buffer, remain_buffer_size);
    if (new_pos == remain_buffer_size) {
        return false;
    }
    remain_buffer_size -= new_pos;

    if (!extract_http_request_header(
            &buffer[new_pos],
            remain_buffer_size,
            header_capacity,
            &request->header_size,
            request->headers)) {
        return false;
    }

    return true;
}

// #include <alloca.h>
// #include <assert.h>
//
// void nothing(void* ptr)
//{
// }

// int main(void)
//{
//     HTTPRequest request;
//     ALLOCATE_HTTP_REQUEST(request, alloca);
//
//     char* buffer =
//         "GET /chat HTTP/1.1\r\n"
//         "Host: server.example.com\r\n"
//         "Upgrade: websocket\r\n"
//         "Connection: Upgrade\r\n"
//         "Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==\r\n"
//         "Origin: http://example.com\r\n"
//         "Sec-WebSocket-Protocol: chat, superchat\r\n"
//         "Sec-WebSocket-Version: 13\r\n";
//     size_t buffer_size = strlen(buffer);
//
//     printf("exec!\n");
//     assert(
//         extract_http_request(buffer, buffer_size, HTTP_HEADER_CAPACITY, &request) == true);
//
//     assert(strcmp(request.line.method, "GET") == 0);
//     assert(strcmp(request.line.target, "/chat") == 0);
//     assert(strcmp(request.line.http_version, "HTTP/1.1") == 0);
//     assert(request.header_size == 7);
//     assert(strcmp(request.headers[0].key, "Host") == 0);
//     assert(strcmp(request.headers[0].value, "server.example.com") == 0);
//     assert(strcmp(request.headers[1].key, "Upgrade") == 0);
//     assert(strcmp(request.headers[1].value, "websocket") == 0);
//     assert(strcmp(request.headers[2].key, "Connection") == 0);
//     assert(strcmp(request.headers[2].value, "Upgrade") == 0);
//     assert(strcmp(request.headers[3].key, "Sec-WebSocket-Key") == 0);
//     assert(strcmp(request.headers[3].value, "dGhlIHNhbXBsZSBub25jZQ==") == 0);
//     assert(strcmp(request.headers[4].key, "Origin") == 0);
//     assert(strcmp(request.headers[4].value, "http://example.com") == 0);
//     assert(strcmp(request.headers[5].key, "Sec-WebSocket-Protocol") == 0);
//     assert(strcmp(request.headers[5].value, "chat, superchat") == 0);
//     assert(strcmp(request.headers[6].key, "Sec-WebSocket-Version") == 0);
//     assert(strcmp(request.headers[6].value, "13") == 0);
//
//     FREE_HTTP_REQUEST(request, nothing);
//     printf("end...\n");
//     return 0;
// }
