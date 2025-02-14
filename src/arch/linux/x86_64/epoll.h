#include "./asm.h"
#include "../epoll.h"
#include "../errno.h"

static inline long linux_x8664_epoll_ctl(
    const int epoll_fd,
    const int op,
    const int fd,
    PWebSocketEpollEvent event
)
{
    long           ret;
    register void* r10_asm asm("r10") = event;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "0"(__NR_epoll_ctl),
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

static inline long linux_x8664_epoll_create1(const int flags)
{
    long ret;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "0"(__NR_epoll_create1),
          "D"(flags)
        : "rcx", "r11", "memory");
    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

static inline long linux_x8664_epoll_wait(
    const int epfd,
    PWebSocketEpollEvent events,
    const int maxevents,
    const int timeout
)
{
    long         ret;
    register int r10 asm("r10") = timeout;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "0"(__NR_epoll_wait),
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
