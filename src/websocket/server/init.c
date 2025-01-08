#include "../../util/log.h"
#include "../websocket.h"

int websocket_server_init(const int port_num, const int backlog)
{
    log_info("websocket server init processing...\n");
    var_info("port    : ", port_num);
    var_info("backlog : ", backlog);

    int server_sock = websocket_server_connect(port_num, backlog);
    if (server_sock < 0) {
        return -1;
    }

    log_info("WebSocket server is listening...\n");
    return server_sock;
}
