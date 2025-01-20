#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdint.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "../../util/log.h"
#include "../websocket.h"
#include "optimize_socket.h"

int websocket_listen(const int port_num, const int backlog)
{
    int                server_sock;
    struct sockaddr_in server_addr;

    server_sock = syscall(SYS_socket, AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        log_error("Failed to create socket.\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(port_num);

    bool err = false;

    if (syscall(SYS_bind, server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        log_error("Failed to bind.\n");
        err = true;
        goto FINALIZE;
    }

    if (syscall(SYS_listen, server_sock, backlog) < 0) {
        log_error("Failed to listen.\n");
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
        syscall(SYS_close, server_sock);
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    return server_sock;
}
