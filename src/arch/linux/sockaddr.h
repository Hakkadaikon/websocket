#ifndef NOSTR_LINUX_SOCKADDR_H_
#define NOSTR_LINUX_SOCKADDR_H_

#include "../../util/types.h"

#define AF_INET 2
#define SOCK_STREAM 1
#define INADDR_ANY ((in_addr_t)0x00000000)

typedef uint32_t in_addr_t;

struct in_addr {
    in_addr_t s_addr;
};

struct sockaddr_in {
    uint16_t       sin_family;
    uint16_t       sin_port;
    struct in_addr sin_addr;
    char           sin_zero[8];
};

struct sockaddr {
    uint16_t sa_family;
    char     sa_data[14];
};

typedef uint32_t socklen_t;
#endif
