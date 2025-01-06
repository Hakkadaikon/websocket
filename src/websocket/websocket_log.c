#include "websocket.h"

void websocket_frame_dump(PWebsocketFrame restrict frame)
{
    printf("fin             : %d\n", frame->fin);
    printf("rsv1            : %d\n", frame->rsv1);
    printf("rsv2            : %d\n", frame->rsv2);
    printf("rsv3            : %d\n", frame->rsv3);
    printf("opcode          : %d\n", frame->opcode);
    printf("mask            : %d\n", frame->mask);
    printf("payload_len     : %d\n", frame->payload_len);
    printf("ext_payload_len : %ld\n", frame->ext_payload_len);
    printf("payload         : %s\n", frame->payload);
    fflush(stdout);
}
