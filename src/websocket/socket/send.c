#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/syscall.h>

#include "../../util/log.h"
#include "../../util/signal.h"
#include "../websocket.h"

#ifdef __APPLE__
#define SYSCALL_SENDTO(fd, buffer, length, flags, dest_addr, dest_len) sendto(fd, buffer, length, flags, dest_addr, dest_len)
#else
#define SYSCALL_SENDTO(fd, buffer, length, flags, dest_addr, dest_len) syscall(SYS_sendto, fd, buffer, length, flags, dest_addr, dest_len)
#endif

int websocket_send(const int sock_fd, const char* restrict buffer, const size_t buffer_size)
{
    log_debug("WebSocket server send\n");

    ssize_t rtn = SYSCALL_SENDTO(sock_fd, buffer, buffer_size, 0, NULL, 0);
    if (rtn == WEBSOCKET_SYSCALL_ERROR) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
        }

        return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
        str_error("Failed to send(). reason : ", strerror(errno));
    }

    return WEBSOCKET_ERRORCODE_NONE;
}
