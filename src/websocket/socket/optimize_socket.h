#ifndef NOSTR_OPTIMIZE_SOCKET_H_
#define NOSTR_OPTIMIZE_SOCKET_H_

#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/syscall.h>

#include "../../util/log.h"
#include "../websocket.h"

static inline int set_nonblocking(int fd)
{
    long flags = syscall(SYS_fcntl, fd, F_GETFL);
    if (flags == WEBSOCKET_SYSCALL_ERROR) {
        return WEBSOCKET_SYSCALL_ERROR;  // Failed to get flags
    }

    return syscall(SYS_fcntl, fd, F_SETFL, flags | O_NONBLOCK);
}

static inline int optimize_client_socket(int client_sock)
{
    int rtn = WEBSOCKET_ERRORCODE_NONE;
    int param;

    param = 1;
    if (syscall(SYS_setsockopt, client_sock, IPPROTO_TCP, TCP_NODELAY, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }

    param = 1;
    if (syscall(SYS_setsockopt, client_sock, SOL_SOCKET, SO_REUSEADDR, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }

#if defined(SO_REUSEPORT)
    param = 1;
    if (syscall(SYS_setsockopt, client_sock, SOL_SOCKET, SO_REUSEPORT, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
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
    if (syscall(SYS_setsockopt, server_sock, IPPROTO_TCP, TCP_NODELAY, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }

    param = 1;
    if (syscall(SYS_setsockopt, server_sock, SOL_SOCKET, SO_REUSEADDR, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }

#if defined(SO_REUSEPORT)
    param = 1;
    if (syscall(SYS_setsockopt, server_sock, SOL_SOCKET, SO_REUSEPORT, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }
#endif

    param = 5;
    if (syscall(SYS_setsockopt, server_sock, IPPROTO_TCP, TCP_FASTOPEN, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }

FINALIZE:
    return rtn;
}

#endif
