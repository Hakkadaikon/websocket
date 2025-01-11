###############################################################################
# File        : Malefile
# Description : makefile.
# Author      : hakkadaikon
###############################################################################

#------------------------------------------------------------------------------
# Build options
#------------------------------------------------------------------------------
.PHONY: build clean run valgrind-run test format

build:
	nix-build

clean:
	nix store gc
	rm result

run: build
	result/bin/ws-server

valgrind-run: build
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes -s result/bin/ws-server

# format (use clang)
format:
	@clang-format -i $(shell find ./src -name '*.[ch]')
