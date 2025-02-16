#ifndef NOSTR_DARWIN_RECV_H_
#define NOSTR_DARWIN_RECV_H_

#include <sys/socket.h>
#include <sys/types.h>

static inline ssize_t darwin_recvfrom(
    const int        sock_fd,
    void*            buf,
    const size_t     len,
    const int        flags,
    struct sockaddr* src_addr,
    socklen_t*       addr_len)
{
    return recvfrom(sock_fd, buf, len, flags, src_addr, addr_len);
}

#endif
