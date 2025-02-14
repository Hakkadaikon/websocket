#ifndef NOSTR_INTERNAL_SEND_H_
#define NOSTR_INTERNAL_SEND_H_

#include "./linux/epoll.h"
#include "./linux/x86_64/epoll.h"

static inline long internal_epoll_ctl(
    const int epoll_fd,
    const int op,
    const int fd,
    PWebSocketEpollEvent event
)
{
    return linux_x8664_epoll_ctl(epoll_fd, op, fd, event);
}

static inline long internal_epoll_create1(const int flags)
{
    return linux_x8664_epoll_create1(flags);
}

static inline long internal_epoll_wait(
    const int epfd,
    PWebSocketEpollEvent events,
    const int maxevents,
    const int timeout
)
{
    return linux_x8664_epoll_wait(epfd, events, maxevents, timeout);
}

#endif
