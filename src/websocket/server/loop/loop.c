#include "../../../util/allocator.h"
#include "../../../util/log.h"
#include "../../../util/string.h"
#include "../../websocket.h"
#include "accept_handle.h"
#include "receive_handle.h"

int server_epoll_loop(
    const PWebSocketEpollEvent restrict epoll_events,
    const int                           epoll_fd,
    const int                           server_sock,
    const size_t                        client_buffer_capacity,
    char* restrict                      request_buffer,
    char* restrict                      response_buffer,
    const PWebSocketEpollEvent restrict register_event)
{
    if (epoll_events->events & WEBSOCKET_EPOLL_ERR) {
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    if (!(epoll_events->events & WEBSOCKET_EPOLL_IN)) {
        return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
    }

    if (!server_accept_handle(
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

int client_epoll_loop(
    const PWebSocketEpollEvent restrict epoll_events,
    const int                           epoll_fd,
    const int                           client_buffer_capacity,
    char** restrict                     request_buffers,
    const int                           num_of_buffer,
    char* restrict                      response_buffer,
    PWebSocketCallback                  callback)
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

        int ret = server_receive_handle(
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

bool websocket_server_loop(int server_sock, const size_t client_buffer_capacity, PWebSocketCallback callback)
{
    int epoll_fd = websocket_epoll_create();
    if (epoll_fd < 0) {
        return false;
    }

    const size_t        MAX_EVENTS = 16384;
    WebSocketEpollEvent register_event;
    WebSocketEpollEvent epoll_events[MAX_EVENTS];

    memset(&register_event, 0x00, sizeof(register_event));
    memset(epoll_events, 0x00, sizeof(epoll_events));

    if (!websocket_epoll_add(epoll_fd, server_sock, &register_event)) {
        websocket_close(epoll_fd);
        return false;
    }

    var_debug("websocket server fd : ", server_sock);
    var_debug("websocket epoll  fd : ", epoll_fd);

    const int num_of_buffer = 64;
    char*     request_buffers[num_of_buffer];
    bool      alloc_error = false;

    for (int i = 0; i < num_of_buffer; i++) {
        request_buffers[i] = websocket_alloc(client_buffer_capacity);

        if (is_null(request_buffers[i])) {
            alloc_error = true;
            break;
        }

        memset(request_buffers[i], 0x00, sizeof(client_buffer_capacity));
    }

    char* response_buffer = websocket_alloc(client_buffer_capacity);

    if (alloc_error || !response_buffer) {
        log_error("Failed to allocate buffers\n");
        websocket_close(epoll_fd);
        return false;
    }

    memset(response_buffer, 0x00, client_buffer_capacity);

    while (1) {
        int num_of_events = websocket_epoll_wait(epoll_fd, epoll_events, MAX_EVENTS);
        if (num_of_events <= 0) {
            if (num_of_events != WEBSOCKET_ERRORCODE_FATAL_ERROR) {
                continue;
            }

            goto FINALIZE;
        }

        var_debug("Escaped from epoll wait. nfds: ", nfds);

        for (int i = 0; i < num_of_events; ++i) {
            websocket_epoll_event_dump(epoll_events[i].events);

            if (epoll_events[i].data.fd == server_sock) {
                int ret = server_epoll_loop(
                    &epoll_events[i],
                    epoll_fd,
                    server_sock,
                    client_buffer_capacity,
                    request_buffers[0],
                    response_buffer,
                    &register_event);

                if (ret == WEBSOCKET_ERRORCODE_FATAL_ERROR) {
                    goto FINALIZE;
                }

                if (ret == WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR) {
                    continue;
                }
            } else {
                int client_sock = epoll_events->data.fd;

                int ret = client_epoll_loop(
                    epoll_events,
                    epoll_fd,
                    client_buffer_capacity,
                    request_buffers,
                    num_of_buffer,
                    response_buffer,
                    callback);

                if (ret == WEBSOCKET_ERRORCODE_FATAL_ERROR) {
                    websocket_epoll_del(epoll_fd, client_sock);
                    websocket_close(client_sock);
                    goto FINALIZE;
                }

                if (ret == WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR) {
                    websocket_epoll_del(epoll_fd, client_sock);
                    websocket_close(client_sock);
                    continue;
                }

                if (ret == WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR) {
                    continue;
                }
            }
        }
    }

FINALIZE:
    for (int i = 0; i < num_of_buffer; i++) {
        websocket_free(request_buffers);
    }
    websocket_free(response_buffer);
    websocket_epoll_del(epoll_fd, server_sock);
    websocket_close(epoll_fd);
    return true;
}
