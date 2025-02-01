#include "../../../util/allocator.h"
#include "../../websocket_local.h"
#include "accept/epoll_accept.h"
#include "receive/epoll_receive.h"

bool websocket_server_loop(int server_sock, const size_t client_buffer_capacity, PWebSocketReceiveCallback callback)
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
        int num_of_events = websocket_epoll_wait(epoll_fd, epoll_events, MAX_EVENTS);
        if (num_of_events <= 0) {
            if (num_of_events != WEBSOCKET_ERRORCODE_FATAL_ERROR) {
                continue;
            }

            goto FINALIZE;
        }

        var_debug("Escaped from epoll wait. num of events: ", num_of_events);

        for (int i = 0; i < num_of_events; ++i) {
            int fd = websocket_epoll_getfd(&epoll_events[i]);

            if (fd == server_sock) {
                if (epoll_accept(
                        &epoll_events[i],
                        epoll_fd,
                        server_sock,
                        client_buffer_capacity,
                        request_buffer,
                        response_buffer,
                        &register_event) == WEBSOCKET_ERRORCODE_FATAL_ERROR) {
                    goto FINALIZE;
                }

                continue;
            }

            int client_sock = fd;

            int ret = epoll_receive(
                epoll_events,
                epoll_fd,
                client_buffer_capacity,
                request_buffer,
                response_buffer,
                callback);

            if (ret == WEBSOCKET_ERRORCODE_FATAL_ERROR || ret == WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR) {
                websocket_epoll_del(epoll_fd, client_sock);
                websocket_close(client_sock);
            }

            if (ret == WEBSOCKET_ERRORCODE_FATAL_ERROR) {
                goto FINALIZE;
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
