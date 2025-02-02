#ifndef NOSTR_SERVER_LOOP_EPOLL_ACCEPT_H_
#define NOSTR_SERVER_LOOP_EPOLL_ACCEPT_H_

#include <string.h>

#include "../../../websocket_local.h"
#include "accept_handle.h"

static inline int epoll_accept(
    const PWebSocketEpollLoopArgs epoll_args,
    const int                     server_sock,
    const size_t                  client_buffer_capacity,
    char*                         request_buffer,
    char*                         response_buffer,
    const PWebSocketEpollEvent    register_event)
{
    log_debug("rise error check...\n");
    int code = websocket_epoll_rise_error(epoll_args->event);
    if (code != WEBSOCKET_ERRORCODE_NONE) {
        return code;
    }

    log_debug("rise input check...\n");
    code = websocket_epoll_rise_input(epoll_args->event);
    if (code != WEBSOCKET_ERRORCODE_NONE) {
        return code;
    }

    log_debug("accept handle\n");
    if (!accept_handle(
            epoll_args->epoll_fd,
            server_sock,
            client_buffer_capacity,
            request_buffer,
            response_buffer,
            register_event)) {
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    return WEBSOCKET_ERRORCODE_NONE;
}

#endif
