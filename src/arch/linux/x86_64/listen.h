#ifndef NOSTR_LINUX_X86_64_LISTEN_H_
#define NOSTR_LINUX_X86_64_LISTEN_H_

#include "./optimize_socket.h"

static inline int linux_x8664_socket(const int domain, const int type, const int protocol)
{
    long ret = linux_x8664_asm_syscall3(
        __NR_socket,
        domain,
        type,
        protocol);

    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

static inline int linux_x8664_bind(const int sockfd, const struct sockaddr* addr, const socklen_t addrlen)
{
    long ret = linux_x8664_asm_syscall3(
        __NR_bind,
        sockfd,
        addr,
        addrlen);

    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

static inline int linux_x8664_listen(const int sockfd, const int backlog)
{
    long ret = linux_x8664_asm_syscall2(
        __NR_listen,
        sockfd,
        backlog);

    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

#endif
