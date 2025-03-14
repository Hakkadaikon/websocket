#include "../../arch/accept.h"

#include "../websocket_local.h"
#include "./optimize_socket.h"

static int32_t get_accept_err(const int32_t client_sock);

int32_t websocket_accept(const int32_t sock_fd)
{
    int32_t            errcode;
    struct sockaddr_in client_addr;
    socklen_t          addr_len = sizeof(client_addr);

    if (is_rise_signal()) {
        log_info("A signal was raised during accept(). The system will abort processing.\n");
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    log_debug("accept...\n");
    int client_sock = internal_accept(sock_fd, (struct sockaddr*)&client_addr, &addr_len, 0);
    if ((errcode = get_accept_err(client_sock)) != WEBSOCKET_ERRORCODE_NONE) {
        return errcode;
    }

    if (optimize_client_socket(client_sock) == WEBSOCKET_ERRORCODE_FATAL_ERROR) {
        websocket_close(client_sock);
        return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
    }

    return client_sock;
}

static int32_t get_accept_err(const int32_t client_sock)
{
    if (client_sock < 0) {
        if (errno == EINTR || errno == EAGAIN) {
            return WEBSOCKET_ERRORCODE_CONTINUABLE_ERROR;
        }

        str_error("accept() failed. reason : ", strerror(errno));
        return WEBSOCKET_ERRORCODE_FATAL_ERROR;
    }

    return WEBSOCKET_ERRORCODE_NONE;
}
