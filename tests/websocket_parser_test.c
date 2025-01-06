/**
 * @file  websocket_parser_test.c
 */

#include "../src/websocket/websocket.h"
#include <string.h>
#include <alloca.h>

size_t calcHeaderSize(const size_t ext_payload_len);
void   createTestFrame(const size_t ext_payload_len, uint8_t* frame);

void websocket_parser_test()
{
    size_t  ext_payload_len = 3 * 1024 * 1024;
    size_t  header_size     = calcHeaderSize(ext_payload_len);
    size_t  frame_size      = header_size + ext_payload_len;
    uint8_t frame[frame_size];
    memset(frame, 0x00, sizeof(frame));

    createTestFrame(ext_payload_len, frame);

    WebsocketFrame out;
    out.payload = (char*)alloca((size_t)(ext_payload_len + 1));
    //out.payload = (char*)malloc((size_t)(ext_payload_len + 1));

    if (!parseWebsocketFrame((const uint8_t*)&frame, frame_size, &out)) {
        printf("Failed to parse websocket frame.\n");
        return;
    }

    printf("fsv             : %d\n", out.fin);
    printf("rsv1            : %d\n", out.rsv1);
    printf("rsv2            : %d\n", out.rsv2);
    printf("rsv3            : %d\n", out.rsv3);
    printf("opcode          : %d\n", out.opcode);
    printf("mask            : %d\n", out.mask);
    printf("payload_len     : %d\n", out.payload_len);
    printf("ext_payload_len : %ld\n", out.ext_payload_len);
    printf("payload         : %s\n", out.payload);
}

/**
 * @brief Calculate the header size of a websocket frame
 *
 * @param[in] ext_payload_len ext payload length of websocket frame
 *
 * @return Calculated header size
 */
size_t calcHeaderSize(const size_t ext_payload_len)
{
    // fin - payload_len : 2byte
    // masking key       : 4byte
    // ext header        : 0(if payload_len <= 125)
    //                     2(if payload_len =  126)
    //                     8(if payload_len =  127)
    if (ext_payload_len >= 65536) {
        return 14;
    } else if (ext_payload_len >= 126) {
        return 8;
    }

    return 6;
}

/**
 * @brief Create a websocket frame for testing.
 *
 * @param[in]  ext_payload_len ext payload length of websocket frame
 * @param[out] frame           Output destination of parsed frame
 *
 * @detail The payload is set to 01234567890...
 */
void createTestFrame(const size_t ext_payload_len, uint8_t* frame)
{
    size_t payload_len;
    size_t offset = 0;

    if (ext_payload_len >= 65536) {
        payload_len = 127;
    } else if (ext_payload_len >= 126) {
        payload_len = 126;
    } else {
        payload_len = ext_payload_len;
    }

    frame[offset] |= 0b10000000;  // fin(1)
    frame[offset] |= 0b00000000;  // rsv1(0)
    frame[offset] |= 0b00000000;  // rsv2(0)
    frame[offset] |= 0b00000000;  // rsv3(0)
    frame[offset] |= 0b00000001;  // opcode(1)
    offset++;
    frame[offset] |= 0b10000000;   // mask(1)
    frame[offset] |= payload_len;  // payload_len(127)
    offset++;

    if (payload_len == 127) {
        frame[offset++] = (ext_payload_len >> 56) & (0x000000FF);
        frame[offset++] = (ext_payload_len >> 48) & (0x000000FF);
        frame[offset++] = (ext_payload_len >> 40) & (0x000000FF);
        frame[offset++] = (ext_payload_len >> 32) & (0x000000FF);
        frame[offset++] = (ext_payload_len >> 24) & (0x000000FF);
        frame[offset++] = (ext_payload_len >> 16) & (0x000000FF);
        frame[offset++] = (ext_payload_len >> 8) & (0x000000FF);
        frame[offset++] = (ext_payload_len >> 0) & (0x000000FF);
    } else if (payload_len == 126) {
        frame[offset++] = (ext_payload_len >> 8);
        frame[offset++] = (ext_payload_len & 0x00FF);
    }

    uint8_t masking_key[] = {0x12, 0x34, 0x56, 0x78};
    for (int i = 0; i < 4; i++) {
        frame[offset++] = masking_key[i];
    }

    for (int i = 0; i < ext_payload_len; i++) {
        frame[offset++] = (((i % 10) + '0') ^ masking_key[i % 4]);
    }
}
