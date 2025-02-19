#ifndef NOSTR_DARWIN_LISTEN_H_
#define NOSTR_DARWIN_LISTEN_H_

#include <sys/socket.h>
#include <sys/types.h>

static inline int darwin_socket(const int domain, const int type, const int protocol)
{
    return socket(domain, type, protocol);
}

static inline int darwin_bind(const int sockfd, const struct sockaddr* addr, const socklen_t addrlen)
{
    return bind(sockfd, addr, addrlen);
}

static inline int darwin_listen(const int sockfd, const int backlog)
{
    return listen(sockfd, backlog);
}

#endif
