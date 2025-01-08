#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "util/log.h"
#include "util/signal.h"
#include "websocket/websocket.h"

void websocket_callback(const int client_sock, PWebSocketFrame frame, const size_t client_buffer_capacity, char* response_buffer)
{
    switch (frame->opcode) {
        case WEBSOCKET_OP_CODE_TEXT: {
            frame->mask       = 0;
            size_t frame_size = create_websocket_frame(frame, client_buffer_capacity, (uint8_t*)&response_buffer[0]);
            if (frame_size == 0) {
                log_error("Failed to create websocket frame.\n");
                return;
            }

            websocket_server_send(client_sock, response_buffer, frame_size);
        } break;
        default:
            break;
    }
}

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

    websocket_server_loop(server_sock, client_buffer_size, websocket_callback);
    websocket_server_close(server_sock);

    log_info("websocket server end.\n");
    return 0;
}
