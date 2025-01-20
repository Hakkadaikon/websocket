#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/syscall.h>

#include "../../util/log.h"
#include "../../util/signal.h"
#include "../websocket.h"

typedef struct {
    struct msghdr msg_hdr;
    unsigned int  msg_len;
} WebSocketMmsgHeader, *PWebSocketMmsgHeader;

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
            log_error("Failed to add client socket to epoll\n");
            return false;
        }
    }

    return true;
}

bool websocket_epoll_del(const int epoll_fd, const int sock_fd)
{
    if (syscall(SYS_epoll_ctl, epoll_fd, EPOLL_CTL_DEL, sock_fd, NULL) == WEBSOCKET_SYSCALL_ERROR) {
        log_error("Failed to del client socket to epoll\n");
        return false;
    }

    return true;
}

int websocket_epoll_create()
{
    int epoll_fd = syscall(SYS_epoll_create1, 0);
    if (epoll_fd == WEBSOCKET_SYSCALL_ERROR) {
        log_error("Failed to create epoll instance\n");
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

    //int nfds = syscall(SYS_epoll_wait, epoll_fd, events, max_events, -1);  // blocking
    int nfds = syscall(SYS_epoll_wait, epoll_fd, events, max_events, 0);  // non blocking
    if (nfds < 0) {
        if (errno != EINTR) {
            char* errmsg = strerror(errno);
            log_debug("Failed to create epoll instance. err : ");
            log_debug(errmsg);
            log_debug("\n");
            log_debug("The system will abort processing.\n");
            return WEBSOCKET_ERRORCODE_FATAL_ERROR;
        }

        return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
    }

    return nfds;
}
