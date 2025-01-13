###############################################################################
# File        : Malefile
# Description : makefile.
# Author      : hakkadaikon
###############################################################################

#------------------------------------------------------------------------------
# Build options
#------------------------------------------------------------------------------
.PHONY:  build debug-build clean format

build:
	nix-build

debug-build:
	nix-build --arg debug true

clean:
	nix store gc
	rm result

# format (use clang)
format:
	@clang-format -i $(shell find ./src -name '*.[ch]')
