#include <sys/syscall.h>

#include "../../util/log.h"
#include "../../util/signal.h"
#include "../websocket.h"

int websocket_close(const int sock_fd)
{
    var_info("WebSocket close...: ", sock_fd);
    return syscall(SYS_close, sock_fd);
}
