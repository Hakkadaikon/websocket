#include <arpa/inet.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "../../util/log.h"
#include "../websocket.h"

int websocket_server_close(const int server_sock)
{
    log_info("WebSocket server close...\n");
    return close(server_sock);
}

int websocket_server_send(const int client_sock, const char* restrict buffer, const size_t buffer_size)
{
    log_debug("WebSocket server send\n");

    int rtn = send(client_sock, buffer, buffer_size, 0);
    if (rtn == -1) {
        char* reason = strerror(errno);
        log_error("Failed to send().\n");
        log_error("reason : ");
        log_error(reason);
        log_error("\n");
    }

    return rtn;
}
