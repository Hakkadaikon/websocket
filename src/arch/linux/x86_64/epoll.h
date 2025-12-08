#ifndef NOSTR_LINUX_X86_64_EPOLL_H_
#define NOSTR_LINUX_X86_64_EPOLL_H_

#include "../epoll.h"
#include "../errno.h"
#include "./asm.h"

static inline int32_t linux_x8664_epoll_ctl(
  const int32_t        epoll_fd,
  const int32_t        op,
  const int32_t        fd,
  PWebSocketEpollEvent event)
{
  int32_t ret = linux_x8664_asm_syscall4(
    __NR_epoll_ctl,
    epoll_fd,
    op,
    fd,
    event);

  SYSCALL_EARLY_RETURN(ret);
  return ret;
}

static inline int32_t linux_x8664_epoll_create1(const int32_t flags)
{
  int32_t ret = linux_x8664_asm_syscall1(
    __NR_epoll_create1,
    flags);

  SYSCALL_EARLY_RETURN(ret);
  return ret;
}

static inline int32_t linux_x8664_epoll_wait(
  const int32_t        epfd,
  PWebSocketEpollEvent events,
  const int32_t        maxevents,
  const int32_t        timeout)
{
  int32_t ret = linux_x8664_asm_syscall4(
    __NR_epoll_wait,
    epfd,
    events,
    maxevents,
    timeout);

  SYSCALL_EARLY_RETURN(ret);
  return ret;
}

#endif
