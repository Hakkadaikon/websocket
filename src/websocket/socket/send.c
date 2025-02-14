#include <string.h>
#include <sys/socket.h>
#include <sys/syscall.h>

#include "../websocket_local.h"
#include "./errno.h"

int websocket_send(const int sock_fd, const size_t buffer_size, const char* restrict buffer)
{
    log_debug("WebSocket server send\n");

    ssize_t rtn = send(sock_fd, buffer, buffer_size, 0);
    if (rtn == WEBSOCKET_SYSCALL_ERROR) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
        }

        return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
        str_error("Failed to send(). reason : ", strerror(errno));
    }

    return WEBSOCKET_ERRORCODE_NONE;
}
