#include <errno.h>
#include <sys/syscall.h>

#include "../websocket_local.h"

static inline int linux_close(int fd)
{
    long ret;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "0"(__NR_close), "D"(fd)
        : "rcx", "r11", "memory");
    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

static inline int internal_close(int fd)
{
#ifdef __APPLE__
    return close(fd);
#else
    return linux_close(fd);
#endif
}

int websocket_close(const int sock_fd)
{
    var_info("WebSocket close...: ", sock_fd);
    if (sock_fd < 0) {
        return WEBSOCKET_ERRORCODE_NONE;
    }

    if (internal_close(sock_fd) == WEBSOCKET_SYSCALL_ERROR) {
        str_info("WebSocket close error: ", strerror(errno));
        return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
    }

    return WEBSOCKET_ERRORCODE_NONE;
}
