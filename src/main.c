#include <stdint.h>
#include <stdlib.h>
#include "util/signal.h"
#include "util/log.h"
#include "websocket/websocket.h"

int main()
{
    int    websocket_port_num = 8080;
    int    backlog            = 5;
    size_t client_buffer_size = 1024;

    signal_init();

    int server_sock = websocket_server_init(websocket_port_num, backlog);
    if (server_sock == -1) {
        log_error("websocket server init error.\n");
        return 1;
    }

    websocket_client_loop(server_sock, client_buffer_size);
    websocket_server_close(server_sock);

    log_info("websocket server end.\n");
    return 0;
}
