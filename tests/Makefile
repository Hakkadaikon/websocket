###############################################################################
# File        : Malefile
# Description : 
# Author      : hakkadaikon
###############################################################################

#------------------------------------------------------------------------------
# Build options
#------------------------------------------------------------------------------
.PHONY: test clean format

clean:
	rm -rf build

# format (use clang)
format:
	@clang-format -i $(shell find . -name '*.[ch]pp' -o -name '*.[ch]')

test: clean
	cmake -S . -B build && cmake --build build && cd build && ctest
