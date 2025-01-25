#ifndef NOSTR_SERVER_LOOP_OPCODE_HANDLE_H_
#define NOSTR_SERVER_LOOP_OPCODE_HANDLE_H_

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../../../util/allocator.h"
#include "../../../../util/log.h"
#include "../../../../util/signal.h"
#include "../../../websocket.h"

static inline int opcode_handle(
    const int          client_sock,
    const size_t       buffer_capacity,
    char*              response_buffer,
    PWebSocketCallback callback,
    PWebSocketFrame    frame)
{
    switch (frame->opcode) {
        case WEBSOCKET_OP_CODE_TEXT:
        case WEBSOCKET_OP_CODE_BINARY:
            callback(client_sock, frame, buffer_capacity, response_buffer);
            break;
        case WEBSOCKET_OP_CODE_CLOSE:
            return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
        case WEBSOCKET_OP_CODE_PING: {
            frame->mask   = 0;
            frame->opcode = WEBSOCKET_OP_CODE_PONG;

            size_t frame_size = create_websocket_frame(frame, buffer_capacity, response_buffer);
            if (frame_size == 0) {
                log_error("Failed to create pong frame.\n");
                return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
            }

            int rtn = websocket_send(client_sock, response_buffer, frame_size);
            if (rtn != WEBSOCKET_ERRORCODE_NONE) {
                return rtn;
            }

            break;
        }
        case WEBSOCKET_OP_CODE_PONG:
            break;
        default:
            var_error("Unknown op code: ", frame->opcode);
            return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
    }

    return WEBSOCKET_ERRORCODE_NONE;
}

#endif
