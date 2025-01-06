#include "websocket.h"
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>
#include "../util/log.h"

int websocket_server_init(const int port_num, const int backlog)
{
    int                server_sock;
    struct sockaddr_in server_addr;

    log_info("websocket server init processing...\n");
    var_info("port    : ", port_num);
    var_info("backlog : ", backlog);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        log_error("Failed to create socket.\n");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(port_num);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        log_error("Failed to bind.\n");
        close(server_sock);
        return -1;
    }

    if (listen(server_sock, backlog) < 0) {
        log_error("Failed to listen.\n");
        close(server_sock);
        return -1;
    }

    log_info("WebSocket server is listening...\n");
    return server_sock;
}

int websocket_server_close(const int server_sock)
{
    log_info("WebSocket server close...\n");
    return close(server_sock);
}
