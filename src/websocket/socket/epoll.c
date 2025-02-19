#ifndef __APPLE__

#include "../../arch/epoll.h"

#include "../websocket_local.h"
#include "./optimize_socket.h"

bool websocket_epoll_add(const int32_t epoll_fd, const int32_t sock_fd, PWebSocketEpollEvent event)
{
    event->data.fd = sock_fd;
    event->events  = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP | EPOLLET;

    while (1) {
        if (is_rise_signal()) {
            log_info("A signal was raised during epoll_wait(). The system will abort processing.\n");
            return false;
        }

        if (internal_epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, event) == 0) {
            break;
        }

        if (errno != EINTR) {
            str_error("Failed to epoll_ctl(CTL_ADD). reason : ", strerror(errno));
            return false;
        }
    }

    return true;
}

bool websocket_epoll_del(const int32_t epoll_fd, const int32_t sock_fd)
{
    if (internal_epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sock_fd, NULL) == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to epoll_ctl(CTL_DEL). reason : ", strerror(errno));
        return false;
    }

    return true;
}

int32_t websocket_epoll_create()
{
    int32_t epoll_fd = internal_epoll_create1(0);
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

int32_t websocket_epoll_wait(const int32_t epoll_fd, PWebSocketEpollEvent events, const int32_t max_events)
{
    if (is_rise_signal()) {
        log_info("A signal was raised during epoll_wait(). The system will abort processing.\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    int32_t num_of_event = internal_epoll_wait(epoll_fd, events, max_events, 0);
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

int32_t websocket_epoll_getfd(PWebSocketEpollEvent event)
{
    return event->data.fd;
}

int32_t websocket_epoll_rise_error(PWebSocketEpollEvent event)
{
    if (event->events & (EPOLLHUP | EPOLLERR | EPOLLRDHUP)) {
        return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
    }

    return WEBSOCKET_ERRORCODE_NONE;
}

int32_t websocket_epoll_rise_input(PWebSocketEpollEvent event)
{
    if (!(event->events & EPOLLIN)) {
        return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
    }

    return WEBSOCKET_ERRORCODE_NONE;
}

#endif
