#ifndef __APPLE__
#include "../arch/epoll.h"
#endif

#include "websocket_local.h"

void websocket_frame_dump(PWebSocketFrame restrict frame)
{
    var_debug("fin             : ", frame->fin);
    var_debug("rsv1            : ", frame->rsv1);
    var_debug("rsv2            : ", frame->rsv2);
    var_debug("rsv3            : ", frame->rsv3);
    var_debug("opcode          : ", frame->opcode);
    var_debug("mask            : ", frame->mask);
    var_debug("payload_len     : ", frame->payload_len);
    var_debug("ext_payload_len : ", frame->ext_payload_len);
    str_debug("payload         : ", frame->payload);
}

void websocket_epoll_event_dump(const int32_t events)
{
#ifndef __APPLE__
    log_debug("epoll events: ");

    if (events & EPOLLIN) {
        log_debug("EPOLLIN ");
    }

    if (events & EPOLLERR) {
        log_debug("EPOLLIERR ");
    }

    if (events & EPOLLHUP) {
        log_debug("EPOLLHUP ");
    }

    if (events & EPOLLOUT) {
        log_debug("EPOLLOUT ");
    }

    if (events & EPOLLRDHUP) {
        log_debug("EPOLLRDHUP ");
    }

    log_debug("\n");
#endif
}
