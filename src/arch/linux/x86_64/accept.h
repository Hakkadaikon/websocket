#ifndef NOSTR_LINUX_X86_64_ACCEPT_H_
#define NOSTR_LINUX_X86_64_ACCEPT_H_

#include "../errno.h"
#include "../sockaddr.h"
#include "./asm.h"

static inline int32_t linux_x8664_accept4(const int32_t sock_fd, const struct sockaddr* addr, const socklen_t* addrlen, const int32_t flags)
{
    int32_t ret = linux_x8664_asm_syscall4(
        __NR_accept4,
        sock_fd,
        addr,
        addrlen,
        flags);

    if ((uint64_t)ret >= (uint64_t)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

#endif
