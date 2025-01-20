#ifndef NOSTR_SERVER_LOOP_OPCODE_HANDLE_H_
#define NOSTR_SERVER_LOOP_OPCODE_HANDLE_H_

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "../../../util/allocator.h"
#include "../../../util/log.h"
#include "../../../util/signal.h"
#include "../../websocket.h"

static inline bool opcode_handle(
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
            return false;
        case WEBSOCKET_OP_CODE_PING: {
            frame->mask   = 0;
            frame->opcode = WEBSOCKET_OP_CODE_PONG;

            size_t frame_size = create_websocket_frame(
                frame, buffer_capacity, (uint8_t*)&response_buffer[0]);
            if (frame_size == 0) {
                log_error("Failed to create pong frame.\n");
                return false;
            }
            websocket_send(client_sock, response_buffer, frame_size);
            break;
        }
        case WEBSOCKET_OP_CODE_PONG:
            break;
        default:
            var_error("Unknown op code: ", frame->opcode);
            return false;
    }

    return true;
}

#endif
