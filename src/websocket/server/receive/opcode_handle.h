#ifndef NOSTR_SERVER_LOOP_OPCODE_HANDLE_H_
#define NOSTR_SERVER_LOOP_OPCODE_HANDLE_H_

#include "../../../util/allocator.h"
#include "../../websocket_local.h"

static inline int32_t opcode_handle(
  const int32_t       client_sock,
  PWebSocketRawBuffer buffer,
  PWebSocketCallbacks callbacks,
  PWebSocketFrame     frame)
{
  switch (frame->opcode) {
    case WEBSOCKET_OP_CODE_TEXT:
    case WEBSOCKET_OP_CODE_BINARY:
      if (!is_null(callbacks->receive_callback)) {
        callbacks->receive_callback(client_sock, frame, buffer->capacity, buffer->response);
      }

      break;
    case WEBSOCKET_OP_CODE_CLOSE:
      return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
    case WEBSOCKET_OP_CODE_PING: {
      frame->mask   = 0;
      frame->opcode = WEBSOCKET_OP_CODE_PONG;

      size_t frame_size = create_websocket_frame(frame, buffer->capacity, buffer->response);
      if (frame_size == 0) {
        log_error("Failed to create pong frame.\n");
        return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
      }

      int32_t rtn = websocket_send(client_sock, frame_size, buffer->response);
      if (rtn != WEBSOCKET_ERRORCODE_NONE) {
        return rtn;
      }

      break;
    }
    case WEBSOCKET_OP_CODE_PONG:
      break;
    default:
      var_error("Unknown op code: ", frame->opcode);
      return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
  }

  return WEBSOCKET_ERRORCODE_NONE;
}

#endif
