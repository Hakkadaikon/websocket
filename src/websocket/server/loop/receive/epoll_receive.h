#ifndef NOSTR_SERVER_LOOP_EPOLL_RECEIVE_H_
#define NOSTR_SERVER_LOOP_EPOLL_RECEIVE_H_

#include "../../../../util/allocator.h"
#include "../../../websocket_local.h"
#include "receive_handle.h"

static inline int32_t epoll_receive(
  const PWebSocketEpollLoopArgs epoll_args,
  PWebSocketRawBuffer           buffer,
  PWebSocketCallbacks           callbacks)
{
  int32_t code = websocket_epoll_rise_error(epoll_args->event);
  if (code != WEBSOCKET_ERRORCODE_NONE) {
    return code;
  }

  code = websocket_epoll_rise_input(epoll_args->event);
  if (code != WEBSOCKET_ERRORCODE_NONE) {
    return code;
  }

  int32_t client_sock = websocket_epoll_getfd(epoll_args->event);

  ssize_t read_size = websocket_recv(client_sock, buffer->capacity, buffer->request);

  if (read_size <= 0) {
    if (read_size == WEBSOCKET_ERRORCODE_FATAL_ERROR || read_size == WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR) {
      return read_size;
    }

    return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
  }

  int32_t ret = receive_handle(client_sock, read_size, buffer, callbacks);

  if (ret == WEBSOCKET_ERRORCODE_FATAL_ERROR || ret == WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR) {
    return ret;
  }

  return WEBSOCKET_ERRORCODE_NONE;
}

#endif
