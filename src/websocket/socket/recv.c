#include "../../arch/recv.h"

#include "../websocket_local.h"

#ifndef __APPLE__
#define MSG_DONTWAIT 0x40
#endif

static int32_t get_recv_err(const ssize_t bytes_read, const int32_t sock_fd);

ssize_t websocket_recv(const int32_t sock_fd, const size_t capacity, char* restrict buffer)
{
    if (is_rise_signal()) {
        log_info("A signal was raised during recv(). The system will abort processing.\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    int32_t errcode;
    ssize_t bytes_read = internal_recvfrom(sock_fd, buffer, capacity - 1, MSG_DONTWAIT, NULL, NULL);
    if ((errcode = get_recv_err(bytes_read, sock_fd)) != WEBSOCKET_ERRORCODE_NONE) {
        return errcode;
    }

    buffer[bytes_read] = '\0';
    return bytes_read;
}

static int32_t get_recv_err(const ssize_t bytes_read, const int32_t sock_fd)
{
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

    return WEBSOCKET_ERRORCODE_NONE;
}
