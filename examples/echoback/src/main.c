#include <stddef.h>
//#include <websocket.h>
#include "../../../src/websocket/websocket.h"

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
    WebSocketInitArgs init_args;
    init_args.port_num = 8080;
    init_args.backlog  = 5;

    int server_sock = websocket_server_init(&init_args);
    if (server_sock < WEBSOCKET_ERRORCODE_NONE) {
        log_error("websocket server init error.\n");
        return 1;
    }

    WebSocketLoopArgs loop_args;
    loop_args.server_sock                     = server_sock;
    loop_args.callbacks.receive_callback      = websocket_callback;
    loop_args.callbacks.socket_close_callback = NULL;
    loop_args.buffer_capacity                 = 1024;

    websocket_server_loop(&loop_args);
    websocket_close(server_sock);

    log_error("websocket server end.\n");
    return 0;
}
