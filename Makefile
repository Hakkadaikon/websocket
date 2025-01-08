###############################################################################
# File        : Malefile
# Description : makefile.
# Author      : hakkadaikon
###############################################################################

#------------------------------------------------------------------------------
# Build options
#------------------------------------------------------------------------------
.PHONY: build clean run format

build:
	nix-build

clean:
	nix store gc

run: build
	result/bin/ws-server

# format (use clang)
format:
	@clang-format -i \
		-style="{    \
			BasedOnStyle: Google,                      \
			AlignConsecutiveAssignments: true,         \
			AlignConsecutiveDeclarations: true,        \
			ColumnLimit: 0,                            \
			IndentWidth: 4,                            \
			AllowShortFunctionsOnASingleLine: None,    \
			AllowShortLoopsOnASingleLine: false,       \
			BreakBeforeBraces: Linux,                  \
			SortIncludes: true,                        \
			DerivePointerAlignment: false,             \
			PointerAlignment: Left,                    \
			AlignOperands: true,                       \
		}"                                             \
		$(shell find . -name '*.c' -o -name '*.h')

