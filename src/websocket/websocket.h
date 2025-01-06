#ifndef _NOSTR_WEBSOCKET_H_
#define _NOSTR_WEBSOCKET_H_

/**
 * @file  websocket.h
 *
 * @brief Parses each parameter of a websocket frame stored in network byte order.
 * @see RFC6455 (https://datatracker.ietf.org/doc/html/rfc6455)
 */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "../http/http.h"

typedef struct _WebsocketFrame {
    uint8_t  fin;
    uint8_t  rsv1;
    uint8_t  rsv2;
    uint8_t  rsv3;
    uint8_t  opcode;
    uint8_t  mask;
    uint8_t  payload_len;
    uint64_t ext_payload_len;
    uint8_t  masking_key[4];
    char*    payload;
} WebsocketFrame, *PWebsocketFrame;

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
bool   parseWebsocketFrame(const uint8_t* raw, const size_t frame_size, PWebsocketFrame frame);
size_t createWebsocketFrame(PWebsocketFrame frame, const size_t capacity, uint8_t* raw);

/*----------------------------------------------------------------------------*/
/* websocket_crypto.c                                                         */
/*----------------------------------------------------------------------------*/

bool generate_websocket_acceptkey(const char* client_key, const size_t accept_key_size, char* accept_key);

/*----------------------------------------------------------------------------*/
/* websocket_server.c                                                         */
/*----------------------------------------------------------------------------*/

int websocket_server_init(const int port_num, const int backlog);
int websocket_server_close(const int server_sock);

/*----------------------------------------------------------------------------*/
/* websocket_client.c                                                         */
/*----------------------------------------------------------------------------*/

bool websocket_client_loop(int server_sock, const size_t client_buffer_size);

/*----------------------------------------------------------------------------*/
/* websocket_handshake.c                                                      */
/*----------------------------------------------------------------------------*/

bool  is_valid_websocket_request(PHTTPRequest request);
char* select_websocket_client_key(PHTTPRequest request);
bool  create_handshake_ok_flame(const char* accept_key, const size_t capacity, char* buffer);

/*----------------------------------------------------------------------------*/
/* websocket_log.c                                                            */
/*----------------------------------------------------------------------------*/

void websocket_frame_dump(PWebsocketFrame frame);

#endif
