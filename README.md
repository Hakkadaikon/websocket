# websocket  
![Tests](https://github.com/Hakkadaikon/websocket/actions/workflows/test.yml/badge.svg)  
<a href="https://scan.coverity.com/projects/hakkadaikon-websocket">
  <img alt="Coverity Scan Build Status"
       src="https://scan.coverity.com/projects/31257/badge.svg"/>
</a>

Websocket server that complies with RFC6455.(The WebSocket Protocol)  

![icon](https://github.com/user-attachments/assets/7f02a1de-a4cf-456f-b833-30b2e1c440d1)

# Usage  
## Build  

```shell  
# library build (host/release)
# output : build/lib/
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# library build (host/debug)
# output : build/lib/
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build


# library build (nix/release)
# output : result/lib/
nix build

# library build (nix/debug)
# output : result/lib/
nix build .#debug

# sample build (debug)
# LDLIB : build/lib/
make BUILD=debug -C examples/echoback

# sample build (release)
# LDLIB : build/lib/
make BUILD=release -C examples/echoback

# musl build with example/echoback (release & x86/64 & linux only)
./shell/musl_build.sh
```

## Install  

```shell  
# default install dir:
# - /usr/local/lib/libwsserver.a
# - /usr/local/include/websocket.h
sudo cmake --install build
```

## Format  

```shell  
# Formatting source code (use clang-format)
./format.sh
```

## Test  

```shell  
cd tests
make test
```

## Run  

```shell  
# Example: echoback server
docker compose build --no-cache
docker compose up

# Static analysis (use clang-tidy)
./shell/static_analysis.sh
```

## Examples  
### Echo back  

```c
#include <stddef.h>
#include <websocket.h>

void websocket_receive_callback(
    const int       client_sock,
    PWebSocketFrame frame,
    const size_t    buffer_capacity,
    char*           response_buffer)
{
    switch (frame->opcode) {
        case WEBSOCKET_OP_CODE_TEXT: {
            frame->mask       = 0;
            size_t frame_size = create_websocket_frame(frame, buffer_capacity, response_buffer);
            if (frame_size == 0) {
                log_error("Failed to create websocket frame.\n");
                return;
            }

            websocket_send(client_sock, frame_size, response_buffer);
        } break;
        default:
            break;
    }
}

void websocket_connect_callback(int client_sock)
{
    printf("[user] connect. socket fd : %d\n", client_sock);
    fflush(stdout);
}

void websocket_disconnect_callback(int client_sock)
{
    printf("[user] disconnect. socket fd : %d\n", client_sock);
    fflush(stdout);
}

int main()
{
    WebSocketInitArgs init_args;
    init_args.port_num = 8080;
    init_args.backlog  = 5;

    int server_sock = websocket_server_init(&init_args);
    if (server_sock < WEBSOCKET_ERRORCODE_NONE) {
        log_error("websocket server init error.\n");
        return 1;
    }

    WebSocketLoopArgs loop_args;
    loop_args.server_sock                   = server_sock;
    loop_args.callbacks.receive_callback    = websocket_receive_callback;
    loop_args.callbacks.connect_callback    = websocket_connect_callback;
    loop_args.callbacks.disconnect_callback = websocket_disconnect_callback;
    loop_args.buffer_capacity               = 1024;

    websocket_server_loop(&loop_args);
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
## Develop
[clang-format](https://github.com/llvm/llvm-project/tree/main/clang/tools/clang-format) - Formatter  
[cmake](https://github.com/Kitware/CMake) - Build tool  

## Test  
[googletest](https://github.com/google/googletest) - Google Testing and Mocking Framework  

# Author
Hakkadaikon
