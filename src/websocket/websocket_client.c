#include "websocket.h"
#include "../http/http.h"
#include "../util/log.h"
#include "../util/signal.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <alloca.h>
#include <pthread.h>
#include <errno.h>

typedef struct {
    int    client_sock;
    size_t client_buffer_size;
} ThreadData, *PThreadData;

static void* client_handle_thread(void* arg);
static void  client_handle(const int client_sock, const size_t buffer_capacity, PHTTPRequest request);

static void nothing(void* ptr)
{
    // nothing...
}

bool websocket_client_loop(int server_sock, const size_t client_buffer_size)
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

        data->client_sock        = client_sock;
        data->client_buffer_size = client_buffer_size;

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

static void* client_handle_thread(void* restrict arg)
{
    PThreadData data = (PThreadData)arg;
    HTTPRequest request;
    ALLOCATE_HTTP_REQUEST(request, alloca);
    //ALLOCATE_HTTP_REQUEST(request, malloc);
    client_handle(data->client_sock, data->client_buffer_size, &request);
    close(data->client_sock);
    FREE_HTTP_REQUEST(request, nothing)
    //FREE_HTTP_REQUEST(request, free)
    return NULL;
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

static void client_handle(const int client_sock, const size_t buffer_capacity, PHTTPRequest request)
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
    if (!create_handshake_ok_flame(accept_key, sizeof(response), response)) {
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
        WebsocketFrame frame;
        memset(&frame, 0x00, sizeof(frame));
        frame.payload = alloca(bytes_read);
        parseWebsocketFrame((uint8_t*)buffer, bytes_read, &frame);
        websocket_frame_dump(&frame);

        // close frame
        if (frame.opcode == 0x8) {
            break;
        }

        switch (frame.opcode) {
            case 0x1:  // text frame
                memset(response, 0x00, sizeof(response));
                frame.mask        = 0;
                size_t frame_size = createWebsocketFrame(&frame, sizeof(response), (uint8_t*)&response[0]);
                if (frame_size == 0) {
                    //printf("create frame error\n");
                    return;
                }
                send(client_sock, response, frame_size, 0);
                break;
            case 0x2:  // binary frame
            case 0x9:  // ping
            case 0xA:  // pong
            default:
                break;
        }

        if (is_rise_signal()) {
            var_info("rise signal. sock : ", client_sock);
            break;
        }
    }
}
