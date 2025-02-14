#ifndef NOSTR_INTERNAL_SEND_H_
#define NOSTR_INTERNAL_SEND_H_

#ifdef __APPLE__
#include <sys/socket.h>
#include <sys/types.h>
#else
#include "linux/x86_64/send.h"
#endif

static inline ssize_t internal_sendto(
    const int        sock_fd,
    const void*      buf,
    const size_t     len,
    const int        flags,
    struct sockaddr* dest_addr,
    const socklen_t  addr_len)
{
#ifdef __APPLE__
    return sendto(sock_fd, buf, len, flags, dest_addr, addr_len);
#else
    return linux_x8664_sendto(sock_fd, buf, len, flags, dest_addr, addr_len);
#endif
}

#endif
