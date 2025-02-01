#ifndef NOSTR_SERVER_LOOP_EPOLL_RECEIVE_H_
#define NOSTR_SERVER_LOOP_EPOLL_RECEIVE_H_

#include "../../../../util/allocator.h"
#include "../../../websocket_local.h"
#include "receive_handle.h"

static inline int epoll_receive(
    const PWebSocketEpollEvent epoll_events,
    const int                  epoll_fd,
    const int                  client_buffer_capacity,
    char*                      request_buffer,
    char*                      response_buffer,
    PWebSocketReceiveCallback  callback)
{
    int code = websocket_epoll_rise_error(epoll_events);
    if (code != WEBSOCKET_ERRORCODE_NONE) {
        return code;
    }

    code = websocket_epoll_rise_input(epoll_events);
    if (code != WEBSOCKET_ERRORCODE_NONE) {
        return code;
    }

    int client_sock = websocket_epoll_getfd(epoll_events);

    ssize_t read_size = websocket_recvfrom(
        client_sock,
        client_buffer_capacity,
        request_buffer);

    if (read_size <= 0) {
        if (read_size == WEBSOCKET_ERRORCODE_FATAL_ERROR || read_size == WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR) {
            return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
        }

        return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
    }

    int ret = receive_handle(
        client_sock,
        read_size,
        request_buffer,
        response_buffer,
        callback);

    if (ret == WEBSOCKET_ERRORCODE_FATAL_ERROR || ret == WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR) {
        return ret;
    }

    return WEBSOCKET_ERRORCODE_NONE;
}

#endif
