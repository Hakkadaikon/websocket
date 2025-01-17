#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "../../util/allocator.h"
#include "../../util/log.h"
#include "../../util/signal.h"
#include "../websocket.h"

static inline bool opcode_handle(
    const int                client_sock,
    const size_t             buffer_capacity,
    char* restrict           response_buffer,
    PWebSocketCallback       callback,
    PWebSocketFrame restrict frame)
{
    switch (frame->opcode) {
        case WEBSOCKET_OP_CODE_TEXT:
        case WEBSOCKET_OP_CODE_BINARY:
            callback(client_sock, frame, buffer_capacity, response_buffer);
            break;
        case WEBSOCKET_OP_CODE_CLOSE:
            return false;
        case WEBSOCKET_OP_CODE_PING:
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
        case WEBSOCKET_OP_CODE_PONG:
            break;
        default:
            var_error("Unknown op code: ", frame->opcode);
            break;
    }

    return true;
}

static inline int client_recv_func(
    const int          client_sock,
    const size_t       buffer_capacity,
    const ssize_t      bytes_read,
    char* restrict     request_buffer,
    char* restrict     response_buffer,
    PWebSocketCallback callback)
{
    WebSocketFrame frame;
    memset(&frame, 0x00, sizeof(frame));

    frame.payload = websocket_alloc(bytes_read);

    int rtn = 0;

    if (!parse_websocket_frame((uint8_t*)request_buffer, bytes_read, &frame)) {
        rtn = -1;
        goto FINALIZE;
    }

    websocket_frame_dump(&frame);

    if (!opcode_handle(client_sock, buffer_capacity, response_buffer, callback, &frame)) {
        rtn = -2;
        goto FINALIZE;
    }

    if (is_rise_signal()) {
        var_info("rise signal. sock : ", client_sock);
        rtn = -2;
        goto FINALIZE;
    }

FINALIZE:
    websocket_free(frame.payload);
    return rtn;
}

static inline bool server_accept_func(
    const int            epoll_fd,
    const int            server_sock,
    const size_t         buffer_capacity,
    char* restrict       request_buffer,
    char* restrict       response_buffer,
    PWebSocketEpollEvent event)
{
    HTTPRequest request;
    ALLOCATE_HTTP_REQUEST(request, websocket_alloc);

    bool err         = false;
    int  client_sock = websocket_accept(server_sock);
    if (client_sock <= 0) {
        if (client_sock == -2) {
            err = true;
        }

        goto FINALIZE;
    }

    if (!websocket_epoll_add(epoll_fd, client_sock, event)) {
        err = true;
        goto FINALIZE;
    }

    ssize_t bytes_read = websocket_recv(client_sock, buffer_capacity, request_buffer);
    if (bytes_read <= 0) {
        err = true;
        goto FINALIZE;
    }

    if (!client_handshake(client_sock, buffer_capacity, bytes_read, request_buffer, response_buffer, &request)) {
        err = true;
        goto FINALIZE;
    }

FINALIZE:
    FREE_HTTP_REQUEST(request, websocket_free);

    if (client_sock <= 0 || err) {
        log_debug("websocket_accept error. finalize...");
        websocket_close(client_sock);
        return false;
    }

    return true;
}

bool websocket_server_loop(int server_sock, const size_t client_buffer_capacity, PWebSocketCallback callback)
{
    int epoll_fd = websocket_epoll_create();
    if (epoll_fd < 0) {
        return false;
    }

    const size_t        MAX_EVENTS = 16384;
    WebSocketEpollEvent register_event;
    WebSocketEpollEvent events[MAX_EVENTS];

    memset(&register_event, 0x00, sizeof(register_event));
    memset(events, 0x00, sizeof(events));

    if (!websocket_epoll_add(epoll_fd, server_sock, &register_event)) {
        websocket_close(epoll_fd);
        return false;
    }

    var_debug("websocket server fd : ", server_sock);
    var_debug("websocket epoll  fd : ", epoll_fd);

    char* request_buffer  = websocket_alloc(client_buffer_capacity);
    char* response_buffer = websocket_alloc(client_buffer_capacity);

    if (!request_buffer || !response_buffer) {
        log_error("Failed to allocate buffers\n");
        websocket_close(epoll_fd);
        return false;
    }

    while (1) {
        log_debug("loop.\n");

        int nfds = websocket_epoll_wait(epoll_fd, events, MAX_EVENTS);
        if (nfds <= 0) {
            if (nfds == -1) {
                continue;
            }

            goto FINALIZE;
        }

        var_debug("Escaped from epoll wait. nfds: ", nfds);

        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == server_sock) {
                if (!server_accept_func(
                        epoll_fd,
                        server_sock,
                        client_buffer_capacity,
                        request_buffer,
                        response_buffer,
                        &register_event)) {
                    goto FINALIZE;
                }
            } else {
                int client_sock = events[i].data.fd;

                ssize_t bytes_read = websocket_recv(client_sock, client_buffer_capacity, request_buffer);
                if (bytes_read <= 0) {
                    log_debug("Client disconnected or error.\n");
                    websocket_epoll_del(epoll_fd, client_sock);
                    websocket_close(client_sock);
                    goto FINALIZE;
                }

                int ret = client_recv_func(client_sock, client_buffer_capacity, bytes_read, request_buffer, response_buffer, callback);
                if (ret == -2) {
                    websocket_epoll_del(epoll_fd, client_sock);
                    websocket_close(client_sock);
                    goto FINALIZE;
                }
            }
        }
    }

FINALIZE:
    websocket_free(request_buffer);
    websocket_free(response_buffer);
    websocket_epoll_del(epoll_fd, server_sock);
    websocket_close(epoll_fd);
    return true;
}
