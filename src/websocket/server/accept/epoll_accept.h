#ifndef NOSTR_SERVER_LOOP_EPOLL_ACCEPT_H_
#define NOSTR_SERVER_LOOP_EPOLL_ACCEPT_H_

#include "../../websocket_local.h"
#include "accept_handle.h"

static inline int32_t epoll_accept(
  const PWebSocketEpollLoopArgs epoll_args,
  const int32_t                 server_sock,
  PWebSocketRawBuffer           buffer,
  const PWebSocketEpollEvent    register_event,
  PWebSocketCallbacks           callbacks)
{
  log_debug("rise error check...\n");
  int32_t code = websocket_epoll_rise_error(epoll_args->event);
  if (code != WEBSOCKET_ERRORCODE_NONE) {
    return code;
  }

  log_debug("rise input check...\n");
  code = websocket_epoll_rise_input(epoll_args->event);
  if (code != WEBSOCKET_ERRORCODE_NONE) {
    return code;
  }

  log_debug("accept handle\n");
  if (!accept_handle(
        epoll_args->epoll_fd,
        server_sock,
        buffer,
        register_event,
        callbacks)) {
    return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
  }

  return WEBSOCKET_ERRORCODE_NONE;
}

#endif
