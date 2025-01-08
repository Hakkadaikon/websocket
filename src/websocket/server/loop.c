#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../http/http.h"
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

static void client_handle(
    const int             client_sock,
    const size_t          buffer_capacity,
    char* restrict        request_buffer,
    char* restrict        response_buffer,
    PHTTPRequest restrict request,
    PWebSocketCallback    callback)
{
    ssize_t bytes_read;

    if ((bytes_read = websocket_server_recv(client_sock, buffer_capacity, request_buffer)) == 0) {
        return;
    }

    log_debug("Received handshake request : ");
    log_debug(request_buffer);
    log_debug("\n");

    if (!extract_http_request(
            request_buffer,
            bytes_read,
            HTTP_HEADER_CAPACITY,
            request)) {
        return;
    }

    if (!is_valid_websocket_request(request)) {
        log_error("Invalid WebSocket handshake request. Invalid parameter : ");
        log_error(request_buffer);
        log_error("\n");
        return;
    }

    char* client_key = select_websocket_client_key(request);
    if (client_key == NULL) {
        log_error("Invalid WebSocket handshake request. Invalid websocket client key\n");
        return;
    }

    char accept_key[HTTP_HEADER_VALUE_CAPACITY];
    if (!generate_websocket_acceptkey(client_key, sizeof(accept_key), accept_key)) {
        log_error("Invalid WebSocket handshake request. Failed generate accept key\n");
        return;
    }

    if (!create_server_handshake_ok_frame(accept_key, buffer_capacity, response_buffer)) {
        log_error("Invalid WebSocket handshake request. Failed create OK flame.\n");
        return;
    }

    websocket_server_send(client_sock, response_buffer, strnlen(response_buffer, buffer_capacity));
    log_debug("Sent handshake response. \n");
    log_debug("client key : ");
    log_debug(client_key);
    log_debug("\n");
    log_debug("accept key : ");
    log_debug(accept_key);
    log_debug("\n");
    memset(response_buffer, 0x00, buffer_capacity);

    while ((bytes_read = websocket_server_recv(client_sock, buffer_capacity, request_buffer)) > 0) {
        WebSocketFrame frame;
        memset(&frame, 0x00, sizeof(frame));
        frame.payload = alloca(bytes_read);
        parse_websocket_frame((uint8_t*)request_buffer, bytes_read, &frame);
        websocket_frame_dump(&frame);

        switch (frame.opcode) {
            case WEBSOCKET_OP_CODE_TEXT:
                callback(client_sock, &frame, buffer_capacity, response_buffer);
                break;
            case WEBSOCKET_OP_CODE_BINARY:
                callback(client_sock, &frame, buffer_capacity, response_buffer);
                break;
            case WEBSOCKET_OP_CODE_CLOSE:
                return;
            case WEBSOCKET_OP_CODE_PING:
                frame.mask   = 0;
                frame.opcode = WEBSOCKET_OP_CODE_PONG;

                size_t frame_size = create_websocket_frame(
                    &frame, buffer_capacity, (uint8_t*)&response_buffer[0]);
                if (frame_size == 0) {
                    log_error("Failed to create pong frame.\n");
                    return;
                }
                websocket_server_send(client_sock, response_buffer, frame_size);
                break;
            case WEBSOCKET_OP_CODE_PONG:
                break;
            default:
                var_error("Unknown op code: ", frame.opcode);
                break;
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
    int client_sock;

    while (1) {
        client_sock = websocket_server_accept(server_sock);
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
            close(client_sock);
            continue;
        }

        data->client_sock            = client_sock;
        data->client_buffer_capacity = client_buffer_capacity;
        data->callback               = callback;

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, client_handle_thread, data) != 0) {
            var_error("Failed to create thread. id :", thread_id);
            close(client_sock);
            continue;
        }

        pthread_detach(thread_id);
    }

    return true;
}
