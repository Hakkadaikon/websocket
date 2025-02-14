#ifndef NOSTR_INTERNAL_LISTEN_H_
#define NOSTR_INTERNAL_LISTEN_H_

#ifdef __APPLE__
#include <sys/types.h>
#include <sys/socket.h>
#else
#include "linux/x86_64/listen.h"
#endif

static inline int internal_socket(const int domain, const int type, const int protocol)
{
#ifdef __APPLE__
    return socket(domain, type, protocol);
#else
    return linux_x8664_socket(domain, type, protocol);
#endif
}

static inline int internal_bind(const int sockfd, const struct sockaddr* addr, const socklen_t addrlen)
{
#ifdef __APPLE__
    return bind(sockfd, addr, addrlen);
#else
    return linux_x8664_bind(sockfd, addr, addrlen);
#endif
}

static inline int internal_listen(const int sockfd, const int backlog)
{
#ifdef __APPLE__
    return listen(sockfd, backlog);
#else
    return linux_x8664_listen(sockfd, backlog);
#endif
}

#endif
