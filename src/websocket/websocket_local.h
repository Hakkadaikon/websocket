#ifndef _NOSTR_WEBSOCKET_LOCAL_H_
#define _NOSTR_WEBSOCKET_LOCAL_H_

/**
 * @file  websocket_local.h
 *
 * @brief Parses each parameter of a websocket frame stored in network byte order.
 * @see RFC6455 (https://datatracker.ietf.org/doc/html/rfc6455)
 */

#include <sys/types.h>
#ifdef __APPLE__
#include "../arch/darwin/epoll.h"
#else
#include "../arch/linux/epoll.h"
#endif

typedef struct {
    int                  epoll_fd;
    PWebSocketEpollEvent event;
} WebSocketEpollLoopArgs, *PWebSocketEpollLoopArgs;

typedef struct {
    size_t capacity;
    char*  request;
    char*  response;
} WebSocketRawBuffer, *PWebSocketRawBuffer;

#include "../http/http.h"
#include "../util/signal.h"
#include "../util/string.h"
#include "websocket.h"

typedef enum {
    WEBSOCKET_SYSCALL_ERROR = -1
} WebSocketSysCallErrorCode;

/*----------------------------------------------------------------------------*/
/* websocket/server/handshake.c                                               */
/*----------------------------------------------------------------------------*/

bool client_handshake(
    const int           client_sock,
    const size_t        bytes_read,
    PWebSocketRawBuffer buffer,
    PHTTPRequest        request);

/*----------------------------------------------------------------------------*/
/* websocket/crypto.c                                                         */
/*----------------------------------------------------------------------------*/

bool generate_websocket_acceptkey(const char* client_key, const size_t accept_key_size, char* accept_key);

/*----------------------------------------------------------------------------*/
/* websocket/socket/accept.c                                                  */
/*----------------------------------------------------------------------------*/

int websocket_accept(const int sock_fd);

/*----------------------------------------------------------------------------*/
/* websocket/socket/listen.c                                                  */
/*----------------------------------------------------------------------------*/

int websocket_listen(const int port_num, const int backlog);

/*----------------------------------------------------------------------------*/
/* websocket/socket/epoll.c or websocket/socket/kqueue.c                      */
/*----------------------------------------------------------------------------*/

bool websocket_epoll_add(const int epoll_fd, const int sock_fd, PWebSocketEpollEvent event);
bool websocket_epoll_del(const int epoll_fd, const int sock_fd);
int  websocket_epoll_create();
int  websocket_epoll_wait(const int epoll_fd, PWebSocketEpollEvent events, const int max_events);
int  websocket_epoll_getfd(PWebSocketEpollEvent event);
int  websocket_epoll_rise_error(PWebSocketEpollEvent event);
int  websocket_epoll_rise_input(PWebSocketEpollEvent event);

/*----------------------------------------------------------------------------*/
/* websocket/internal_log.c                                                   */
/*----------------------------------------------------------------------------*/

void websocket_frame_dump(PWebSocketFrame frame);
void websocket_epoll_event_dump(const int events);

#endif
