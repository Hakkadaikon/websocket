#ifndef _NOSTR_WEBSOCKET_H_
#define _NOSTR_WEBSOCKET_H_

/**
 * @file  websocket.h
 *
 * @brief Parses each parameter of a websocket frame stored in network byte order.
 * @see RFC6455 (https://datatracker.ietf.org/doc/html/rfc6455)
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../http/http.h"

#define WEBSOCKET_EPOLL_ERR (EPOLLHUP | EPOLLERR | EPOLLRDHUP)
#define WEBSOCKET_EPOLL_IN (EPOLLIN)

typedef struct epoll_event WebSocketEpollEvent, *PWebSocketEpollEvent;

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

typedef enum {
    WEBSOCKET_SYSCALL_ERROR = -1
} WebSocketSysCallErrorCode;

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
bool   parse_websocket_frame(const uint8_t* raw, const size_t frame_size, PWebSocketFrame frame);
size_t create_websocket_frame(PWebSocketFrame frame, const size_t capacity, uint8_t* raw);

/*----------------------------------------------------------------------------*/
/* websocket/crypto.c                                                         */
/*----------------------------------------------------------------------------*/

bool generate_websocket_acceptkey(const char* client_key, const size_t accept_key_size, char* accept_key);

/*----------------------------------------------------------------------------*/
/* websocket/server/init.c                                                    */
/*----------------------------------------------------------------------------*/

int websocket_server_init(const int port_num, const int backlog);

/*----------------------------------------------------------------------------*/
/* websocket/func.c                                                           */
/*----------------------------------------------------------------------------*/

int     websocket_close(const int sock_fd);
int     websocket_send(const int sock_fd, const char* buffer, const size_t buffer_size);
ssize_t websocket_recv(const int sock_fd, const size_t capacity, char* buffer);
ssize_t websocket_recvmmsg(const int sock_fd, const size_t capacity, char** buffers, const int num_of_buffer);
int     websocket_accept(const int sock_fd);
int     websocket_connect(const int port_num, const int backlog);
bool    websocket_epoll_add(const int epoll_fd, const int sock_fd, PWebSocketEpollEvent event);
bool    websocket_epoll_del(const int epoll_fd, const int sock_fd);
int     websocket_epoll_create();
int     websocket_epoll_wait(const int epoll_fd, PWebSocketEpollEvent events, const int max_events);

/*----------------------------------------------------------------------------*/
/* websocket/server/loop.c                                                    */
/*----------------------------------------------------------------------------*/

bool websocket_server_loop(int server_sock, const size_t client_buffer_size, PWebSocketCallback callback);

/*----------------------------------------------------------------------------*/
/* websocket/handshake.c                                                      */
/*----------------------------------------------------------------------------*/

bool client_handshake(const int client_sock, const size_t buffer_capacity, const size_t bytes_read, char* request_buffer, char* response_buffer, PHTTPRequest request);

/*----------------------------------------------------------------------------*/
/* websocket/log.c                                                            */
/*----------------------------------------------------------------------------*/

void websocket_frame_dump(PWebSocketFrame frame);
void websocket_epoll_event_dump(const int events);

#endif
