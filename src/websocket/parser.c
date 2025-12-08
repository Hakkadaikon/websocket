/**
 * @file  websocket_parse.c
 *
 * @brief Parses each parameter of a websocket frame stored in network byte order.
 * @see RFC6455 (https://datatracker.ietf.org/doc/html/rfc6455)
 */

#include "../util/allocator.h"
#include "websocket_local.h"

/**
 * @brief Parse raw data in network byte order into a websocket frame structure
 *
 * @param[in]  raw         Raw data (network byte order)
 * @param[in]  frame_size  Capacity of raw data
 * @param[out] frame       Output destination of parsed frame
 *
 * @return true: Parse was successful / false: Failed parse
 */
bool parse_websocket_frame(const char* restrict raw, const size_t capacity, PWebSocketFrame restrict frame)
{
  if (capacity < 2) {
    return false;
  }

  if (is_null(frame)) {
    return false;
  }

  if (is_null(frame->payload)) {
    return false;
  }

  //  0                   1                   2                   3
  //  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  // +-+-+-+-+-------+-+-------------+-------------------------------+
  // |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
  // |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
  // |N|V|V|V|       |S|             |   (if payload len==126/127)   |
  // | |1|2|3|       |K|             |                               |
  // +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
  // |     Extended payload length continued, if payload len == 127  |
  // + - - - - - - - - - - - - - - - +-------------------------------+
  // |                               |Masking-key, if MASK set to 1  |
  // +-------------------------------+-------------------------------+
  // | Masking-key (continued)       |          Payload Data         |
  // +-------------------------------- - - - - - - - - - - - - - - - +
  // :                     Payload Data continued ...                :
  // + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
  // |                     Payload Data continued ...                |
  // +---------------------------------------------------------------+
  // 0-7bit
  // +-+-+-+-+-------+
  // |F|R|R|R| opcode|
  // |I|S|S|S|  (4)  |
  // |N|V|V|V|       |
  // | |1|2|3|       |
  // +-+-+-+-+-------+
  frame->fin    = (raw[0] & 0x80) >> 7;
  frame->rsv1   = (raw[0] & 0x40) >> 6;
  frame->rsv2   = (raw[0] & 0x20) >> 5;
  frame->rsv3   = (raw[0] & 0x10) >> 4;
  frame->opcode = (raw[0] & 0x0F);

  // 8-15bit
  // +-+-------------+
  // |M| Payload len |
  // |A|     (7)     |
  // |S|             |
  // |K|             |
  // +-+-------------+
  frame->mask        = (raw[1] & 0x80) >> 7;
  frame->payload_len = (raw[1] & 0x7F);

  size_t frame_offset = 2;

  // Expanded payload length
  // - nothing  (if payload_len <= 125)
  // - 16-31bit (if payload_len = 126)
  // - 16-79bit (if payload_len = 127)
  //
  //  0                   1                   2                   3
  //  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  //                                 +-------------------------------+
  //                                 |    Extended payload length    |
  //                                 |             (16/64)           |
  //                                 |   (if payload len==126/127)   |
  //                                 |                               |
  // +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
  // |     Extended payload length continued, if payload len == 127  |
  // + - - - - - - - - - - - - - - - +-------------------------------+
  // |                               |
  // +-------------------------------+
  if (frame->payload_len == 126) {
    if (capacity < 4) {
      return false;
    }
    frame->ext_payload_len = (raw[2] << 8) | raw[3];
    if (frame->ext_payload_len > capacity) {
      return false;
    }
    frame_offset += 2;
  } else if (frame->payload_len == 127) {
    if (capacity < 10) {
      return false;
    }

    for (int32_t i = 0; i < 8; i++) {
      frame->ext_payload_len = (frame->ext_payload_len << 8) | raw[2 + i];
    }
    frame_offset += 8;
  } else {
    frame->ext_payload_len = frame->payload_len;
  }

  // masking key
  // - 16-47bit  (if payload_len <= 125)
  // - 32-63bit  (if payload_len = 126)
  // - 80-111bit (if payload_len = 127)
  //
  //  0                   1                   2                   3
  //  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  // + - - - - - - - - - - - - - - - +-------------------------------+
  // |                               |Masking-key, if MASK set to 1  |
  // +-------------------------------+-------------------------------+
  // | Masking-key (continued)       |
  // +--------------------------------
  if (frame->mask) {
    if (capacity < frame_offset + 4) {
      return false;
    }

    websocket_memcpy(frame->masking_key, &raw[frame_offset], 4);
    frame_offset += sizeof(frame->masking_key);
  }

  if (frame->ext_payload_len > (capacity - frame_offset)) {
    return false;
  }

  const char* payload_raw = &raw[frame_offset];
  for (size_t i = 0; i < frame->ext_payload_len; i++) {
    frame->payload[i] =
      payload_raw[i] ^ (frame->mask ? frame->masking_key[i % 4] : 0);
  }
  frame->payload[frame->ext_payload_len] = '\0';
  return true;
}

