#include "websocket.h"
#include "../http/http.h"
#include "../util/log.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <alloca.h>
#include <pthread.h>

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

    while ((client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len)) >= 0) {
        //printf("Client connected.\n");

        PThreadData data = (PThreadData)alloca(sizeof(ThreadData));
        if (!data) {
            perror("Failed to allocate memory for client data");
            close(client_sock);
            continue;
        }

        data->client_sock        = client_sock;
        data->client_buffer_size = client_buffer_size;

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, client_handle_thread, data) != 0) {
            perror("Failed to create thread");
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
        fprintf(stderr, "Failed read buffer, sock[%d]\n", client_sock);
        return;
    }
    //printf("Received handshake request:\n%s\n", buffer);

    if (!extract_http_request(
            buffer,
            bytes_read,
            HTTP_HEADER_CAPACITY,
            request)) {
        return;
    }

    if (!is_valid_websocket_request(request)) {
        fprintf(stderr, "Invalid WebSocket handshake request: Invalid parameter: [%s]\n", buffer);
        return;
    }

    char* client_key = select_websocket_client_key(request);
    if (client_key == NULL) {
        perror("Invalid WebSocket handshake request: select websocket client key\n");
        return;
    }

    char accept_key[HTTP_HEADER_VALUE_CAPACITY];
    if (!generate_websocket_acceptkey(client_key, sizeof(accept_key), accept_key)) {
        perror("Invalid WebSocket handshake request: generate accept key\n");
        return;
    }

    char response[buffer_capacity];
    if (!create_handshake_ok_flame(accept_key, sizeof(response), response)) {
        return;
    }
    send(client_sock, response, strnlen(response, sizeof(response)), 0);
    //printf("Sent handshake response. client_key:[%s] accept_key:[%s]\n", client_key, accept_key);

    while ((bytes_read = read_buffer(client_sock, sizeof(buffer), buffer)) > 0) {
        //printf("Received data: %ld bytes\n", bytes_read);
        //printf("hex dump\n");
        //hex_dump(buffer, bytes_read);

        WebsocketFrame frame;
        memset(&frame, 0x00, sizeof(frame));
        frame.payload = alloca(bytes_read);
        parseWebsocketFrame((uint8_t*)buffer, bytes_read, &frame);
        //websocket_frame_dump(&frame);

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
                //printf("send hex dump recv size: %ld send size:%ld\n", bytes_read, frame_size);
                //hex_dump(response, frame_size);

                send(client_sock, response, frame_size, 0);
                break;
            case 0x2:  // binary frame
            case 0x9:  // ping
            case 0xA:  // pong
            default:
                break;
        }
    }
}
