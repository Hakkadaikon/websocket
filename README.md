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
