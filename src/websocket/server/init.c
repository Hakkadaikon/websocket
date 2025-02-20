#include "../websocket_local.h"

int32_t websocket_server_init(const PWebSocketInitArgs args)
{
    log_info("websocket server init processing...\n");
    var_info("port    : ", args->port_num);
    var_info("backlog : ", args->backlog);

    if (!signal_init()) {
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    int32_t server_sock = websocket_listen(args->port_num, args->backlog);
    if (server_sock < 0) {
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    log_info("WebSocket server is listening...\n");
    return server_sock;
}
