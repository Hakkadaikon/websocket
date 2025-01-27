#ifndef __APPLE__

#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/syscall.h>

#include "../websocket_local.h"
#include "optimize_socket.h"

bool websocket_epoll_add(const int epoll_fd, const int sock_fd, PWebSocketEpollEvent event)
{
    event->data.fd = sock_fd;
    event->events  = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP | EPOLLET;

    while (1) {
        if (is_rise_signal()) {
            log_info("A signal was raised during epoll_wait(). The system will abort processing.\n");
            return false;
        }

        if (syscall(SYS_epoll_ctl, epoll_fd, EPOLL_CTL_ADD, sock_fd, event) == 0) {
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
    if (syscall(SYS_epoll_ctl, epoll_fd, EPOLL_CTL_DEL, sock_fd, NULL) == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to epoll_ctl(CTL_DEL). reason : ", strerror(errno));
        return false;
    }

    return true;
}

int websocket_epoll_create()
{
    int epoll_fd = syscall(SYS_epoll_create1, 0);
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

    // int num_of_event = syscall(SYS_epoll_wait, epoll_fd, events, max_events, -1);  // blocking
    int num_of_event = syscall(SYS_epoll_wait, epoll_fd, events, max_events, 0);  // non blocking
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
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
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
