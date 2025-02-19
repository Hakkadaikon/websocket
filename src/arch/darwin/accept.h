#ifndef NOSTR_DARWIN_ACCEPT_H_
#define NOSTR_DARWIN_ACCEPT_H_

#include <netinet/in.h>

#include "../../util/types.h"

static inline int32_t darwin_accept(const int32_t sock_fd, struct sockaddr* addr, socklen_t* addrlen, const int32_t flags)
{
    return accept(sock_fd, addr, addrlen);
}

#endif
