#include <arpa/inet.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "../../util/log.h"
#include "../../util/signal.h"
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

ssize_t websocket_server_recv(const int client_sock, const size_t capacity, char* restrict buffer)
{
    ssize_t bytes_read = recv(client_sock, buffer, capacity - 1, 0);
    if (bytes_read == 0) {
        var_error("Socket was disconnected. socket : ", client_sock);
        return 0;
    } else if (bytes_read == -1) {
        char* errmsg = strerror(errno);
        log_error("Failed to recv error. reason : ");
        log_error(errmsg);
        log_error("\n");
        var_error("socket : ", client_sock);
        return 0;
    }

    buffer[bytes_read] = '\0';
    return bytes_read;
}

int websocket_server_accept(const int server_sock)
{
    struct sockaddr_in client_addr;
    socklen_t          addr_len = sizeof(client_addr);

    log_debug("accept...\n");
    int client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_len);
    if (client_sock < 0) {
        if ((errno != EINTR)) {
            log_error("accept() failed. err : ");
            log_error(strerror(errno));
            log_error("\n");
            log_error("The system will abort processing.\n");
            return -2;
        }

        if (is_rise_signal()) {
            log_info("A signal was raised during accept(). The system will abort processing.\n");
            return -2;
        }

        return -1;
    }

    return client_sock;
}

int websocket_server_connect(const int port_num, const int backlog)
{
    int                server_sock;
    struct sockaddr_in server_addr;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        log_error("Failed to create socket.\n");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(port_num);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        log_error("Failed to bind.\n");
        close(server_sock);
        return -1;
    }

    if (listen(server_sock, backlog) < 0) {
        log_error("Failed to listen.\n");
        close(server_sock);
        return -1;
    }

    return server_sock;
}
