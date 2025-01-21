#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/syscall.h>

#include "../../util/log.h"
#include "../../util/signal.h"
#include "../websocket.h"

int websocket_send(const int sock_fd, const char* restrict buffer, const size_t buffer_size)
{
    log_debug("WebSocket server send\n");

    ssize_t rtn = syscall(SYS_sendto, sock_fd, buffer, buffer_size, 0, NULL, 0);
    if (rtn == WEBSOCKET_SYSCALL_ERROR) {
        str_error("Failed to send(). reason : ", strerror(errno));
    }

    return (int)rtn;
}
