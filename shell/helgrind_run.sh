#!/bin/bash

cd $(dirname $0)

make debug-build -C ..
valgrind --tool=helgrind --history-level=approx -s ../result/bin/ws-server
