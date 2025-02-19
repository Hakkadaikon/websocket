#ifndef NOSTR_DARWIN_ACCEPT_H_
#define NOSTR_DARWIN_ACCEPT_H_

#include <netinet/in.h>

static inline int darwin_accept(const int sock_fd, struct sockaddr* addr, socklen_t* addrlen, const int flags)
{
    return accept(sock_fd, addr, addrlen);
}

#endif
