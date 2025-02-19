#ifndef NOSTR_INTERNAL_LISTEN_H_
#define NOSTR_INTERNAL_LISTEN_H_

#ifdef __APPLE__
#include "darwin/listen.h"
#else
#include "linux/x86_64/listen.h"
#endif

static inline int internal_socket(const int domain, const int type, const int protocol)
{
#ifdef __APPLE__
    return darwin_socket(domain, type, protocol);
#else
    return linux_x8664_socket(domain, type, protocol);
#endif
}

static inline int internal_bind(const int sockfd, const struct sockaddr* addr, const socklen_t addrlen)
{
#ifdef __APPLE__
    return darwin_bind(sockfd, addr, addrlen);
#else
    return linux_x8664_bind(sockfd, addr, addrlen);
#endif
}

static inline int internal_listen(const int sockfd, const int backlog)
{
#ifdef __APPLE__
    return darwin_listen(sockfd, backlog);
#else
    return linux_x8664_listen(sockfd, backlog);
#endif
}

#endif
