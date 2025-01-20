#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdint.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "../util/log.h"
#include "../util/signal.h"
#include "websocket.h"

typedef struct {
    struct msghdr msg_hdr;
    unsigned int  msg_len;
} WebSocketMmsgHeader, *PWebSocketMmsgHeader;

static inline int set_nonblocking(int fd)
{
    long flags = syscall(SYS_fcntl, fd, F_GETFL);
    if (flags == WEBSOCKET_SYSCALL_ERROR) {
        return WEBSOCKET_SYSCALL_ERROR;  // Failed to get flags
    }

    return syscall(SYS_fcntl, fd, F_SETFL, flags | O_NONBLOCK);
}

int websocket_close(const int sock_fd)
{
    var_info("WebSocket close...: ", sock_fd);
    return syscall(SYS_close, sock_fd);
}

int websocket_send(const int sock_fd, const char* restrict buffer, const size_t buffer_size)
{
    log_debug("WebSocket server send\n");

    ssize_t rtn = syscall(SYS_sendto, sock_fd, buffer, buffer_size, 0, NULL, 0);
    if (rtn == WEBSOCKET_SYSCALL_ERROR) {
        char* reason = strerror(errno);
        log_error("Failed to send().\n");
        log_error("reason : ");
        log_error(reason);
        log_error("\n");
    }

    return (int)rtn;
}

ssize_t websocket_recvmmsg(const int sock_fd, const size_t capacity, char** restrict buffers, const int num_of_buffer)
{
    const int           iov_capacity = 1;
    WebSocketMmsgHeader headers[num_of_buffer];
    struct iovec        iov[iov_capacity][num_of_buffer];

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

int websocket_accept(const int sock_fd)
{
    struct sockaddr_in client_addr;
    socklen_t          addr_len = sizeof(client_addr);

    if (is_rise_signal()) {
        log_info("A signal was raised during accept(). The system will abort processing.\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    log_debug("accept...\n");
    int client_sock = syscall(SYS_accept, sock_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_sock < 0) {
        if ((errno != EINTR)) {
            log_error("accept() failed. err : ");
            log_error(strerror(errno));
            log_error("\n");
            log_error("The system will abort processing.\n");
            return WEBSOCKET_ERRORCODE_FATAL_ERROR;
        }

        return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
    }

    if (set_nonblocking(sock_fd) == WEBSOCKET_SYSCALL_ERROR) {
        log_error("Failed to set client socket non-blocking\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    // Optimize socket option
    int flag = 1;
    syscall(SYS_setsockopt, client_sock, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    syscall(SYS_setsockopt, client_sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
#if defined(SO_REUSEPORT)
    syscall(SYS_setsockopt, client_sock, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag));
#endif

    return client_sock;
}

int websocket_connect(const int port_num, const int backlog)
{
    int                server_sock;
    struct sockaddr_in server_addr;

    server_sock = syscall(SYS_socket, AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        log_error("Failed to create socket.\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(port_num);

    bool err = false;

    if (syscall(SYS_bind, server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        log_error("Failed to bind.\n");
        err = true;
        goto FINALIZE;
    }

    if (syscall(SYS_listen, server_sock, backlog) < 0) {
        log_error("Failed to listen.\n");
        err = true;
        goto FINALIZE;
    }

    if (set_nonblocking(server_sock) == WEBSOCKET_SYSCALL_ERROR) {
        log_error("Failed to set server socket non-blocking\n");
        err = true;
        goto FINALIZE;
    }

    // Optimize socket option
    int flag = 1;
    syscall(SYS_setsockopt, server_sock, IPPROTO_TCP, TCP_NODELAY, &flag, sizeof(flag));
    syscall(SYS_setsockopt, server_sock, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
#if defined(SO_REUSEPORT)
    syscall(SYS_setsockopt, server_sock, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag));
#endif
    int qlen = 5;
    syscall(SYS_setsockopt, server_sock, IPPROTO_TCP, TCP_FASTOPEN, &qlen, sizeof(qlen));

FINALIZE:
    if (err) {
        syscall(SYS_close, server_sock);
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    return server_sock;
}

bool websocket_epoll_add(const int epoll_fd, const int sock_fd, PWebSocketEpollEvent event)
{
    event->data.fd = sock_fd;
    event->events  = EPOLLIN | EPOLLHUP | EPOLLERR | EPOLLRDHUP | EPOLLET;

    while (1) {
        if (is_rise_signal()) {
            log_info("A signal was raised during epoll_wait(). The system will abort processing.\n");
            return false;
        }

        if (syscall(SYS_epoll_ctl, epoll_fd, EPOLL_CTL_ADD, sock_fd, event) == 0) {
            break;
        }

        if (errno != EINTR) {
            log_error("Failed to add client socket to epoll\n");
            return false;
        }
    }

    return true;
}

bool websocket_epoll_del(const int epoll_fd, const int sock_fd)
{
    if (syscall(SYS_epoll_ctl, epoll_fd, EPOLL_CTL_DEL, sock_fd, NULL) == WEBSOCKET_SYSCALL_ERROR) {
        log_error("Failed to del client socket to epoll\n");
        return false;
    }

    return true;
}

int websocket_epoll_create()
{
    int epoll_fd = syscall(SYS_epoll_create1, 0);
    if (epoll_fd == WEBSOCKET_SYSCALL_ERROR) {
        log_error("Failed to create epoll instance\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    return epoll_fd;
}

int websocket_epoll_wait(const int epoll_fd, PWebSocketEpollEvent events, const int max_events)
{
    if (is_rise_signal()) {
        log_info("A signal was raised during epoll_wait(). The system will abort processing.\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    //int nfds = syscall(SYS_epoll_wait, epoll_fd, events, max_events, -1);  // blocking
    int nfds = syscall(SYS_epoll_wait, epoll_fd, events, max_events, 0);  // non blocking
    if (nfds < 0) {
        if (errno != EINTR) {
            char* errmsg = strerror(errno);
            log_debug("Failed to create epoll instance. err : ");
            log_debug(errmsg);
            log_debug("\n");
            log_debug("The system will abort processing.\n");
            return WEBSOCKET_ERRORCODE_FATAL_ERROR;
        }

        return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
    }

    return nfds;
}
