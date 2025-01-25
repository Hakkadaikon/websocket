#include <sys/syscall.h>

#include "../../util/log.h"
#include "../../util/signal.h"
#include "../websocket.h"

#ifdef __APPLE__
#define SYSCALL_CLOSE(fd) close(sock_fd)
#else
#define SYSCALL_CLOSE(fd) syscall(SYS_close, sock_fd)
#endif

int websocket_close(const int sock_fd)
{
    var_info("WebSocket close...: ", sock_fd);
    return SYSCALL_CLOSE(sock_fd);
}
