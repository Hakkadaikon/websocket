#include "../../util/allocator.h"
#include "../websocket_local.h"
#include "accept/epoll_accept.h"
#include "receive/epoll_receive.h"

bool websocket_server_loop(PWebSocketLoopArgs args)
{
  int32_t epoll_fd = websocket_epoll_create();
  if (epoll_fd < 0) {
    return false;
  }

  const size_t        MAX_EVENTS = 16384;
  WebSocketEpollEvent register_event;
  WebSocketEpollEvent epoll_events[MAX_EVENTS];
  WebSocketRawBuffer  buffer;

  websocket_memset(&register_event, 0x00, sizeof(register_event));
  websocket_memset(epoll_events, 0x00, sizeof(epoll_events));

  if (!websocket_epoll_add(epoll_fd, args->server_sock, &register_event)) {
    websocket_close(epoll_fd);
    return false;
  }

  var_debug("websocket server fd : ", args->server_sock);
  var_debug("websocket epoll  fd : ", epoll_fd);

  buffer.capacity = args->buffer_capacity;
  buffer.request  = websocket_alloc(buffer.capacity);
  buffer.response = websocket_alloc(buffer.capacity);

  if (!buffer.request || !buffer.response) {
    log_error("Failed to allocate buffers\n");
    websocket_close(epoll_fd);
    return false;
  }

  websocket_memset(buffer.request, 0x00, buffer.capacity);
  websocket_memset(buffer.response, 0x00, buffer.capacity);

  while (1) {
    int32_t num_of_events = websocket_epoll_wait(epoll_fd, epoll_events, MAX_EVENTS);
    if (num_of_events <= 0) {
      if (num_of_events != WEBSOCKET_ERRORCODE_FATAL_ERROR) {
        continue;
      }

      log_debug("epoll_wait error. go to finalize...\n");
      goto FINALIZE;
    }

    var_debug("Escaped from epoll wait. num of events: ", num_of_events);

    for (int32_t i = 0; i < num_of_events; ++i) {
      WebSocketEpollLoopArgs epoll_args;
      epoll_args.event    = &epoll_events[i];
      epoll_args.epoll_fd = epoll_fd;

      int32_t fd = websocket_epoll_getfd(epoll_args.event);

      if (fd == args->server_sock) {
        if (epoll_accept(
              &epoll_args,
              args->server_sock,
              &buffer,
              &register_event,
              &args->callbacks) == WEBSOCKET_ERRORCODE_FATAL_ERROR) {
          log_debug("accept error. go to finalize...\n");
          goto FINALIZE;
        }

        continue;
      }

      int32_t client_sock = fd;

      int32_t ret = epoll_receive(&epoll_args, &buffer, &args->callbacks);

      if (ret == WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR) {
        log_debug("socket close...\n");

        websocket_epoll_del(epoll_fd, client_sock);
        websocket_close(client_sock);

        if (!is_null(args->callbacks.disconnect_callback)) {
          args->callbacks.disconnect_callback(client_sock);
        }

        continue;
      }

      if (ret == WEBSOCKET_ERRORCODE_FATAL_ERROR) {
        log_debug("receive error. go to finalize...\n");

        websocket_epoll_del(epoll_fd, client_sock);
        websocket_close(client_sock);

        if (!is_null(args->callbacks.disconnect_callback)) {
          args->callbacks.disconnect_callback(client_sock);
        }

        goto FINALIZE;
      }
    }
  }

FINALIZE:
  websocket_epoll_del(epoll_fd, args->server_sock);
  websocket_close(epoll_fd);

  // Wipe buffer
  websocket_memset_s(buffer.request, buffer.capacity, 0x00, buffer.capacity);
  websocket_memset_s(buffer.response, buffer.capacity, 0x00, buffer.capacity);

  // Free buffer
  websocket_free(buffer.request);
  websocket_free(buffer.response);

  return true;
}
