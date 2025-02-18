#ifndef NOSTR_LINUX_X86_64_ACCEPT_H_
#define NOSTR_LINUX_X86_64_ACCEPT_H_

#include "../errno.h"
#include "../sockaddr.h"
#include "./asm.h"

static inline int linux_x8664_accept4(const int sock_fd, const struct sockaddr* addr, const socklen_t* addrlen, const int flags)
{
    long         ret;
    register int r10_asm asm("r10") = flags;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "0"(__NR_accept4),
          "D"(sock_fd),
          "S"(addr),
          "d"(addrlen),
          "r"(r10_asm)
        : "rcx", "r11", "memory");
    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

#endif
