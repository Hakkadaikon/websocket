#ifndef NOSTR_HTTP_H_
#define NOSTR_HTTP_H_

#include "../util/allocator.h"
#include "../util/types.h"

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
  HTTPRequestLine       line;
  HTTPRequestHeaderLine headers[HTTP_HEADER_CAPACITY];
  size_t                header_size;
} HTTPRequest, *PHTTPRequest;

bool extract_http_request(
  const char*  buffer,
  const size_t buffer_size,
  PHTTPRequest request);

#endif
