#include "../../crypto/base64.h"
#include "../../util/allocator.h"
#include "../../util/string.h"
#include "../websocket_local.h"

#define IS_VALID_KEY(value, expected) is_compare_str(value, expected, HTTP_HEADER_KEY_CAPACITY, sizeof(expected), false)
#define IS_VALID_VALUE(value, expected) is_compare_str(value, expected, HTTP_HEADER_VALUE_CAPACITY, sizeof(expected), false)
#define IS_VALID_HTTP_VERSION(value, expected) is_compare_str(value, expected, HTTP_VERSION_CAPACITY, sizeof(expected), false)
#define IS_VALID_HTTP_METHOD(value, expected) is_compare_str(value, expected, HTTP_METHOD_CAPACITY, sizeof(expected), false)

static inline bool is_valid_host(const char* restrict host)
{
  return true;
}

static inline bool is_valid_upgrade(const char* restrict value)
{
  if (!IS_VALID_KEY(value, "websocket")) {
    str_error("Invalid websocket request header [Key: Upgrade] : ", value);
    return false;
  }

  return true;
}

static inline bool is_valid_connection(const char* restrict value)
{
  if (!IS_VALID_KEY(value, "upgrade")) {
    str_error("Invalid websocket request header [Key: Connection] : ", value);
    return false;
  }

  return true;
}

static inline bool is_valid_version(const char* restrict value)
{
  if (!IS_VALID_KEY(value, "13")) {
    str_error("Invalid websocket request header [Key: Sec-WebSocket-Version] : ", value);
    return false;
  }

  return true;
}

static inline bool is_valid_websocket_key(const char* restrict value)
{
  if (get_str_nlen(value, HTTP_HEADER_VALUE_CAPACITY) < 16) {
    log_error("Invalid websocket request header [Key: Sec-WebSocket-Key] Length is less than 16.\n");
    return false;
  }

  if (!is_base64(value)) {
    log_error("Invalid websocket request header [Key: Sec-WebSocket-Key] Client key is not base64.\n");
    return false;
  }

  return true;
}

static inline bool is_valid_method(char* value)
{
  if (!IS_VALID_HTTP_METHOD(value, "get")) {
    log_error("Invalid websocket request line: method is not GET\n");
    return false;
  }

  return true;
}

static inline bool is_valid_target(char* value)
{
  if (get_str_nlen(value, HTTP_TARGET_CAPACITY) <= 0) {
    log_error("Invalid websocket request line: target size is 0\n");
    return false;
  }

  return true;
}

static inline bool is_valid_http_version(char* value)
{
  bool has_error = false;

  if (!IS_VALID_HTTP_VERSION(value, "http/1.1") &&
      !IS_VALID_HTTP_VERSION(value, "http/2.0") &&
      !IS_VALID_HTTP_VERSION(value, "http/3.0")) {
    log_error("Invalid websocket request line: Invalid HTTP version(Not 1.1/2.0/3.0)\n");
    return false;
  }

  return true;
}

static inline bool is_valid_request_line(PHTTPRequestLine restrict line)
{
  if (!is_valid_method(line->method)) {
    return false;
  }

  if (!is_valid_target(line->target)) {
    return false;
  }

  if (!is_valid_http_version(line->http_version)) {
    return false;
  }

  return true;
}

static inline bool is_valid_request_header_line(PHTTPRequestHeaderLine restrict line)
{
  if (IS_VALID_KEY(line->key, "host")) {
    if (is_valid_host(line->value)) {
      return true;
    }
    return false;
  } else if (IS_VALID_KEY(line->key, "upgrade")) {
    if (is_valid_upgrade(line->value)) {
      return true;
    }
    return false;
  } else if (IS_VALID_KEY(line->key, "connection")) {
    if (is_valid_connection(line->value)) {
      return true;
    }
    return false;
  } else if (IS_VALID_KEY(line->key, "sec-webSocket-key")) {
    if (is_valid_websocket_key(line->value)) {
      return true;
    }
    return false;
  } else if (IS_VALID_KEY(line->key, "sec-websocket-version")) {
    if (is_valid_version(line->value)) {
      return true;
    }
    return false;
  }

  return true;
}

