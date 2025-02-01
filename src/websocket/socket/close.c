#include <sys/syscall.h>
#include <unistd.h>

#include "../websocket_local.h"

int websocket_close(const int sock_fd)
{
    var_info("WebSocket close...: ", sock_fd);
    return close(sock_fd);
}
