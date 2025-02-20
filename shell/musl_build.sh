#!/bin/bash

cd $(dirname $0)
CURRENT_PATH=$(realpath .)

ECHOBACK_SERVER_PATH=../examples/echoback

curl -LO https://musl.cc/x86_64-linux-musl-native.tgz
tar xfz x86_64-linux-musl-native.tgz

rm -rf ../build
make clean -C ${ECHOBACK_SERVER_PATH} 1>/dev/null

export LD=${CURRENT_PATH}/x86_64-linux-musl-native/bin/ld
export LIBRARY_PATH=${CURRENT_PATH}/x86_64-linux-musl-native/lib:$LIBRARY_PATH
export LD_LIBRARY_PATH=${CURRENT_PATH}/x86_64-linux-musl-native/lib:$LD_LIBRARY_PATH

cmake -S .. -B ../build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_C_COMPILER=${CURRENT_PATH}/x86_64-linux-musl-native/bin/gcc \
  -DCMAKE_CXX_COMPILER=${CURRENT_PATH}/x86_64-linux-musl-native/bin/g++
cmake --build ../build

BUILD=release \
CC=${CURRENT_PATH}/x86_64-linux-musl-native/bin/gcc \
CXX=${CURRENT_PATH}/x86_64-linux-musl-native/bin/g++ \
LD=${CURRENT_PATH}/x86_64-linux-musl-native/bin/ld \
make -C ${ECHOBACK_SERVER_PATH} 1>/dev/null

rm -f x86_64-linux-musl-native.tgz
rm -rf x86_64-linux-musl-native/
