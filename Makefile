###############################################################################
# File        : Malefile
# Description : makefile.
# Author      : hakkadaikon
###############################################################################

LIBNAME := libwsserver

#------------------------------------------------------------------------------
# Build options
#------------------------------------------------------------------------------
.PHONY:  build debug-build native-build clean format

build: clean
	nix-build
	cp -p result/lib/$(LIBNAME).a lib/

debug-build: clean
	nix-build --arg debug true
	cp -p result/lib/$(LIBNAME).a lib/

native-build: clean
	make BUILD=release -C native
	cp -p native/lib/$(LIBNAME).a lib/

native-debug-build: clean
	make BUILD=debug -C native
	cp -p native/lib/$(LIBNAME).a lib/

clean:
	rm -f result
	rm -rf lib/
	mkdir -p lib/
	make clean -C native

# format (use clang)
format:
	@clang-format -i $(shell find ./src -name '*.[ch]')
