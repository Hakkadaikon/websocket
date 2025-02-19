//#include <websocket.h>
//#include <stdio.h>
#include "../../../src/websocket/websocket.h"

void websocket_receive_callback(
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

void websocket_connect_callback(int client_sock)
{
    //printf("[user] connect. socket fd : %d\n", client_sock);
    //fflush(stdout);
}

void websocket_disconnect_callback(int client_sock)
{
    //printf("[user] disconnect. socket fd : %d\n", client_sock);
    //fflush(stdout);
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
    loop_args.callbacks.receive_callback      = websocket_receive_callback;
    loop_args.callbacks.connect_callback      = websocket_connect_callback;
    loop_args.callbacks.disconnect_callback   = websocket_disconnect_callback;
    loop_args.buffer_capacity                 = 1024;

    websocket_server_loop(&loop_args);
    websocket_close(server_sock);

    log_error("websocket server end.\n");
    return 0;
}
