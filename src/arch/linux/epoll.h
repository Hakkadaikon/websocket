#ifndef NOSTR_EPOLL_H_
#define NOSTR_EPOLL_H_

#include <stdint.h>

#define EPOLL_CTL_ADD 1  // Add a file descriptor to the interface.
#define EPOLL_CTL_DEL 2  // Remove a file descriptor from the interface.
#define EPOLL_CTL_MOD 3  // Change file descriptor epoll_event structure.

enum EPOLL_EVENTS {
    EPOLLIN        = 0x001,
    EPOLLPRI       = 0x002,
    EPOLLOUT       = 0x004,
    EPOLLRDNORM    = 0x040,
    EPOLLRDBAND    = 0x080,
    EPOLLWRNORM    = 0x100,
    EPOLLWRBAND    = 0x200,
    EPOLLMSG       = 0x400,
    EPOLLERR       = 0x008,
    EPOLLHUP       = 0x010,
    EPOLLRDHUP     = 0x2000,
    EPOLLEXCLUSIVE = 1u << 28,
    EPOLLWAKEUP    = 1u << 29,
    EPOLLONESHOT   = 1u << 30,
    EPOLLET        = 1u << 31
};

typedef union {
    void*    ptr;
    int      fd;
    uint32_t u32;
    uint64_t u64;
} WebSocketEpollData, *PWebSocketEpollData;

typedef struct
{
    uint32_t           events;
    WebSocketEpollData data;
} WebSocketEpollEvent, *PWebSocketEpollEvent;

#endif
