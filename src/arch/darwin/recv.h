#ifndef NOSTR_DARWIN_RECV_H_
#define NOSTR_DARWIN_RECV_H_

#include <sys/socket.h>

#include "../../util/types.h"

static inline ssize_t darwin_recvfrom(
    const int32_t    sock_fd,
    void*            buf,
    const size_t     len,
    const int32_t    flags,
    struct sockaddr* src_addr,
    socklen_t*       addr_len)
{
    return recvfrom(sock_fd, buf, len, flags, src_addr, addr_len);
}

#endif
