#include <errno.h>
#include <string.h>
#include <sys/syscall.h>

#include "../websocket_local.h"
#include "optimize_socket.h"

#ifdef __APPLE__
#define SYSCALL_SOCKET(domain, type, protocol) socket(domain, type, protocol)
#define SYSCALL_BIND(fd, addr, addr_len) bind(fd, addr, addr_len)
#define SYSCALL_LISTEN(fd, backlog) listen(fd, backlog)
#define SYSCALL_CLOSE(fd) close(fd)
#else
#define SYSCALL_SOCKET(domain, type, protocol) syscall(SYS_socket, domain, type, protocol)
#define SYSCALL_BIND(fd, addr, addr_len) syscall(SYS_bind, fd, addr, addr_len)
#define SYSCALL_LISTEN(fd, backlog) syscall(SYS_listen, fd, backlog)
#define SYSCALL_CLOSE(fd) syscall(SYS_close, fd)
#endif

int websocket_listen(const int port_num, const int backlog)
{
    int                server_sock;
    struct sockaddr_in server_addr;

    server_sock = SYSCALL_SOCKET(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        str_error("Failed to socket(). reason : ", strerror(errno));
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(port_num);

    bool err = false;

    if (SYSCALL_BIND(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        str_error("Failed to bind(). reason : ", strerror(errno));
        err = true;
        goto FINALIZE;
    }

    if (SYSCALL_LISTEN(server_sock, backlog) < 0) {
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
        SYSCALL_CLOSE(server_sock);
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    return server_sock;
}
