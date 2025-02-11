#!/bin/bash

cd $(dirname $0)

echo "check link (examples/echoback/bin/wsserver)..."

rm -rf ../build 1>/dev/null
cmake -S .. -B ../build -DCMAKE_BUILD_TYPE=Debug 1>/dev/null
cmake --build ../build 1>/dev/null
make clean -C ../examples/echoback 1>/dev/null
make -C ../examples/echoback 1>/dev/null
ldd ../examples/echoback/bin/wsserver
