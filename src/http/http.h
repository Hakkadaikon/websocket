#ifndef NOSTR_HTTP_H_
#define NOSTR_HTTP_H_

#include <stddef.h>
#include <stdbool.h>

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
    HTTPRequestHeaderLine* headers;
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
    REQUEST.headers = ALLOCATOR(sizeof(HTTPRequestHeaderLine) * HTTP_HEADER_CAPACITY);

#define FREE_HTTP_HEADER_LINE(HEADER_LINE, FREE) \
    FREE(HEADER_LINE.key);                       \
    FREE(HEADER_LINE.value);

#define FREE_HTTP_REQUEST(REQUEST, FREE) \
    FREE(REQUEST.headers);

#endif
