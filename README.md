# websocket  
RFC6455 (The WebSocket Protocol) protocol stack.  

# Usage  
## Build  

```shell  
make build
```

## Format  

```shell  
make format
```

## Run  

```shell  
# ./result/bin/ws-server
make run
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
[openssl/openssl](https://github.com/openssl/openssl) - SSL/TLS and crypto library  
[NixOS/nix](https://github.com/NixOS/nix) - Package manager / OS  
[DeterminateSystems/nix-installer](https://github.com/DeterminateSystems/nix-installer) - Nix installer  
[clang-format](https://github.com/llvm/llvm-project/tree/main/clang/tools/clang-format) - Formatter  

# Author
Hakkadaikon
