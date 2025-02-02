#include <sys/syscall.h>
#include <unistd.h>

#include "../websocket_local.h"

int websocket_close(const int sock_fd)
{
    var_info("WebSocket close...: ", sock_fd);
    if (sock_fd < 0) {
        return WEBSOCKET_ERRORCODE_NONE;
    }

    if (close(sock_fd) == WEBSOCKET_SYSCALL_ERROR) {
        str_info("WebSocket close error: ", strerror(errno));
        return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
    }

    return WEBSOCKET_ERRORCODE_NONE;
}
