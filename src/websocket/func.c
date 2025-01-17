#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdint.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/syscall.h>

#include "../util/log.h"
#include "../util/signal.h"
#include "websocket.h"

static inline int set_nonblocking(int fd)
{
    long flags = syscall(SYS_fcntl, fd, F_GETFL);
    if (flags == -1) {
        return -1;  // Failed to get flags
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
    if (rtn == -1) {
        char* reason = strerror(errno);
        log_error("Failed to send().\n");
        log_error("reason : ");
        log_error(reason);
        log_error("\n");
    }

    return (int)rtn;
}

ssize_t websocket_recv(const int sock_fd, const size_t capacity, char* restrict buffer)
{
    ssize_t bytes_read = syscall(SYS_recvfrom, sock_fd, buffer, capacity - 1, 0, NULL, NULL);
    if (bytes_read == 0) {
        var_error("Socket was disconnected. socket : ", sock_fd);
        return 0;
    } else if (bytes_read == -1) {
        char* errmsg = strerror(errno);
        log_error("Failed to recv error. reason : ");
        log_error(errmsg);
        log_error("\n");
        var_error("socket : ", sock_fd);
        return 0;
    }

    buffer[bytes_read] = '\0';
    return bytes_read;
}

int websocket_accept(const int sock_fd)
{
    struct sockaddr_in client_addr;
    socklen_t          addr_len = sizeof(client_addr);

    log_debug("accept...\n");
    int client_sock = syscall(SYS_accept, sock_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_sock < 0) {
        if ((errno != EINTR)) {
            log_error("accept() failed. err : ");
            log_error(strerror(errno));
            log_error("\n");
            log_error("The system will abort processing.\n");
            return -2;
        }

        if (is_rise_signal()) {
            log_info("A signal was raised during accept(). The system will abort processing.\n");
            return -2;
        }

        return -1;
    }

    if (set_nonblocking(sock_fd) == -1) {
        log_error("Failed to set client socket non-blocking\n");
        return -2;
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
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(port_num);

    if (syscall(SYS_bind, server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        log_error("Failed to bind.\n");
        syscall(SYS_close, server_sock);
        return -1;
    }

    if (syscall(SYS_listen, server_sock, backlog) < 0) {
        log_error("Failed to listen.\n");
        syscall(SYS_close, server_sock);
        return -1;
    }

    if (set_nonblocking(server_sock) == -1) {
        log_error("Failed to set server socket non-blocking\n");
        return -2;
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

    return server_sock;
}

bool websocket_epoll_add(const int epoll_fd, const int sock_fd, PWebSocketEpollEvent event)
{
    event->data.fd = sock_fd;
    event->events  = EPOLLIN;

    while (1) {
        if (syscall(SYS_epoll_ctl, epoll_fd, EPOLL_CTL_ADD, sock_fd, event) == 0) {
            break;
        }

        if (errno != EINTR) {
            log_error("Failed to add client socket to epoll\n");
            return false;
        }

        if (is_rise_signal()) {
            log_info("A signal was raised during accept(). The system will abort processing.\n");
            return false;
        }
    }

    return true;
}

bool websocket_epoll_del(const int epoll_fd, const int sock_fd)
{
    if (syscall(SYS_epoll_ctl, epoll_fd, EPOLL_CTL_DEL, sock_fd, NULL) == -1) {
        log_error("Failed to del client socket to epoll\n");
        return false;
    }

    return true;
}

int websocket_epoll_create()
{
    int epoll_fd = syscall(SYS_epoll_create1, 0);
    if (epoll_fd == -1) {
        log_error("Failed to create epoll instance\n");
        return -1;
    }

    return epoll_fd;
}

bool websocket_epoll_wait(const int epoll_fd, PWebSocketEpollEvent events, const int max_events)
{
    //int nfds = syscall(SYS_epoll_wait, epoll_fd, events, max_events, -1);
    int nfds = syscall(SYS_epoll_wait, epoll_fd, events, max_events, 0);
    if (nfds < 0) {
        if (errno != EINTR) {
            log_error("Failed to create epoll instance. err : ");
            log_error(strerror(errno));
            log_error("\n");
            log_error("The system will abort processing.\n");
            return -2;
        }
        if (is_rise_signal()) {
            log_info("A signal was raised during accept(). The system will abort processing.\n");
            return -2;
        }

        return -1;
    }

    return nfds;
}
