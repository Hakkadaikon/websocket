#ifndef NOSTR_LINUX_X86_64_LISTEN_H_
#define NOSTR_LINUX_X86_64_LISTEN_H_

#include "./optimize_socket.h"

static inline int32_t linux_x8664_socket(const int32_t domain, const int32_t type, const int32_t protocol)
{
    int32_t ret = linux_x8664_asm_syscall3(
        __NR_socket,
        domain,
        type,
        protocol);

    if ((uint64_t)ret >= (uint64_t)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

static inline int32_t linux_x8664_bind(const int32_t sockfd, const struct sockaddr* addr, const socklen_t addrlen)
{
    int32_t ret = linux_x8664_asm_syscall3(
        __NR_bind,
        sockfd,
        addr,
        addrlen);

    if ((uint64_t)ret >= (uint64_t)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

static inline int32_t linux_x8664_listen(const int32_t sockfd, const int32_t backlog)
{
    int32_t ret = linux_x8664_asm_syscall2(
        __NR_listen,
        sockfd,
        backlog);

    if ((uint64_t)ret >= (uint64_t)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

#endif
