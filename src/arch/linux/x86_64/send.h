#ifndef NOSTR_LINUX_X86_64_SEND_H_
#define NOSTR_LINUX_X86_64_SEND_H_

#include "../../../util/types.h"
#include "../errno.h"
#include "../sockaddr.h"
#include "./asm.h"

static inline ssize_t linux_x8664_sendto(
    const int        sock_fd,
    const void*      buf,
    const size_t     len,
    const int        flags,
    struct sockaddr* dest_addr,
    const socklen_t  addrlen)
{
    long ret = linux_x8664_asm_syscall6(
        __NR_sendto,
        sock_fd,
        buf,
        len,
        flags,
        dest_addr,
        addrlen);

    if ((unsigned long)ret >= (unsigned long)-4095) {
        errno = -ret;
        ret   = -1;
    }
    return ret;
}

#endif
