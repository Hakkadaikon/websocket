#include "../util/log.h"

#include "websocket.h"

void websocket_frame_dump(PWebsocketFrame restrict frame)
{
    var_debug("fin             : ", frame->fin);
    var_debug("rsv1            : ", frame->rsv1);
    var_debug("rsv2            : ", frame->rsv2);
    var_debug("rsv3            : ", frame->rsv3);
    var_debug("opcode          : ", frame->opcode);
    var_debug("mask            : ", frame->mask);
    var_debug("payload_len     : ", frame->payload_len);
    var_debug("ext_payload_len : ", frame->ext_payload_len);
    log_debug("payload         :\n");
    log_debug(frame->payload);
    log_debug("\n");
}
