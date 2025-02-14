#ifndef NOSTR_DARWIN_EPOLL_H_
#define NOSTR_DARWIN_EPOLL_H_

#ifdef __APPLE__
#include <sys/event.h>
typedef struct kevent WebSocketEpollEvent, *PWebSocketEpollEvent;
#endif

#endif
