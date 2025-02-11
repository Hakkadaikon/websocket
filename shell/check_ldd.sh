#!/bin/bash

cd $(dirname $0)

ECHOBACK_SERVER_PATH=../examples/echoback
ECHOBACK_SERVER_BIN_PATH=${ECHOBACK_SERVER_PATH}/bin/wsserver

wsDebugBuild() {
  rm -rf ../build 1>/dev/null
  cmake -S .. -B ../build -DCMAKE_BUILD_TYPE=Debug 1>/dev/null
  cmake --build ../build 1>/dev/null
  make clean -C ${ECHOBACK_SERVER_PATH} 1>/dev/null
  BUILD=debug make -C ${ECHOBACK_SERVER_PATH} 1>/dev/null
}

echo "check link (examples/echoback/bin/wsserver)..."
wsDebugBuild
ldd ../examples/echoback/bin/wsserver
