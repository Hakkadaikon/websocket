#ifndef NOSTR_SERVER_LOOP_RECEIVE_H_
#define NOSTR_SERVER_LOOP_RECEIVE_H_

#include "../../../util/allocator.h"
#include "../../../util/log.h"
#include "../../../util/signal.h"
#include "../../websocket.h"
#include "opcode_handle.h"

static inline int server_receive_handle(
    const int          client_sock,
    const size_t       buffer_capacity,
    char*              request_buffer,
    char*              response_buffer,
    PWebSocketCallback callback)
{
    WebSocketFrame frame;
    memset(&frame, 0x00, sizeof(frame));

    frame.payload = (char*)websocket_alloc(buffer_capacity);

    int rtn = 0;

    if (!parse_websocket_frame((uint8_t*)request_buffer, buffer_capacity, &frame)) {
        rtn = WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
        goto FINALIZE;
    }

    websocket_frame_dump(&frame);

    rtn = opcode_handle(client_sock, buffer_capacity, response_buffer, callback, &frame);
    if (rtn != WEBSOCKET_ERRORCODE_NONE) {
        goto FINALIZE;
    }

    if (is_rise_signal()) {
        var_info("rise signal. sock : ", client_sock);
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }

FINALIZE:
    websocket_free(frame.payload);
    return rtn;
}

#endif
