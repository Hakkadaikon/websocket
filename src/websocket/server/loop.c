#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "../../util/log.h"
#include "../../util/signal.h"
#include "../websocket.h"

typedef struct {
    int                client_sock;
    size_t             client_buffer_capacity;
    PWebSocketCallback callback;
} ThreadData, *PThreadData;

static void* client_handle_thread(void* arg);

static void nothing(void* ptr)
{
    // nothing...
}

static inline bool opcode_handle(
    const int                client_sock,
    const size_t             buffer_capacity,
    char* restrict           response_buffer,
    PWebSocketCallback       callback,
    PWebSocketFrame restrict frame)
{
    switch (frame->opcode) {
        case WEBSOCKET_OP_CODE_TEXT:
            callback(client_sock, frame, buffer_capacity, response_buffer);
            break;
        case WEBSOCKET_OP_CODE_BINARY:
            callback(client_sock, frame, buffer_capacity, response_buffer);
            break;
        case WEBSOCKET_OP_CODE_CLOSE:
            return false;
        case WEBSOCKET_OP_CODE_PING:
            frame->mask   = 0;
            frame->opcode = WEBSOCKET_OP_CODE_PONG;

            size_t frame_size = create_websocket_frame(
                frame, buffer_capacity, (uint8_t*)&response_buffer[0]);
            if (frame_size == 0) {
                log_error("Failed to create pong frame.\n");
                return false;
            }
            websocket_server_send(client_sock, response_buffer, frame_size);
            break;
        case WEBSOCKET_OP_CODE_PONG:
            break;
        default:
            var_error("Unknown op code: ", frame->opcode);
            break;
    }

    return true;
}

static inline void client_handle(
    const int             client_sock,
    const size_t          buffer_capacity,
    char* restrict        request_buffer,
    char* restrict        response_buffer,
    PHTTPRequest restrict request,
    PWebSocketCallback    callback)
{
    ssize_t bytes_read;

    if ((bytes_read = websocket_server_recv(client_sock, buffer_capacity, request_buffer)) <= 0) {
        return;
    }

    memset(response_buffer, 0x00, buffer_capacity);

    if (!client_handshake(client_sock, buffer_capacity, bytes_read, request_buffer, response_buffer, request)) {
        return;
    }

    while ((bytes_read = websocket_server_recv(client_sock, buffer_capacity, request_buffer)) > 0) {
        WebSocketFrame frame;
        memset(&frame, 0x00, sizeof(frame));
        frame.payload = alloca(bytes_read);

        if (!parse_websocket_frame((uint8_t*)request_buffer, bytes_read, &frame)) {
            continue;
        }

        websocket_frame_dump(&frame);

        if (!opcode_handle(client_sock, buffer_capacity, response_buffer, callback, &frame)) {
            return;
        }

        if (is_rise_signal()) {
            var_info("rise signal. sock : ", client_sock);
            return;
        }
    }
}

static void* client_handle_thread(void* restrict arg)
{
    PThreadData data = (PThreadData)arg;
    HTTPRequest request;
    bool        is_allocate_stack = true;

    char* request_buffer  = NULL;
    char* response_buffer = NULL;

    if (is_allocate_stack) {
        ALLOCATE_HTTP_REQUEST(request, alloca);
        request_buffer  = alloca(data->client_buffer_capacity);
        response_buffer = alloca(data->client_buffer_capacity);
    } else {
        ALLOCATE_HTTP_REQUEST(request, malloc);
        request_buffer  = malloc(data->client_buffer_capacity);
        response_buffer = malloc(data->client_buffer_capacity);
    }

    if (request_buffer == NULL || response_buffer == NULL) {
        log_error("Failed to allocate request/response buffer.\n");
        goto FINALIZE;
    }

    client_handle(data->client_sock, data->client_buffer_capacity, request_buffer, response_buffer, &request, data->callback);

FINALIZE:
    websocket_server_close(data->client_sock);

    if (is_allocate_stack) {
        FREE_HTTP_REQUEST(request, nothing)
    } else {
        FREE_HTTP_REQUEST(request, free)
        if (request_buffer != NULL) free(request_buffer);
        if (response_buffer != NULL) free(response_buffer);
    }

    return NULL;
}

bool websocket_server_loop(int server_sock, const size_t client_buffer_capacity, PWebSocketCallback callback)
{
    while (1) {
        int client_sock = websocket_server_accept(server_sock);
        if (client_sock == -2) {
            return false;
        }

        if (client_sock == -1) {
            continue;
        }

        log_debug("Client connected.\n");

        PThreadData data = (PThreadData)alloca(sizeof(ThreadData));
        if (!data) {
            log_error("Failed to allocate memory for client data\n");
            websocket_server_close(client_sock);
            continue;
        }

        data->client_sock            = client_sock;
        data->client_buffer_capacity = client_buffer_capacity;
        data->callback               = callback;

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, client_handle_thread, data) != 0) {
            var_error("Failed to create thread. id :", pthread_self());
            websocket_server_close(client_sock);
            continue;
        }

        pthread_detach(thread_id);
    }

    return true;
}
