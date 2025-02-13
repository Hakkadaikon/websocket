#ifndef __APPLE__

#include <errno.h>
#include <netinet/in.h>

#include "../websocket_local.h"
#include "optimize_socket.h"

#define EPOLL_CTL_ADD 1  // Add a file descriptor to the interface.
#define EPOLL_CTL_DEL 2  // Remove a file descriptor from the interface.
#define EPOLL_CTL_MOD 3  // Change file descriptor epoll_event structure.

enum EPOLL_EVENTS {
    EPOLLIN        = 0x001,
    EPOLLPRI       = 0x002,
    EPOLLOUT       = 0x004,
    EPOLLRDNORM    = 0x040,
    EPOLLRDBAND    = 0x080,
    EPOLLWRNORM    = 0x100,
    EPOLLWRBAND    = 0x200,
    EPOLLMSG       = 0x400,
    EPOLLERR       = 0x008,
    EPOLLHUP       = 0x010,
    EPOLLRDHUP     = 0x2000,
    EPOLLEXCLUSIVE = 1u << 28,
    EPOLLWAKEUP    = 1u << 29,
    EPOLLONESHOT   = 1u << 30,
    EPOLLET        = 1u << 31
};

static inline long linux_epoll_ctl(int epoll_fd, int op, int fd, PWebSocketEpollEvent event)
{
    long           ret;
    register void* r10_asm asm("r10") = event;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "0"(SYS_epoll_ctl),
          "D"(epoll_fd),
          "S"(op),
          "d"(fd),
          "r"(r10_asm)
        : "rcx", "r11", "memory");
    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

static inline long linux_epoll_create1(int flags)
{
    long ret;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "0"(SYS_epoll_create1),
          "D"(flags)
        : "rcx", "r11", "memory");
    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

static inline long linux_epoll_wait(int epfd, PWebSocketEpollEvent events, int maxevents, int timeout)
{
    long         ret;
    register int r10 asm("r10") = timeout;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "0"(SYS_epoll_wait),
          "D"(epfd),
          "S"(events),
          "d"(maxevents),
          "r"(r10)
        : "rcx", "r11", "memory");
    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

bool websocket_epoll_add(const int epoll_fd, const int sock_fd, PWebSocketEpollEvent event)
{
    event->data.fd = sock_fd;
    event->events  = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP | EPOLLET;

    while (1) {
        if (is_rise_signal()) {
            log_info("A signal was raised during epoll_wait(). The system will abort processing.\n");
            return false;
        }

        if (linux_epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, event) == 0) {
            break;
        }

        if (errno != EINTR) {
            str_error("Failed to epoll_ctl(CTL_ADD). reason : ", strerror(errno));
            return false;
        }
    }

    return true;
}

bool websocket_epoll_del(const int epoll_fd, const int sock_fd)
{
    if (linux_epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sock_fd, NULL) == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to epoll_ctl(CTL_DEL). reason : ", strerror(errno));
        return false;
    }

    return true;
}

int websocket_epoll_create()
{
    int epoll_fd = linux_epoll_create1(0);
    if (epoll_fd == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to epoll_create1(). reason : ", strerror(errno));
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    if (set_nonblocking(epoll_fd) == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to fnctl - epoll_create1(). reason : ", strerror(errno));
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    return epoll_fd;
}

int websocket_epoll_wait(const int epoll_fd, PWebSocketEpollEvent events, const int max_events)
{
    if (is_rise_signal()) {
        log_info("A signal was raised during epoll_wait(). The system will abort processing.\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    int num_of_event = linux_epoll_wait(epoll_fd, events, max_events, 0);
    if (num_of_event < 0) {
        if (errno == EINTR || errno == EAGAIN) {
            return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
        }

        str_error("Failed to epoll_wait(). reason : ", strerror(errno));
        log_error("The system will abort processing.\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    return num_of_event;
}

int websocket_epoll_getfd(PWebSocketEpollEvent event)
{
    return event->data.fd;
}

int websocket_epoll_rise_error(PWebSocketEpollEvent event)
{
    if (event->events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) {
        return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
    }

    return WEBSOCKET_ERRORCODE_NONE;
}

int websocket_epoll_rise_input(PWebSocketEpollEvent event)
{
    if (!(event->events & EPOLLIN)) {
        return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
    }

    return WEBSOCKET_ERRORCODE_NONE;
}

#endif
