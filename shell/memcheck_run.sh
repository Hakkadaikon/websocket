#!/bin/bash

cd $(dirname $0)

make debug-build -C ..
valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes -s ../result/bin/ws-server
