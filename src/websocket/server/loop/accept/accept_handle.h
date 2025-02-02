#ifndef NOSTR_SERVER_LOOP_ACCEPT_H_
#define NOSTR_SERVER_LOOP_ACCEPT_H_

#include "../../../../util/allocator.h"
#include "../../../websocket_local.h"

static inline bool accept_handle(
    const int            epoll_fd,
    const int            server_sock,
    const size_t         buffer_capacity,
    char*                request_buffer,
    char*                response_buffer,
    PWebSocketEpollEvent event)
{
    HTTPRequest request;
    ssize_t     bytes_read;

    ALLOCATE_HTTP_REQUEST(request, websocket_alloc);

    bool err = false;

    log_debug("accept...\n");
    int client_sock = websocket_accept(server_sock);
    if (client_sock <= 0) {
        if (client_sock == WEBSOCKET_ERRORCODE_FATAL_ERROR) {
            err = true;
        }

        goto FINALIZE;
    }

    log_debug("epoll add(client sock)...\n");
    if (!websocket_epoll_add(epoll_fd, client_sock, event)) {
        err = true;
        goto FINALIZE;
    }

    log_debug("Read to handshake message...\n");
    while (1) {
        bytes_read = websocket_recv(client_sock, buffer_capacity, request_buffer);
        if (bytes_read < 0) {
            if (bytes_read == WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR) {
                continue;
            }

            err = true;
            var_info("Failed to handshake message read.\n", client_sock);
            websocket_epoll_del(epoll_fd, client_sock);
            goto FINALIZE;
        }

        break;
    }

    log_debug("Analyze to handshake message...\n");
    if (!client_handshake(client_sock, buffer_capacity, bytes_read, request_buffer, response_buffer, &request)) {
        websocket_epoll_del(epoll_fd, client_sock);
        err = true;
        goto FINALIZE;
    }

FINALIZE:
    FREE_HTTP_REQUEST(request, websocket_free);

    if (client_sock <= 0 || err) {
        log_debug("websocket_accept error. finalize...\n");
        websocket_close(client_sock);
        return false;
    }

    var_debug("accept done. client_sock : ", client_sock);
    return true;
}

#endif
