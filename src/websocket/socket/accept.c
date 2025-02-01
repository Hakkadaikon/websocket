#include <errno.h>
#include <string.h>
#include <sys/syscall.h>

#include "../websocket_local.h"
#include "optimize_socket.h"

int websocket_accept(const int sock_fd)
{
    struct sockaddr_in client_addr;
    socklen_t          addr_len = sizeof(client_addr);

    if (is_rise_signal()) {
        log_info("A signal was raised during accept(). The system will abort processing.\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    log_debug("accept...\n");
    int client_sock = accept(sock_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_sock < 0) {
        if (errno == EINTR || errno == EAGAIN) {
            return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
        }

        str_error("accept() failed. reason : ", strerror(errno));
        log_error("The system will abort processing.\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    if (optimize_client_socket(client_sock) == WEBSOCKET_ERRORCODE_FATAL_ERROR) {
        return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
    }

    return client_sock;
}
