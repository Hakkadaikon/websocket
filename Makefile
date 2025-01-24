###############################################################################
# File        : Malefile
# Description : makefile.
# Author      : hakkadaikon
###############################################################################

#------------------------------------------------------------------------------
# Build options
#------------------------------------------------------------------------------
.PHONY:  build native-build debug-build clean format

build:
	nix-build

native-build:
	mkdir -p native/bin native/obj
	rm -rf native/src native/obj/* native/bin/*
	make -C native

debug-build:
	nix-build --arg debug true

clean:
	nix store gc
	rm result

# format (use clang)
format:
	@clang-format -i $(shell find ./src -name '*.[ch]')
