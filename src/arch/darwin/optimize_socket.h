#ifndef NOSTR_DARWIN_OPTIMIZE_SOCKET_H_
#define NOSTR_DARWIN_OPTIMIZE_SOCKET_H_

#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "../../util/types.h"

static inline int32_t darwin_fcntl(const int32_t fd, const int32_t cmd, const long arg)
{
    return fcntl(fd, cmd, arg);
}

static inline int32_t darwin_setsockopt(
    const int32_t   sock_fd,
    const int32_t   level,
    const int32_t   optname,
    const void*     optval,
    const socklen_t optlen)
{
    return setsockopt(sock_fd, level, optname, optval, optlen);
}

#endif
