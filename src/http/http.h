#ifndef NOSTR_HTTP_H_
#define NOSTR_HTTP_H_

#include <stdbool.h>
#include <stddef.h>

#include "../util/allocator.h"

enum {
    HTTP_METHOD_CAPACITY       = 16,
    HTTP_TARGET_CAPACITY       = 256,
    HTTP_VERSION_CAPACITY      = 16,
    HTTP_HEADER_KEY_CAPACITY   = 32,
    HTTP_HEADER_VALUE_CAPACITY = 256,
    HTTP_HEADER_CAPACITY       = 32
};

typedef struct {
    char method[HTTP_METHOD_CAPACITY];
    char target[HTTP_TARGET_CAPACITY];
    char http_version[HTTP_VERSION_CAPACITY];
} HTTPRequestLine, *PHTTPRequestLine;

typedef struct {
    char key[HTTP_HEADER_KEY_CAPACITY];
    char value[HTTP_HEADER_VALUE_CAPACITY];
} HTTPRequestHeaderLine, *PHTTPRequestHeaderLine;

typedef struct {
    HTTPRequestLine        line;
    PHTTPRequestHeaderLine headers;
    size_t                 header_size;
} HTTPRequest, *PHTTPRequest;

bool extract_http_request(
    const char*  buffer,
    const size_t buffer_size,
    const size_t header_capacity,
    PHTTPRequest request);

#define ALLOCATE_HTTP_HEADER_LINE(HEADER_LINE, ALLOCATOR)    \
    HEADER_LINE.key   = ALLOCATOR(HTTP_HEADER_KEY_CAPACITY); \
    HEADER_LINE.value = ALLOCATOR(HTTP_HEADER_VALUE_CAPACITY);

#define ALLOCATE_HTTP_REQUEST(REQUEST, ALLOCATOR) \
    REQUEST.headers = (PHTTPRequestHeaderLine)ALLOCATOR(sizeof(HTTPRequestHeaderLine) * HTTP_HEADER_CAPACITY);

#define FREE_HTTP_REQUEST(REQUEST, FREE)                                                                                                                   \
    websocket_memset_s(REQUEST.headers, sizeof(HTTPRequestHeaderLine) * HTTP_HEADER_CAPACITY, 0x00, sizeof(HTTPRequestHeaderLine) * HTTP_HEADER_CAPACITY); \
    FREE(REQUEST.headers);

#endif
