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
        rtn = WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
        goto FINALIZE;
    }

    websocket_frame_dump(&frame);

    if (!opcode_handle(client_sock, buffer_capacity, response_buffer, callback, &frame)) {
        rtn = WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
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
        if (client_sock == WEBSOCKET_ERRORCODE_FATAL_ERROR) {
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
        if (client_sock == WEBSOCKET_ERRORCODE_FATAL_ERROR) {
            err = true;
        }

        websocket_epoll_del(epoll_fd, client_sock);
        goto FINALIZE;
    }

    if (!client_handshake(client_sock, buffer_capacity, bytes_read, request_buffer, response_buffer, &request)) {
        websocket_epoll_del(epoll_fd, client_sock);
        goto FINALIZE;
    }

FINALIZE:
    FREE_HTTP_REQUEST(request, websocket_free);

    if (client_sock <= 0 || err) {
        log_debug("websocket_accept error. finalize...\n");
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

    memset(request_buffer, 0x00, client_buffer_capacity);
    memset(response_buffer, 0x00, client_buffer_capacity);

    while (1) {
        int nfds = websocket_epoll_wait(epoll_fd, events, MAX_EVENTS);
        if (nfds <= 0) {
            if (nfds != WEBSOCKET_ERRORCODE_FATAL_ERROR) {
                continue;
            }

            goto FINALIZE;
        }

        var_debug("Escaped from epoll wait. nfds: ", nfds);

        for (int i = 0; i < nfds; ++i) {
            websocket_epoll_event_dump(events[i].events);

            if (events[i].data.fd == server_sock) {
                if (events[i].events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) {
                    goto FINALIZE;
                }

                if (!(events[i].events & (EPOLLIN))) {
                    continue;
                }

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
                if (events[i].events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) {
                    var_error("Client disconnected. : ", client_sock);
                    websocket_epoll_del(epoll_fd, client_sock);
                    websocket_close(client_sock);
                    continue;
                }

                if (!(events[i].events & (EPOLLIN))) {
                    continue;
                }

                ssize_t bytes_read = websocket_recv(client_sock, client_buffer_capacity, request_buffer);
                if (bytes_read <= 0) {
                    if (bytes_read == WEBSOCKET_ERRORCODE_FATAL_ERROR) {
                        websocket_epoll_del(epoll_fd, client_sock);
                        websocket_close(client_sock);
                    }

                    continue;
                }

                int ret = client_recv_func(client_sock, client_buffer_capacity, bytes_read, request_buffer, response_buffer, callback);
                switch (ret) {
                    case WEBSOCKET_ERRORCODE_FATAL_ERROR:
                        websocket_epoll_del(epoll_fd, client_sock);
                        websocket_close(client_sock);
                        goto FINALIZE;
                    case WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR:
                        websocket_epoll_del(epoll_fd, client_sock);
                        websocket_close(client_sock);
                        break;
                    default:
                        break;
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
