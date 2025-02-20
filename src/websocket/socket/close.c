#include "../../arch/close.h"

#include "../websocket_local.h"

int32_t websocket_close(const int32_t sock_fd)
{
    var_info("WebSocket close...: ", sock_fd);
    if (sock_fd < 0) {
        return WEBSOCKET_ERRORCODE_NONE;
    }

    if (internal_close(sock_fd) == WEBSOCKET_SYSCALL_ERROR) {
        str_info("WebSocket close error: ", strerror(errno));
        return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
    }

    return WEBSOCKET_ERRORCODE_NONE;
}
