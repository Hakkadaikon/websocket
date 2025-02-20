#ifndef NOSTR_DARWIN_SEND_H_
#define NOSTR_DARWIN_SEND_H_

#include <sys/socket.h>

#include "../../util/types.h"

static inline ssize_t darwin_sendto(
    const int32_t    sock_fd,
    const void*      buf,
    const size_t     len,
    const int32_t    flags,
    struct sockaddr* dest_addr,
    const socklen_t  addr_len)
{
    return sendto(sock_fd, buf, len, flags, dest_addr, addr_len);
}

#endif
