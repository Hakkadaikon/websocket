#ifndef NOSTR_DARWIN_OPTIMIZE_SOCKET_H_
#define NOSTR_DARWIN_OPTIMIZE_SOCKET_H_

#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

static inline int darwin_fcntl(const int fd, const int cmd, const long arg)
{
    return fcntl(fd, cmd, arg);
}

static inline int darwin_setsockopt(
    const int       sock_fd,
    const int       level,
    const int       optname,
    const void*     optval,
    const socklen_t optlen)
{
    return setsockopt(sock_fd, level, optname, optval, optlen);
}

#endif
