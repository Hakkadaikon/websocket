#include "../../arch/listen.h"

#include "../websocket_local.h"
#include "./optimize_socket.h"

int websocket_listen(const int port_num, const int backlog)
{
    int                server_sock;
    struct sockaddr_in server_addr;

    server_sock = internal_socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        str_error("Failed to socket(). reason : ", strerror(errno));
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    websocket_memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(port_num);

    bool err = false;

    if (internal_bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        str_error("Failed to bind(). reason : ", strerror(errno));
        err = true;
        goto FINALIZE;
    }

    if (internal_listen(server_sock, backlog) < 0) {
        str_error("Failed to listen(). reason : ", strerror(errno));
        err = true;
        goto FINALIZE;
    }

    if (set_nonblocking(server_sock) == WEBSOCKET_SYSCALL_ERROR) {
        log_error("Failed to set server socket non-blocking\n");
        err = true;
        goto FINALIZE;
    }

    if (optimize_server_socket(server_sock) == WEBSOCKET_ERRORCODE_FATAL_ERROR) {
        err = true;
        goto FINALIZE;
    }

FINALIZE:
    if (err) {
        websocket_close(server_sock);
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    return server_sock;
}
