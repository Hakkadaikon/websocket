#ifndef NOSTR_INTERNAL_OPTIMIZE_SOCKET_H_
#define NOSTR_INTERNAL_OPTIMIZE_SOCKET_H_

#ifdef __APPLE__
#include "darwin/optimize_socket.h"
#else
#include "linux/x86_64/optimize_socket.h"
#endif

static inline int32_t internal_fcntl(const int32_t fd, const int32_t cmd, const int64_t arg)
{
#ifdef __APPLE__
    return darwin_fcntl(fd, cmd, arg);
#else
    return linux_x8664_fcntl(fd, cmd, arg);
#endif
}

static inline int32_t internal_setsockopt(
    const int32_t   sock_fd,
    const int32_t   level,
    const int32_t   optname,
    const void*     optval,
    const socklen_t optlen)
{
#ifdef __APPLE__
    return darwin_setsockopt(sock_fd, level, optname, optval, optlen);
#else
    return linux_x8664_setsockopt(sock_fd, level, optname, optval, optlen);
#endif
}

#endif
