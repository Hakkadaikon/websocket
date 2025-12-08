#ifndef NOSTR_INTERNAL_SEND_H_
#define NOSTR_INTERNAL_SEND_H_

#include "../util/types.h"
#include "./linux/epoll.h"
#include "./linux/x86_64/epoll.h"

static inline int32_t internal_epoll_ctl(
  const int32_t        epoll_fd,
  const int32_t        op,
  const int32_t        fd,
  PWebSocketEpollEvent event)
{
  return linux_x8664_epoll_ctl(epoll_fd, op, fd, event);
}

static inline int32_t internal_epoll_create1(const int32_t flags)
{
  return linux_x8664_epoll_create1(flags);
}

static inline int32_t internal_epoll_wait(
  const int32_t        epfd,
  PWebSocketEpollEvent events,
  const int32_t        maxevents,
  const int32_t        timeout)
{
  return linux_x8664_epoll_wait(epfd, events, maxevents, timeout);
}

#endif
