#ifndef NOSTR_DARWIN_LISTEN_H_
#define NOSTR_DARWIN_LISTEN_H_

#include <sys/socket.h>

#include "../../util/types.h"

static inline int32_t darwin_socket(const int32_t domain, const int32_t type, const int32_t protocol)
{
    return socket(domain, type, protocol);
}

static inline int32_t darwin_bind(const int32_t sockfd, const struct sockaddr* addr, const socklen_t addrlen)
{
    return bind(sockfd, addr, addrlen);
}

static inline int32_t darwin_listen(const int32_t sockfd, const int32_t backlog)
{
    return listen(sockfd, backlog);
}

#endif
