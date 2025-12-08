#ifndef NOSTR_SERVER_LOOP_RECEIVE_HANDLE_H_
#define NOSTR_SERVER_LOOP_RECEIVE_HANDLE_H_

#include "../../../../util/allocator.h"
#include "../../../websocket_local.h"
#include "opcode_handle.h"

static inline int32_t receive_handle(
  const int32_t       client_sock,
  const size_t        read_size,
  PWebSocketRawBuffer buffer,
  PWebSocketCallbacks callbacks)
{
  WebSocketFrame frame;
  websocket_memset(&frame, 0x00, sizeof(frame));

  frame.payload = (char*)websocket_alloc(read_size);

  int32_t rtn = 0;

  if (!parse_websocket_frame(buffer->request, read_size, &frame)) {
    rtn = WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
    goto FINALIZE;
  }

  websocket_frame_dump(&frame);

  rtn = opcode_handle(client_sock, buffer, callbacks, &frame);
  if (rtn != WEBSOCKET_ERRORCODE_NONE) {
    goto FINALIZE;
  }

  if (is_rise_signal()) {
    var_info("rise signal. sock : ", client_sock);
    rtn = WEBSOCKET_ERRORCODE_FATAL_ERROR;
    goto FINALIZE;
  }

FINALIZE:
  websocket_free(frame.payload);
  return rtn;
}

#endif
