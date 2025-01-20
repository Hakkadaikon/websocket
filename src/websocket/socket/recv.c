#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/syscall.h>

#include "../../util/log.h"
#include "../../util/signal.h"
#include "../websocket.h"

typedef struct {
    struct msghdr msg_hdr;
    unsigned int  msg_len;
} WebSocketMmsgHeader, *PWebSocketMmsgHeader;

ssize_t websocket_recvmmsg(const int sock_fd, const size_t capacity, char** restrict buffers, const int num_of_buffer)
{
    const int           iov_capacity = 1;
    WebSocketMmsgHeader headers[num_of_buffer];
    struct iovec        iov[num_of_buffer][iov_capacity];

    memset(&headers, 0x00, sizeof(headers));

    for (int i = 0; i < num_of_buffer; i++) {
        headers[i].msg_hdr.msg_name            = NULL;
        headers[i].msg_hdr.msg_namelen         = 0;
        headers[i].msg_hdr.msg_control         = NULL;
        headers[i].msg_hdr.msg_controllen      = 0;
        headers[i].msg_hdr.msg_iov             = iov[i];
        headers[i].msg_hdr.msg_iovlen          = 1;
        headers[i].msg_hdr.msg_iov[0].iov_len  = capacity;
        headers[i].msg_hdr.msg_iov[0].iov_base = buffers[i];
    }

    ssize_t read_count = syscall(SYS_recvmmsg, sock_fd, &headers, num_of_buffer, MSG_DONTWAIT, NULL);
    if (read_count < 0) {
        if (errno != EINTR) {
            char* errmsg = strerror(errno);
            log_error("Failed to recvmsg error. reason : ");
            log_error(errmsg);
            log_error("\n");
            var_error("socket : ", sock_fd);
            return WEBSOCKET_ERRORCODE_FATAL_ERROR;
        }

        return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
    }

    return read_count;
}

ssize_t websocket_recv(const int sock_fd, const size_t capacity, char* restrict buffer)
{
    if (is_rise_signal()) {
        log_info("A signal was raised during recv(). The system will abort processing.\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    ssize_t bytes_read = syscall(SYS_recvfrom, sock_fd, buffer, capacity - 1, MSG_DONTWAIT, NULL, NULL);
    if (bytes_read == 0) {
        var_error("Socket was disconnected. socket : ", sock_fd);
        return WEBSOCKET_ERRORCODE_SOCKET_CLOSE_ERROR;
    }

    if (bytes_read == WEBSOCKET_SYSCALL_ERROR) {
        if (errno != EINTR) {
            char* errmsg = strerror(errno);
            log_error("Failed to recv error. reason : ");
            log_error(errmsg);
            log_error("\n");
            var_error("socket : ", sock_fd);
            return WEBSOCKET_ERRORCODE_FATAL_ERROR;
        }

        return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
    }

    buffer[bytes_read] = '\0';
    return bytes_read;
}
