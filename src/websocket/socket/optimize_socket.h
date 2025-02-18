#ifndef NOSTR_OPTIMIZE_SOCKET_H_
#define NOSTR_OPTIMIZE_SOCKET_H_

#include "../../arch/optimize_socket.h"
#include "../websocket_local.h"

static inline int set_nonblocking(int fd)
{
    long flags = internal_fcntl(fd, F_GETFL, 0);
    if (flags == WEBSOCKET_SYSCALL_ERROR) {
        return WEBSOCKET_SYSCALL_ERROR;  // Failed to get flags
    }

    return internal_fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static inline int optimize_client_socket(int client_sock)
{
    int rtn = WEBSOCKET_ERRORCODE_NONE;
    int param;

    param = 1;
    if (internal_setsockopt(client_sock, IPPROTO_TCP, TCP_NODELAY, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to setsockopt[IPPROTO_TCP/TCP_NODELAY] reason : ", strerror(errno));
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }

    param = 1;
    if (internal_setsockopt(client_sock, SOL_SOCKET, SO_REUSEADDR, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to setsockopt[SOL_SOCKET/SO_REUSEADDR] reason : ", strerror(errno));
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }

#if defined(SO_REUSEPORT)
    param = 1;
    if (internal_setsockopt(client_sock, SOL_SOCKET, SO_REUSEPORT, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to setsockopt[SOL_SOCKET/SO_REUSEPORT] reason : ", strerror(errno));
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }
#endif

FINALIZE:
    return rtn;
}

static inline int optimize_server_socket(int server_sock)
{
    int rtn   = WEBSOCKET_ERRORCODE_NONE;
    int param = 1;

    param = 1;
    if (internal_setsockopt(server_sock, IPPROTO_TCP, TCP_NODELAY, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to setsockopt[IPPROTO_TCP/TCP_NODELAY] reason : ", strerror(errno));
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }

    param = 1;
    if (internal_setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to setsockopt[SOL_SOCKET/SO_REUSEADDR] reason : ", strerror(errno));
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }

#if defined(SO_REUSEPORT)
    param = 1;
    if (internal_setsockopt(server_sock, SOL_SOCKET, SO_REUSEPORT, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to setsockopt[SOL_SOCKET/SO_REUSEPORT] reason : ", strerror(errno));
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }
#endif

#ifndef __APPLE__
    param = 5;
    if (internal_setsockopt(server_sock, IPPROTO_TCP, TCP_FASTOPEN, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to setsockopt[IPPROTO_TCP/TCP_FASTOPEN] reason : ", strerror(errno));
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }
#endif

FINALIZE:
    return rtn;
}

#endif
