#!/bin/bash

cd $(dirname $0)

case "$1" in
    memcheck)
        make debug-build -C ..
        valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes -s ../result/bin/ws-server
        ;;
    helgrind)
        make debug-build -C ..
        valgrind --tool=helgrind --history-level=approx -s ../result/bin/ws-server
        ;;
    *)
        cd .. && nix-build
        result/bin/ws-server
        ;;
esac
