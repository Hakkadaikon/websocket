#ifndef NOSTR_INTERNAL_LISTEN_H_
#define NOSTR_INTERNAL_LISTEN_H_

#include "../util/types.h"
#ifdef __APPLE__
#include "darwin/listen.h"
#else
#include "linux/x86_64/listen.h"
#endif

static inline int32_t internal_socket(const int32_t domain, const int32_t type, const int32_t protocol)
{
#ifdef __APPLE__
    return darwin_socket(domain, type, protocol);
#else
    return linux_x8664_socket(domain, type, protocol);
#endif
}

static inline int32_t internal_bind(const int32_t sockfd, const struct sockaddr* addr, const socklen_t addrlen)
{
#ifdef __APPLE__
    return darwin_bind(sockfd, addr, addrlen);
#else
    return linux_x8664_bind(sockfd, addr, addrlen);
#endif
}

static inline int32_t internal_listen(const int32_t sockfd, const int32_t backlog)
{
#ifdef __APPLE__
    return darwin_listen(sockfd, backlog);
#else
    return linux_x8664_listen(sockfd, backlog);
#endif
}

#endif
