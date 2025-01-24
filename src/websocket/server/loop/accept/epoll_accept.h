#ifndef NOSTR_SERVER_LOOP_EPOLL_ACCEPT_H_
#define NOSTR_SERVER_LOOP_EPOLL_ACCEPT_H_

#include "../../../../util/log.h"
#include "../../../websocket.h"
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
#ifndef __APPLE__
    if (epoll_events->events & WEBSOCKET_EPOLL_ERROR) {
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    if (!(epoll_events->events & WEBSOCKET_EPOLL_IN)) {
        return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
    }
#endif

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
