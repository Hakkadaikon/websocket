#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>

#include "../websocket_local.h"
#include "optimize_socket.h"

#ifndef __APPLE__
#ifndef __NR_accept4
#if defined(__x86_64__)
#define __NR_accept4 288
#else
#error "Unsupported architecture for linux_accept4"
#endif
#endif
#endif

static inline int linux_accept4(int sock_fd, struct sockaddr* addr, socklen_t* addrlen, int flags)
{
    long ret;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "0"(__NR_accept4),
          "D"(sock_fd),
          "S"(addr),
          "d"(addrlen),
          "r"(flags)
        : "rcx", "r11", "memory");
    if (ret < 0) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

static inline int internal_accept(int sock_fd, struct sockaddr* addr, socklen_t* addrlen)
{
#ifdef __APPLE__
    return accept(sock_fd, addr, addrlen);
#else
    return linux_accept4(sock_fd, addr, addrlen, 0);
#endif
}

int websocket_accept(const int sock_fd)
{
    struct sockaddr_in client_addr;
    socklen_t          addr_len = sizeof(client_addr);

    if (is_rise_signal()) {
        log_info("A signal was raised during accept(). The system will abort processing.\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    log_debug("accept...\n");
    int client_sock = internal_accept(sock_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_sock < 0) {
        if (errno == EINTR || errno == EAGAIN) {
            return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
        }

        str_error("accept() failed. reason : ", strerror(errno));
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    if (optimize_client_socket(client_sock) == WEBSOCKET_ERRORCODE_FATAL_ERROR) {
        websocket_close(client_sock);
        return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
    }

    return client_sock;
}
