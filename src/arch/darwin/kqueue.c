#ifdef __APPLE__

#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/event.h>
#include <sys/syscall.h>
#include <time.h>

#include "../../util/signal.h"
#include "../../util/types.h"
#include "../../websocket/websocket_local.h"
#include "./epoll.h"

bool websocket_epoll_add(const int32_t epoll_fd, const int32_t sock_fd, PWebSocketEpollEvent event)
{
    EV_SET(event, sock_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);

    while (1) {
        if (is_rise_signal()) {
            log_info("A signal was raised during kevent(). The system will abort processing.\n");
            return false;
        }

        if (kevent(epoll_fd, event, 1, NULL, 0, NULL) != WEBSOCKET_SYSCALL_ERROR) {
            break;
        }

        if (errno != EINTR) {
            str_error("Failed to kevent(EV_ADD | EV_CLEAR). reason : ", strerror(errno));
            return false;
        }
    }

    return true;
}

bool websocket_epoll_del(const int32_t epoll_fd, const int32_t sock_fd)
{
    struct kevent event;
    EV_SET(&event, sock_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);

    if (kevent(epoll_fd, &event, 1, NULL, 0, NULL) < 0) {
        str_error("Failed to kevent(EV_DELETE). reason : ", strerror(errno));
        return false;
    }

    return true;
}

int32_t websocket_epoll_create()
{
    int32_t epoll_fd = kqueue();
    if (epoll_fd == -1) {
        str_error("Failed to kqueue(). reason : ", strerror(errno));
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    return epoll_fd;
}

int32_t websocket_epoll_wait(const int32_t epoll_fd, PWebSocketEpollEvent events, const int32_t max_events)
{
    if (is_rise_signal()) {
        log_info("A signal was raised during kevent(). The system will abort processing.\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    struct timespec timeout      = {0, 0};  // Non-blocking
    int32_t         num_of_event = kevent(epoll_fd, NULL, 0, events, max_events, &timeout);

    if (num_of_event < 0) {
        if (errno == EINTR || errno == EAGAIN) {
            return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
        }

        str_error("Failed to kevent(). reason : ", strerror(errno));
        log_error("The system will abort processing.\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    return num_of_event;
}

int32_t websocket_epoll_getfd(PWebSocketEpollEvent event)
{
    return event->ident;
}

int32_t websocket_epoll_rise_error(PWebSocketEpollEvent event)
{
    if (event->flags & EV_ERROR) {
        str_error("EV_ERROR : ", strerror((int)event->data));
        return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
    }

    return WEBSOCKET_ERRORCODE_NONE;
}

int32_t websocket_epoll_rise_input(PWebSocketEpollEvent event)
{
    if (!(event->filter | EVFILT_READ)) {
        return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
    }

    return WEBSOCKET_ERRORCODE_NONE;
}

#endif
