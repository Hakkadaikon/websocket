#ifndef NOSTR_SOCKADDR_H_
#define NOSTR_SOCKADDR_H_

#include <stdint.h>

#define AF_INET 2
#define SOCK_STREAM 1
#define INADDR_ANY ((in_addr_t)0x00000000)

typedef uint32_t in_addr_t;

struct in_addr {
    in_addr_t s_addr;
};

struct sockaddr_in {
    unsigned short sin_family;
    unsigned short sin_port;
    struct in_addr sin_addr;
    char           sin_zero[8];
};

struct sockaddr {
    unsigned short sa_family;
    char           sa_data[14];
};

typedef unsigned int socklen_t;
#endif
