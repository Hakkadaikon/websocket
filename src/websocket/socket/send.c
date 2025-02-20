#include "../../arch/send.h"

#include "../websocket_local.h"

static int32_t get_send_err(ssize_t bytes_send);

int32_t websocket_send(const int32_t sock_fd, const size_t buffer_size, const char* restrict buffer)
{
    log_debug("WebSocket server send\n");

    int32_t errcode;
    ssize_t bytes_send = internal_sendto(sock_fd, buffer, buffer_size, 0, NULL, 0);
    if ((errcode = get_send_err(bytes_send)) != WEBSOCKET_ERRORCODE_NONE) {
        return errcode;
    }

    return WEBSOCKET_ERRORCODE_NONE;
}

static int32_t get_send_err(ssize_t bytes_send)
{
    if (bytes_send == WEBSOCKET_SYSCALL_ERROR) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
        }

        return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
        str_error("Failed to send(). reason : ", strerror(errno));
    }

    return WEBSOCKET_ERRORCODE_NONE;
}
