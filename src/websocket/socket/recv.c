#include "../../arch/recv.h"

#include "../websocket_local.h"

#define MSG_DONTWAIT 0x40

ssize_t websocket_recv(const int sock_fd, const size_t capacity, char* restrict buffer)
{
    if (is_rise_signal()) {
        log_info("A signal was raised during recv(). The system will abort processing.\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    ssize_t bytes_read = internal_recvfrom(sock_fd, buffer, capacity - 1, MSG_DONTWAIT, NULL, NULL);
    if (bytes_read == 0) {
        var_info("Socket was disconnected. socket : ", sock_fd);
        return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
    }

    if (bytes_read == WEBSOCKET_SYSCALL_ERROR) {
        if (errno == EINTR || errno == EAGAIN) {
            return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
        }

        str_info("Failed to recv(). reason : ", strerror(errno));
        var_info("socket : ", sock_fd);
        return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
    }

    buffer[bytes_read] = '\0';
    return bytes_read;
}
