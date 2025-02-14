#ifndef NOSTR_INTERNAL_OPTIMIZE_SOCKET_H_
#define NOSTR_INTERNAL_OPTIMIZE_SOCKET_H_

#ifdef __APPLE__
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#else
#include "linux/x86_64/optimize_socket.h"
#endif

static inline int internal_fcntl(const int fd, const int cmd, const long arg)
{
#ifdef __APPLE__
    return fcntl(fd, cmd, arg);
#else
    return linux_x8664_fcntl(fd, cmd, arg);
#endif
}

static inline int internal_setsockopt(
    const int sock_fd,
    const int level,
    const int optname,
    const void* optval,
    const socklen_t optlen)
{
#ifdef __APPLE__
    return setsockopt(sock_fd, level, optname, optval, optlen);
#else
    return linux_x8664_setsockopt(sock_fd, level, optname, optval, optlen);
#endif
}

#endif
