#ifndef NOSTR_LINUX_X86_64_LISTEN_H_
#define NOSTR_LINUX_X86_64_LISTEN_H_

#include "./optimize_socket.h"

static inline int linux_x8664_socket(const int domain, const int type, const int protocol)
{
    long ret;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "0"(__NR_socket),
          "D"(domain),
          "S"(type),
          "d"(protocol)
        : "rcx", "r11", "memory");
    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

static inline int linux_x8664_bind(const int sockfd, const struct sockaddr* addr, const socklen_t addrlen)
{
    long ret;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "0"(__NR_bind),
          "D"(sockfd),
          "S"(addr),
          "d"(addrlen)
        : "rcx", "r11", "memory");
    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

static inline int linux_x8664_listen(const int sockfd, const int backlog)
{
    long ret;
    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "0"(__NR_listen), "D"(sockfd), "S"(backlog)
        : "rcx", "r11", "memory");
    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

#endif
