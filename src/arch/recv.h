#ifndef NOSTR_INTERNAL_RECV_H_
#define NOSTR_INTERNAL_RECV_H_

#ifdef __APPLE__
#include "darwin/recv.h"
#else
#include "linux/x86_64/recv.h"
#endif

static inline ssize_t internal_recvfrom(
    const int        sock_fd,
    void*            buf,
    const size_t     len,
    const int        flags,
    struct sockaddr* src_addr,
    socklen_t*       addr_len)
{
#ifdef __APPLE__
    return darwin_recvfrom(sock_fd, buf, len, flags, src_addr, addr_len);
#else
    return linux_x8664_recvfrom(sock_fd, buf, len, flags, src_addr, addr_len);
#endif
}

#endif
