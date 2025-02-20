#!/bin/bash

cd $(dirname $0)

ECHOBACK_SERVER_PATH=../examples/echoback
ECHOBACK_SERVER_BIN_PATH=${ECHOBACK_SERVER_PATH}/bin/wsserver

clean() {
  rm -rf ../build 1>/dev/null
  make clean -C ${ECHOBACK_SERVER_PATH} 1>/dev/null
}

wsDebugBuild() {
  rm -rf ../build 1>/dev/null
  make clean -C ${ECHOBACK_SERVER_PATH} 1>/dev/null
  cmake -S .. -B ../build -DCMAKE_BUILD_TYPE=Debug 1>/dev/null
  cmake --build ../build
  BUILD=debug make -C ${ECHOBACK_SERVER_PATH}
}

wsReleaseBuild() {
  rm -rf ../build 1>/dev/null
  make clean -C ${ECHOBACK_SERVER_PATH} 1>/dev/null
  cmake -S .. -B ../build -DCMAKE_BUILD_TYPE=Release 1>/dev/null
  cmake --build ../build
  BUILD=release make -C ${ECHOBACK_SERVER_PATH}
}

case "$1" in
  memcheck)
    wsDebugBuild
    echo "debug run (memcheck)"
    valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes -s ${ECHOBACK_SERVER_BIN_PATH}
    ;;
  helgrind)
    wsDebugBuild
    echo "debug run (helgrind)"
    valgrind --tool=helgrind --history-level=approx -s ${ECHOBACK_SERVER_BIN_PATH}
    ;;
  debug)
    wsDebugBuild
    echo "debug run"
    #gdb --args ${ECHOBACK_SERVER_BIN_PATH}
    ${ECHOBACK_SERVER_BIN_PATH}
    ;;
  release)
    wsReleaseBuild
    echo "release run"
    ${ECHOBACK_SERVER_BIN_PATH}
    ;;
  clean)
    clean
    ;;
  *)
    wsReleaseBuild
    echo "release run"
    ${ECHOBACK_SERVER_BIN_PATH}
    ;;
esac
