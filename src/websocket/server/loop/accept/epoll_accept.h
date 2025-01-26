#ifndef NOSTR_SERVER_LOOP_EPOLL_ACCEPT_H_
#define NOSTR_SERVER_LOOP_EPOLL_ACCEPT_H_

#include <string.h>

#include "../../../websocket_local.h"
#include "accept_handle.h"

static inline int epoll_accept(
    const PWebSocketEpollEvent epoll_events,
    const int                  epoll_fd,
    const int                  server_sock,
    const size_t               client_buffer_capacity,
    char*                      request_buffer,
    char*                      response_buffer,
    const PWebSocketEpollEvent register_event)
{
    int code = websocket_epoll_rise_error(epoll_events);
    if (code != WEBSOCKET_ERRORCODE_NONE) {
        return code;
    }

    code = websocket_epoll_rise_input(epoll_events);
    if (code != WEBSOCKET_ERRORCODE_NONE) {
        return code;
    }

    if (!accept_handle(
            epoll_fd,
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
