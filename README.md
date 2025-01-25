# websocket  
![Tests](https://github.com/Hakkadaikon/websocket/actions/workflows/test.yml/badge.svg)  
<a href="https://scan.coverity.com/projects/hakkadaikon-websocket">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/31257/badge.svg"/>
</a>

Websocket server that complies with RFC6455.(The WebSocket Protocol)  

![icon](https://github.com/user-attachments/assets/f4b92376-17db-4979-897a-b005ed421e22)

# Usage  
## Build  

```shell  
# release build
make build

# debug build
make debug-build

# docker build
make docker-build
docker compose up
```

## Format  

```shell  
# Formatting source code (use clang-format)
make format
```

## Test  

```shell  
cd tests
make test
```

## Run  

```shell  
# Run  
./shell/run.sh

# Debug run (use valgrind --tool memcheck)  
./shell/run.sh memcheck

# Debug run (use valgrind --tool helgrind)  
./shell/run.sh helgrind

# Static analysis (use clang-tidy)
./shell/static_analysis.sh
```

## Sample
### Echo back

```c
// main.c
#include <stdint.h>
#include "util/log.h"
#include "util/signal.h"
#include "websocket/websocket.h"

void websocket_callback(
    const int       client_sock,
    PWebSocketFrame frame,
    const size_t    client_buffer_capacity,
    char*           response_buffer)
{
    switch (frame->opcode) {
        case WEBSOCKET_OP_CODE_TEXT: {
            frame->mask       = 0;
            size_t frame_size = create_websocket_frame(frame, client_buffer_capacity, (uint8_t*)response_buffer);
            if (frame_size == 0) {
                log_error("Failed to create websocket frame.\n");
                return;
            }

            websocket_send(client_sock, response_buffer, frame_size);
        } break;
        default:
            break;
    }
}

int main()
{
    int    websocket_port_num     = 8080;
    int    backlog                = 5;
    size_t client_buffer_capacity = 1024;

    signal_init();

    int server_sock = websocket_server_init(websocket_port_num, backlog);
    if (server_sock < WEBSOCKET_ERRORCODE_NONE) {
        log_error("websocket server init error.\n");
        return 1;
    }

    websocket_server_loop(server_sock, client_buffer_capacity, websocket_callback);
    websocket_close(server_sock);

    log_error("websocket server end.\n");
    return 0;
}
```

# Support  
- opcode  
  - 0x0 (continuation)   : No  
  - 0x1 (text)           : Yes (Interpret with user callbacks)  
  - 0x2 (binary)         : Yes (Interpret with user callbacks)  
  - 0x8 (close)          : Yes  
  - 0x9 (ping)           : No  
  - 0xA (pong)           : Yes (When a ping is received, a pong is sent back.)  
- TLS Support            : No  
- Sub protocol           : No (Sec-WebSocket-Protocol)  
- Extensions             : No (Sec-WebSocket-Extensions)  
- Compression / Decode   : No  

# Dependencies  
## Build  
[NixOS/nix](https://github.com/NixOS/nix) - Package manager / OS  

## develop  
[DeterminateSystems/nix-installer](https://github.com/DeterminateSystems/nix-installer) - Nix installer  
[clang-format](https://github.com/llvm/llvm-project/tree/main/clang/tools/clang-format) - Formatter  

## Test  
[googletest](https://github.com/google/googletest) - Google Testing and Mocking Framework  

## Performance measurement  
[valgrind](https://sourceware.org/git/valgrind.git) - building analysis tools  
[iperf3](https://github.com/esnet/iperf) - A TCP, UDP, and SCTP network bandwidth measurement tool  

# Author
Hakkadaikon
