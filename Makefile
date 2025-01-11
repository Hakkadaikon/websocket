###############################################################################
# File        : Malefile
# Description : makefile.
# Author      : hakkadaikon
###############################################################################

#------------------------------------------------------------------------------
# Build options
#------------------------------------------------------------------------------
.PHONY:  build debug-build clean run valgrind-run test format

build:
	nix-build

debug-build:
	nix-build --arg debug true

clean:
	nix store gc
	rm result

run: build
	result/bin/ws-server

memcheck-run: debug-build
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes -s result/bin/ws-server

helgrind-run: debug-build
	valgrind --tool=helgrind --history-level=approx -s result/bin/ws-server

# format (use clang)
format:
	@clang-format -i $(shell find ./src -name '*.[ch]')
