#ifndef NOSTR_LINUX_X86_64_EPOLL_H_
#define NOSTR_LINUX_X86_64_EPOLL_H_

#include "../epoll.h"
#include "../errno.h"
#include "./asm.h"

static inline long linux_x8664_epoll_ctl(
    const int            epoll_fd,
    const int            op,
    const int            fd,
    PWebSocketEpollEvent event)
{
    long ret = linux_x8664_asm_syscall4(
        __NR_epoll_ctl,
        epoll_fd,
        op,
        fd,
        event);

    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

static inline long linux_x8664_epoll_create1(const int flags)
{
    long ret = linux_x8664_asm_syscall1(
        __NR_epoll_create1,
        flags);

    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

static inline long linux_x8664_epoll_wait(
    const int            epfd,
    PWebSocketEpollEvent events,
    const int            maxevents,
    const int            timeout)
{
    long ret = linux_x8664_asm_syscall4(
        __NR_epoll_wait,
        epfd,
        events,
        maxevents,
        timeout);

    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

#endif
