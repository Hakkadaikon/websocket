#ifdef __APPLE__

#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/event.h>
#include <sys/syscall.h>

#include "../../util/log.h"
#include "../../util/signal.h"
#include "../websocket.h"

bool websocket_epoll_add(const int epoll_fd, const int sock_fd, PWebSocketEpollEvent event)
{
    EV_SET(event, sock_fd, EVFILT_READ, EV_ADD | EV_CLEAR, 0, 0, NULL);
    // EV_SET(event, sock_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);

    while (1) {
        if (is_rise_signal()) {
            log_info("A signal was raised during kevent(). The system will abort processing.\n");
            return false;
        }

        if (kevent(epoll_fd, event, 1, NULL, 0, NULL) != WEBSOCKET_SYSCALL_ERROR) {
            break;
        }

        if (errno != EINTR) {
            str_error("Failed to kevent(ADD). reason : ", strerror(errno));
            return false;
        }
    }

    return true;
}

bool websocket_epoll_del(const int epoll_fd, const int sock_fd)
{
    struct kevent event;
    EV_SET(&event, sock_fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);

    if (kevent(epoll_fd, &event, 1, NULL, 0, NULL) < 0) {
        str_error("Failed to kevent(DEL). reason : ", strerror(errno));
        return false;
    }

    return true;
}

int websocket_epoll_create()
{
    int epoll_fd = kqueue();
    if (epoll_fd == -1) {
        str_error("Failed to kqueue(). reason : ", strerror(errno));
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    return epoll_fd;
}

int websocket_epoll_wait(const int epoll_fd, PWebSocketEpollEvent events, const int max_events)
{
    if (is_rise_signal()) {
        log_info("A signal was raised during kevent(). The system will abort processing.\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    struct timespec timeout      = {0, 0};  // Non-blocking
    int             num_of_event = kevent(epoll_fd, NULL, 0, events, max_events, &timeout);

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

#endif
