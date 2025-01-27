#ifndef _NOSTR_WEBSOCKET_H_
#define _NOSTR_WEBSOCKET_H_

/** @file  websocket.h
 *
 * @brief Parses each parameter of a websocket frame stored in network byte order.
 * @see RFC6455 (https://datatracker.ietf.org/doc/html/rfc6455)
 */
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

typedef enum {
    WEBSOCKET_OP_CODE_TEXT   = 0x1,
    WEBSOCKET_OP_CODE_BINARY = 0x2,
    WEBSOCKET_OP_CODE_CLOSE  = 0x8,
    WEBSOCKET_OP_CODE_PING   = 0x9,
    WEBSOCKET_OP_CODE_PONG   = 0xA,
} WebSocketOpCode;

typedef enum {
    WEBSOCKET_ERRORCODE_FATAL_ERROR        = -3,
    WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR = -2,
    WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR  = -1,
    WEBSOCKET_ERRORCODE_NONE               = 0,
} WebSocketErrorCode;

typedef struct _WebSocketFrame {
    uint8_t         fin;
    uint8_t         rsv1;
    uint8_t         rsv2;
    uint8_t         rsv3;
    WebSocketOpCode opcode;
    uint8_t         mask;
    uint8_t         payload_len;
    uint64_t        ext_payload_len;
    uint8_t         masking_key[4];
    char*           payload;
} WebSocketFrame, *PWebSocketFrame;

typedef void (*PWebSocketCallback)(
    const int       client_sock,
    PWebSocketFrame frame,
    const size_t    client_buffer_capacity,
    char*           response_buffer);

/*----------------------------------------------------------------------------*/
/* websocket_parser.c                                                         */
/*----------------------------------------------------------------------------*/

/**
 * @brief Parse raw data in network byte order into a websocket flame structure
 *
 * @param[in]  raw         raw data (network byte order)
 * @param[in]  frame_size  Size of webSocket frame
 * @param[out] frame       Output destination of parsed frame
 *
 * @return true: Parse was successful / false: Failed parse
 */
bool   parse_websocket_frame(const char* raw, const size_t frame_size, PWebSocketFrame frame);
size_t create_websocket_frame(PWebSocketFrame frame, const size_t capacity, char* raw);

/*----------------------------------------------------------------------------*/
/* websocket/server/init.c                                                    */
/*----------------------------------------------------------------------------*/

int websocket_server_init(const int port_num, const int backlog);

/*----------------------------------------------------------------------------*/
/* websocket/socket/send.c                                                    */
/*----------------------------------------------------------------------------*/

int websocket_send(const int sock_fd, const char* buffer, const size_t buffer_size);

/*----------------------------------------------------------------------------*/
/* websocket/socket/recv.c                                                    */
/*----------------------------------------------------------------------------*/

ssize_t websocket_recvfrom(const int sock_fd, const size_t capacity, char* buffer);
#ifndef __APPLE__
ssize_t websocket_recvmmsg(const int sock_fd, const size_t capacity, char** buffers, const int num_of_buffer);
#endif

/*----------------------------------------------------------------------------*/
/* websocket/socket/close.c                                                   */
/*----------------------------------------------------------------------------*/

int websocket_close(const int sock_fd);

/*----------------------------------------------------------------------------*/
/* websocket/server/loop.c                                                    */
/*----------------------------------------------------------------------------*/

bool websocket_server_loop(int server_sock, const size_t client_buffer_size, PWebSocketCallback callback);

/*----------------------------------------------------------------------------*/
/* util/log.c                                                                 */
/*----------------------------------------------------------------------------*/

// void set_log_level(LOG_LEVEL level);
// #define LOG_LEVEL_DEBUG
// #define LOG_LEVEL_INFO
// #define LOG_LEVEL_ERROR
//
void log_dump_local(const int fd, const char* str);
void var_dump_local(const int fd, const char* str, int value);
void str_dump_local(const int fd, const char* str, const char* value);

#if !defined(LOG_LEVEL_DEBUG) && !defined(LOG_LEVEL_INFO) && !defined(LOG_LEVEL_ERROR)
#define LOG_LEVEL_ERROR
#endif

#ifdef LOG_LEVEL_DEBUG
#define hex_dump(data, size) hex_dump_local(data, size)
#define log_dump(fd, str) log_dump_local(fd, str)
#define log_debug(str) log_dump(STDOUT_FILENO, str)
#define log_info(str) log_dump(STDOUT_FILENO, str)
#define log_error(str) log_dump(STDERR_FILENO, str)
#define var_dump(fd, str, value) var_dump_local(fd, str, value)
#define var_debug(str, value) var_dump(STDOUT_FILENO, str, value)
#define var_info(str, value) var_dump(STDOUT_FILENO, str, value)
#define var_error(str, value) var_dump(STDERR_FILENO, str, value)
#define str_debug(str, value) str_dump_local(STDOUT_FILENO, str, value)
#define str_info(str, value) str_dump_local(STDOUT_FILENO, str, value)
#define str_error(str, value) str_dump_local(STDERR_FILENO, str, value)

#elif defined(LOG_LEVEL_INFO)
#define hex_dump(data, size) hex_dump_local(data, size)
#define log_dump(fd, str) log_dump_local(fd, str)
#define log_debug(str)
#define log_info(str) log_dump(STDOUT_FILENO, str)
#define log_error(str) log_dump(STDERR_FILENO, str)
#define var_dump(fd, str, value) var_dump_local(fd, str, value)
#define var_debug(str, value)
#define var_info(str, value) var_dump(STDOUT_FILENO, str, value)
#define var_error(str, value) var_dump(STDERR_FILENO, str, value)
#define str_debug(str, value)
#define str_info(str, value) str_dump_local(STDOUT_FILENO, str, value)
#define str_error(str, value) str_dump_local(STDERR_FILENO, str, value)

#elif defined(LOG_LEVEL_ERROR)
#define hex_dump(data, size) hex_dump_local(data, size)
#define log_dump(fd, str) log_dump_local(fd, str)
#define log_debug(str)
#define log_info(str)
#define log_error(str) log_dump(STDERR_FILENO, str)
#define var_dump(fd, str, value) var_dump_local(fd, str, value)
#define var_debug(str, value)
#define var_info(str, value)
#define var_error(str, value) var_dump(STDERR_FILENO, str, value)
#define str_debug(str, value)
#define str_info(str, value)
#define str_error(str, value) str_dump_local(STDERR_FILENO, str, value)

#elif
#define hex_dump(data, size)
#define log_dump(fd, str)
#define log_debug(str)
#define log_info(str)
#define log_error(str)
#define var_dump(fd, str, value)
#define var_debug(str, value)
#define var_info(str, value)
#define var_error(str, value)
#define str_debug(str, value)
#define str_info(str, value)
#define str_error(str, value)
#endif

#endif
