#include <stddef.h>
#include <websocket.h>
//#include "../../../src/websocket/websocket.h"

void websocket_callback(
    const int       client_sock,
    PWebSocketFrame frame,
    const size_t    buffer_capacity,
    char*           response_buffer)
{
    switch (frame->opcode) {
        case WEBSOCKET_OP_CODE_TEXT: {
            frame->mask       = 0;
            size_t frame_size = create_websocket_frame(frame, buffer_capacity, response_buffer);
            if (frame_size == 0) {
                log_error("Failed to create websocket frame.\n");
                return;
            }

            websocket_send(client_sock, frame_size, response_buffer);
        } break;
        default:
            break;
    }
}

int main()
{
    int    websocket_port_num     = 8080;
    int    backlog                = 5;
    size_t buffer_capacity        = 1024;

    int server_sock = websocket_server_init(websocket_port_num, backlog);
    if (server_sock < WEBSOCKET_ERRORCODE_NONE) {
        log_error("websocket server init error.\n");
        return 1;
    }

    WebSocketLoopArgs args;
    args.server_sock                     = server_sock;
    args.callbacks.receive_callback      = websocket_callback;
    args.callbacks.socket_close_callback = NULL;
    args.buffer_capacity                 = buffer_capacity;

    websocket_server_loop(&args);
    websocket_close(server_sock);

    log_error("websocket server end.\n");
    return 0;
}
