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

run: build
	result/bin/ws-server

valgrind-run: build
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all --track-origins=yes -s result/bin/ws-server

test:
	cd tests && make clean && make &&  ./bin/test

# format (use clang)
format:
	@clang-format -i $(shell find ./src -name '*.[ch]')

