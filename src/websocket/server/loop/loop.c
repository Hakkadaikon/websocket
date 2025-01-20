#include "../../../util/allocator.h"
#include "../../../util/log.h"
#include "../../websocket.h"
#include "accept_handle.h"
#include "receive_handle.h"

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

    const int num_of_buffer = 64;
    char*     request_buffers[num_of_buffer];
    bool      alloc_error = false;

    for (int i = 0; i < num_of_buffer; i++) {
        request_buffers[i] = websocket_alloc(client_buffer_capacity);

        if (request_buffers[i] == NULL) {
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
                if (events[i].events & WEBSOCKET_EPOLL_ERR) {
                    goto FINALIZE;
                }

                if (!(events[i].events & WEBSOCKET_EPOLL_IN)) {
                    continue;
                }

                if (!server_accept_handle(
                        epoll_fd,
                        server_sock,
                        client_buffer_capacity,
                        request_buffers[0],
                        response_buffer,
                        &register_event)) {
                    goto FINALIZE;
                }
            } else {
                int client_sock = events[i].data.fd;
                if (events[i].events & (WEBSOCKET_EPOLL_ERR)) {
                    var_error("Client disconnected. : ", client_sock);
                    websocket_epoll_del(epoll_fd, client_sock);
                    websocket_close(client_sock);
                    continue;
                }

                if (!(events[i].events & WEBSOCKET_EPOLL_IN)) {
                    continue;
                }

                ssize_t read_count = websocket_recvmmsg(
                    client_sock,
                    client_buffer_capacity,
                    request_buffers,
                    num_of_buffer);

                if (read_count <= 0) {
                    if (read_count == WEBSOCKET_ERRORCODE_FATAL_ERROR) {
                        websocket_epoll_del(epoll_fd, client_sock);
                        websocket_close(client_sock);
                    }

                    continue;
                }

                var_debug("read count : ", read_count);

                size_t client_buffer_length = strnlen(request_buffers[i], client_buffer_capacity);

                for (int i = 0; i < read_count; i++) {
                    int ret = server_receive_handle(
                        client_sock,
                        client_buffer_length,
                        request_buffers[i],
                        response_buffer,
                        callback);

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
