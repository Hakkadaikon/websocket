#ifndef NOSTR_LINUX_X86_64_SEND_H_
#define NOSTR_LINUX_X86_64_SEND_H_

#include "../../../util/types.h"
#include "../sockaddr.h"
#include "./asm.h"
#include "../errno.h"

static inline ssize_t linux_x8664_sendto(
    const int        sock_fd,
    const void*      buf,
    const size_t     len,
    const int        flags,
    struct sockaddr* dest_addr,
    const socklen_t  addrlen)
{
    long               ret;
    register int       r10_asm asm("r10") = flags;
    register void*     r8_asm asm("r8")   = dest_addr;
    register socklen_t r9_asm asm("r9")   = addrlen;

    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "0"(__NR_sendto),
          "D"(sock_fd),
          "S"(buf),
          "d"(len),
          "r"(r10_asm),
          "r"(r8_asm),
          "r"(r9_asm)
        : "rcx", "r11", "memory");
    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

#endif
