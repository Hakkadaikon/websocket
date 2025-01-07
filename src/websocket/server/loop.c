#include <alloca.h>
#include <arpa/inet.h>
#include <errno.h>
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

static inline ssize_t read_buffer(const int client_sock, const size_t capacity, char* restrict buffer)
{
    ssize_t bytes_read = recv(client_sock, buffer, capacity - 1, 0);
    if (bytes_read <= 0) {
        return 0;
    }

    buffer[bytes_read] = '\0';
    return bytes_read;
}

static void client_handle(const int client_sock, const size_t buffer_capacity, PHTTPRequest request, PWebSocketCallback callback)
{
    char    buffer[buffer_capacity];
    ssize_t bytes_read;

    if ((bytes_read = read_buffer(client_sock, sizeof(buffer), buffer)) == 0) {
        var_error("Failed read buffer, sock : ", client_sock);
        return;
    }
    log_debug("Received handshake request : ");
    log_debug(buffer);
    log_debug("\n");

    if (!extract_http_request(
            buffer,
            bytes_read,
            HTTP_HEADER_CAPACITY,
            request)) {
        return;
    }

    if (!is_valid_websocket_request(request)) {
        log_error("Invalid WebSocket handshake request. Invalid parameter : ");
        log_error(buffer);
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

    char response[buffer_capacity];
    if (!create_server_handshake_ok_frame(accept_key, sizeof(response), response)) {
        log_error("Invalid WebSocket handshake request. Failed create OK flame.\n");
        return;
    }

    send(client_sock, response, strnlen(response, sizeof(response)), 0);
    log_debug("Sent handshake response. \n");
    log_debug("client key : ");
    log_debug(client_key);
    log_debug("\n");
    log_debug("accept key : ");
    log_debug(accept_key);
    log_debug("\n");

    while ((bytes_read = read_buffer(client_sock, sizeof(buffer), buffer)) > 0) {
        WebSocketFrame frame;
        memset(&frame, 0x00, sizeof(frame));
        frame.payload = alloca(bytes_read);
        parse_websocket_frame((uint8_t*)buffer, bytes_read, &frame);
        websocket_frame_dump(&frame);

        // close frame
        if (frame.opcode == WEBSOCKET_OP_CODE_CLOSE) {
            break;
        }

        switch (frame.opcode) {
            case WEBSOCKET_OP_CODE_TEXT: {
                callback(client_sock, &frame, buffer_capacity);
            } break;
            case WEBSOCKET_OP_CODE_BINARY: {
                callback(client_sock, &frame, buffer_capacity);
            } break;
            case WEBSOCKET_OP_CODE_PING: {
                memset(response, 0x00, sizeof(response));
                frame.mask   = 0;
                frame.opcode = WEBSOCKET_OP_CODE_PONG;

                size_t frame_size = create_websocket_frame(&frame, sizeof(response), (uint8_t*)&response[0]);
                if (frame_size == 0) {
                    log_error("Failed to create pong frame.\n");
                    return;
                }
                send(client_sock, response, frame_size, 0);
            } break;
            case WEBSOCKET_OP_CODE_PONG: {
            } break;
            default:
                var_error("Unknown op code: ", frame.opcode);
                break;
        }

        if (is_rise_signal()) {
            var_info("rise signal. sock : ", client_sock);
            break;
        }
    }
}

static void* client_handle_thread(void* restrict arg)
{
    PThreadData data = (PThreadData)arg;
    HTTPRequest request;
    ALLOCATE_HTTP_REQUEST(request, alloca);
    //ALLOCATE_HTTP_REQUEST(request, malloc);
    client_handle(data->client_sock, data->client_buffer_capacity, &request, data->callback);
    close(data->client_sock);
    FREE_HTTP_REQUEST(request, nothing)
    //FREE_HTTP_REQUEST(request, free)
    return NULL;
}

bool websocket_server_loop(int server_sock, const size_t client_buffer_capacity, PWebSocketCallback callback)
{
    int                client_sock;
    struct sockaddr_in client_addr;
    socklen_t          addr_len = sizeof(client_addr);

    while (1) {
        log_debug("accept...\n");
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
        if (client_sock < 0) {
            if ((errno != EINTR)) {
                log_error("accept() failed. err : ");
                log_error(strerror(errno));
                log_error("\n");
                log_error("The system will abort processing.\n");
                break;
            }

            if (is_rise_signal()) {
                log_info("A signal was raised during accept(). The system will abort processing.\n");
                break;
            }

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