static inline bool is_valid_request_header(PHTTPRequestHeaderLine restrict headers, size_t header_size)
{
  for (size_t i = 0; i < header_size; i++) {
    if (!is_valid_request_header_line(&headers[i])) {
      return false;
    }
  }

  return true;
}

static inline bool is_valid_request(PHTTPRequest restrict request)
{
  if (!is_valid_request_line(&request->line)) {
    return false;
  }

  if (!is_valid_request_header(request->headers, request->header_size)) {
    return false;
  }

  return true;
}

static inline char* select_websocket_client_key(PHTTPRequest restrict request)
{
  for (size_t i = 0; i < request->header_size; i++) {
    PHTTPRequestHeaderLine line = &request->headers[i];
    if (is_compare_str(line->key, "Sec-WebSocket-Key", sizeof(line->key), 17, false)) {
      return line->value;
    }
  }

  log_error("WebSocket client key is not found.\n");
  return NULL;
}

static inline bool build_response_frame(
  const char* restrict accept_key,
  const int32_t accept_key_capacity,
  char* restrict buffer,
  const size_t capacity)
{
  const char OK_MESSAGE[] =
    "HTTP/1.1 101 Switching Protocols\r\n"
    "Upgrade: websocket\r\n"
    "Connection: Upgrade\r\n"
    "Sec-WebSocket-Accept: ";
  const size_t OK_MESSAGE_LEN    = sizeof(OK_MESSAGE) - 1;
  const size_t ACCEPT_KEY_LEN    = get_str_nlen(accept_key, accept_key_capacity);
  const size_t REQUIRED_CAPACITY = OK_MESSAGE_LEN + ACCEPT_KEY_LEN + 5;

  if (capacity <= REQUIRED_CAPACITY) {
    return false;
  }

  char* ptr = buffer;
  websocket_memcpy(ptr, OK_MESSAGE, OK_MESSAGE_LEN);
  ptr += OK_MESSAGE_LEN;

  websocket_memcpy(ptr, accept_key, ACCEPT_KEY_LEN);
  ptr += ACCEPT_KEY_LEN;

  websocket_memcpy(ptr, "\r\n\r\n", 4);
  ptr += 4;

  return true;
}

bool client_handshake(
  const int32_t       client_sock,
  const size_t        bytes_read,
  PWebSocketRawBuffer buffer,
  PHTTPRequest restrict request)
{
  bool has_error = false;

  if (!extract_http_request(buffer->request, bytes_read, request)) {
    has_error = true;
    goto FINALIZE;
  }

  if (!is_valid_request(request)) {
    has_error = true;
    goto FINALIZE;
  }

  char* client_key = select_websocket_client_key(request);
  if (is_null(client_key)) {
    has_error = true;
    goto FINALIZE;
  }

  char accept_key[HTTP_HEADER_VALUE_CAPACITY];
  if (!generate_websocket_acceptkey(client_key, sizeof(accept_key), accept_key)) {
    has_error = true;
    goto FINALIZE;
  }

  if (has_error) {
    str_info("Invalid handshake request : ", buffer->request);
  } else {
    if (!build_response_frame(accept_key, sizeof(accept_key), buffer->response, buffer->capacity)) {
      has_error = true;
      goto FINALIZE;
    }

    size_t response_len = get_str_nlen(buffer->response, buffer->capacity);
    if (response_len == 0) {
      has_error = true;
      goto FINALIZE;
    }

    if (websocket_send(client_sock, response_len, buffer->response) != WEBSOCKET_ERRORCODE_NONE) {
      log_error("Failed to send OK frame.");
      has_error = true;
      goto FINALIZE;
    }

    log_debug("handshake success !");
    str_debug("request : ", buffer->request);
    str_debug("response : ", buffer->response);
  }

FINALIZE:
  // Wipe variables
  websocket_memset_s(accept_key, sizeof(accept_key), 0x00, sizeof(accept_key));

  return !has_error;
}