size_t create_websocket_frame(PWebSocketFrame restrict frame, const size_t capacity, char* restrict raw)
{
  if (!frame || !raw || capacity < 2) {
    return 0;
  }

  size_t offset = 0;

  // 0-7bit
  // +-+-+-+-+-------+
  // |F|R|R|R| opcode|
  // |I|S|S|S|  (4)  |
  // |N|V|V|V|       |
  // | |1|2|3|       |
  // +-+-+-+-+-------+
  raw[offset] = ((frame->fin & 0x01) << 7) |
                ((frame->rsv1 & 0x01) << 6) |
                ((frame->rsv2 & 0x01) << 5) |
                ((frame->rsv3 & 0x01) << 4) |
                (frame->opcode & 0x0F);
  offset++;

  // 8-15bit
  // +-+-------------+
  // |M| Payload len |
  // |A|     (7)     |
  // |S|             |
  // |K|             |
  // +-+-------------+
  unsigned char mask = (frame->mask ? 0x80 : 0x00);

  if (frame->ext_payload_len <= 125 && !frame->mask) {
    raw[offset] = frame->ext_payload_len & 0x7F;
    offset++;
    websocket_memcpy(&raw[offset], frame->payload, frame->ext_payload_len);
    return offset + frame->ext_payload_len;
  }

  if (frame->ext_payload_len > 125 && frame->ext_payload_len <= 0xFFFF) {
    raw[offset] = mask | 126;  // Mask set + payload length 126
    offset++;
    if (capacity < offset + 2) {
      return 0;
    }
    raw[offset]     = (frame->ext_payload_len >> 8) & 0xFF;
    raw[offset + 1] = frame->ext_payload_len & 0xFF;
    offset += 2;
  } else if (frame->ext_payload_len > 0xFFFF) {
    raw[offset] = mask | 127;  // Mask set + payload length 127
    offset++;
    if (capacity < offset + 8) {
      return 0;
    }
    for (int32_t i = 7; i >= 0; i--) {
      raw[offset + i] = frame->ext_payload_len & 0xFF;
      frame->ext_payload_len >>= 8;
    }
    offset += 8;
  } else {
    raw[offset] = mask | frame->ext_payload_len;
    offset++;
  }

  // Masking key (if mask is set)
  if (frame->mask) {
    if (capacity < offset + 4) {
      return 0;
    }
    websocket_memcpy(&raw[offset], frame->masking_key, 4);
    offset += 4;
  }

  // Payload data
  if (capacity < offset + frame->ext_payload_len) {
    return 0;
  }

  if (frame->mask) {
    // use loop unroll
    size_t i = 0;
    for (; i + 4 <= frame->ext_payload_len; i += 4) {
      raw[offset + i + 0] = frame->payload[i + 0] ^ frame->masking_key[0];
      raw[offset + i + 1] = frame->payload[i + 1] ^ frame->masking_key[1];
      raw[offset + i + 2] = frame->payload[i + 2] ^ frame->masking_key[2];
      raw[offset + i + 3] = frame->payload[i + 3] ^ frame->masking_key[3];
    }
    for (; i < frame->ext_payload_len; i++) {
      raw[offset + i] = frame->payload[i] ^ frame->masking_key[i % 4];
    }
  } else {
    websocket_memcpy(&raw[offset], &frame->payload[0], frame->ext_payload_len);
  }

  offset += frame->ext_payload_len;

  return offset;
}
