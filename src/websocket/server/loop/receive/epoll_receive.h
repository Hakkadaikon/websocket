#ifndef NOSTR_SERVER_LOOP_EPOLL_RECEIVE_H_
#define NOSTR_SERVER_LOOP_EPOLL_RECEIVE_H_

#include "../../../../util/allocator.h"
#include "../../../../util/log.h"
#include "../../../../util/string.h"
#include "../../../websocket.h"
#include "receive_handle.h"

static inline int epoll_receive(
    const PWebSocketEpollEvent epoll_events,
    const int                  epoll_fd,
    const int                  client_buffer_capacity,
    char**                     request_buffers,
    const int                  num_of_buffer,
    char*                      response_buffer,
    PWebSocketCallback         callback)
{
    int client_sock = epoll_events->data.fd;
    if (epoll_events->events & (WEBSOCKET_EPOLL_ERR)) {
        var_error("Client disconnected. : ", client_sock);
        return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
    }

    if (!(epoll_events->events & WEBSOCKET_EPOLL_IN)) {
        return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
    }

    ssize_t read_count = websocket_recvmmsg(
        client_sock,
        client_buffer_capacity,
        request_buffers,
        num_of_buffer);

    if (read_count <= 0) {
        if (read_count == WEBSOCKET_ERRORCODE_FATAL_ERROR) {
            return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
        }

        return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
    }

    var_debug("read count : ", read_count);

    for (int i = 0; i < read_count; i++) {
        size_t client_buffer_length = strnlen(request_buffers[i], client_buffer_capacity);

        int ret = receive_handle(
            client_sock,
            client_buffer_length,
            request_buffers[i],
            response_buffer,
            callback);

        if (ret == WEBSOCKET_ERRORCODE_FATAL_ERROR || ret == WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR) {
            return ret;
        }
    }

    return WEBSOCKET_ERRORCODE_NONE;
}

#endif
