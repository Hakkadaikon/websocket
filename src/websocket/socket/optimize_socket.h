#ifndef NOSTR_OPTIMIZE_SOCKET_H_
#define NOSTR_OPTIMIZE_SOCKET_H_

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/syscall.h>

#include "../websocket_local.h"

#ifdef __APPLE__
#define SYSCALL_FCNTL(fd, ...) fcntl(fd, __VA_ARGS__)
#define SYSCALL_SETSOCKOPT(fd, level, option_name, option_value, option_len) setsockopt(fd, level, option_name, option_value, option_len)
#else
#define SYSCALL_FCNTL(fd, ...) syscall(SYS_fcntl, fd, __VA_ARGS__)
#define SYSCALL_SETSOCKOPT(fd, level, option_name, option_value, option_len) syscall(SYS_setsockopt, fd, level, option_name, option_value, option_len)
#endif

static inline int set_nonblocking(int fd)
{
    long flags = SYSCALL_FCNTL(fd, F_GETFL);
    if (flags == WEBSOCKET_SYSCALL_ERROR) {
        return WEBSOCKET_SYSCALL_ERROR;  // Failed to get flags
    }

    return SYSCALL_FCNTL(fd, F_SETFL, flags | O_NONBLOCK);
}

static inline int optimize_client_socket(int client_sock)
{
    int rtn = WEBSOCKET_ERRORCODE_NONE;
    int param;

    param = 1;
    if (SYSCALL_SETSOCKOPT(client_sock, IPPROTO_TCP, TCP_NODELAY, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to setsockopt[IPPROTO_TCP/TCP_NODELAY] reason : ", strerror(errno));
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }

    param = 1;
    if (SYSCALL_SETSOCKOPT(client_sock, SOL_SOCKET, SO_REUSEADDR, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to setsockopt[SOL_SOCKET/SO_REUSEADDR] reason : ", strerror(errno));
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }

#if defined(SO_REUSEPORT)
    param = 1;
    if (SYSCALL_SETSOCKOPT(client_sock, SOL_SOCKET, SO_REUSEPORT, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
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
    if (SYSCALL_SETSOCKOPT(server_sock, IPPROTO_TCP, TCP_NODELAY, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to setsockopt[IPPROTO_TCP/TCP_NODELAY] reason : ", strerror(errno));
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }

    param = 1;
    if (SYSCALL_SETSOCKOPT(server_sock, SOL_SOCKET, SO_REUSEADDR, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to setsockopt[SOL_SOCKET/SO_REUSEADDR] reason : ", strerror(errno));
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }

#if defined(SO_REUSEPORT)
    param = 1;
    if (SYSCALL_SETSOCKOPT(server_sock, SOL_SOCKET, SO_REUSEPORT, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to setsockopt[SOL_SOCKET/SO_REUSEPORT] reason : ", strerror(errno));
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }
#endif

#ifndef __APPLE__
    param = 5;
    if (SYSCALL_SETSOCKOPT(server_sock, IPPROTO_TCP, TCP_FASTOPEN, &param, sizeof(param)) == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to setsockopt[IPPROTO_TCP/TCP_FASTOPEN] reason : ", strerror(errno));
        rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
        goto FINALIZE;
    }
#endif

FINALIZE:
    return rtn;
}

#endif
