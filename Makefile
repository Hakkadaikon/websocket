###############################################################################
# File        : Malefile
# Description : makefile.
# Author      : hakkadaikon
###############################################################################

#------------------------------------------------------------------------------
# Build options
#------------------------------------------------------------------------------
.PHONY:  build debug-build native-build docker-build clean format

build:
	nix-build

debug-build:
	nix-build --arg debug true

native-build:
	mkdir -p native/bin native/obj
	rm -rf native/src native/obj/* native/bin/*
	make -C native

docker-build:
	docker compose build --no-cache

clean:
	nix store gc
	rm result

# format (use clang)
format:
	@clang-format -i $(shell find ./src -name '*.[ch]')
