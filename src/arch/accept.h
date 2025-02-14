#ifndef NOSTR_INTERNAL_ACCEPT_H_
#define NOSTR_INTERNAL_ACCEPT_H_

#ifdef __APPLE__
#include <netinet/in.h>
#else
#include "linux/x86_64/accept.h"
#endif

static inline int internal_accept(const int sock_fd, struct sockaddr* addr, socklen_t* addrlen, const int flags)
{
#ifdef __APPLE__
    return accept(sock_fd, addr, addrlen);
#else
    return linux_x8664_accept4(sock_fd, addr, addrlen, flags);
#endif
}

#endif
