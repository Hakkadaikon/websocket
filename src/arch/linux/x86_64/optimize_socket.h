#ifndef NOSTR_LINUX_X86_64_OPTIMIZE_SOCKET_H_
#define NOSTR_LINUX_X86_64_OPTIMIZE_SOCKET_H_

#include "../errno.h"
#include "../fcntl.h"
#include "../sockaddr.h"
#include "../sockoption.h"
#include "./asm.h"

static inline int linux_x8664_fcntl(const int fd, const int cmd, const long arg)
{
    long ret;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "0"(__NR_fcntl), "D"(fd), "S"(cmd), "d"(arg)
        : "rcx", "r11", "memory");
    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

static inline int linux_x8664_setsockopt(
    const int       sockfd,
    const int       level,
    const int       optname,
    const void*     optval,
    const socklen_t optlen)
{
    long ret;

    register const void* r10_val asm("r10") = optval;
    register long        r8_val asm("r8")   = optlen;

    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "0"(__NR_setsockopt),
          "D"((long)sockfd),
          "S"((long)level),
          "d"((long)optname),
          "r"(r10_val),
          "r"(r8_val)
        : "rcx", "r11", "memory");
    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

#endif
