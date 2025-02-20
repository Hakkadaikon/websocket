#include "../../arch/send.h"

#include "../websocket_local.h"

int32_t websocket_send(const int32_t sock_fd, const size_t buffer_size, const char* restrict buffer)
{
    log_debug("WebSocket server send\n");

    ssize_t rtn = internal_sendto(sock_fd, buffer, buffer_size, 0, NULL, 0);
    if (rtn == WEBSOCKET_SYSCALL_ERROR) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
        }

        return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
        str_error("Failed to send(). reason : ", strerror(errno));
    }

    return WEBSOCKET_ERRORCODE_NONE;
}
