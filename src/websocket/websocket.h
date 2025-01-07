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

typedef enum {
    WEBSOCKET_OP_CODE_TEXT   = 0x1,
    WEBSOCKET_OP_CODE_BINARY = 0x2,
    WEBSOCKET_OP_CODE_CLOSE  = 0x8,
    WEBSOCKET_OP_CODE_PING   = 0x9,
    WEBSOCKET_OP_CODE_PONG   = 0xA,
} WebSocketOpCode;

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
int websocket_server_close(const int server_sock);

/*----------------------------------------------------------------------------*/
/* websocket/server/loop.c                                                    */
/*----------------------------------------------------------------------------*/

bool websocket_server_loop(int server_sock, const size_t client_buffer_size);

/*----------------------------------------------------------------------------*/
/* websocket/handshake.c                                                      */
/*----------------------------------------------------------------------------*/

bool  is_valid_websocket_request(PHTTPRequest request);
char* select_websocket_client_key(PHTTPRequest request);
bool  create_server_handshake_ok_frame(const char* accept_key, const size_t capacity, char* buffer);

/*----------------------------------------------------------------------------*/
/* websocket/log.c                                                            */
/*----------------------------------------------------------------------------*/

void websocket_frame_dump(PWebSocketFrame frame);

#endif
