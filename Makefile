###############################################################################
# File        : Malefile
# Description : Template of makefile.
# Author      : hakkadaikon
###############################################################################

#------------------------------------------------------------------------------
# Program name
#------------------------------------------------------------------------------
PROGRAM  := test

#------------------------------------------------------------------------------
# Compiler name
#------------------------------------------------------------------------------
CC       := gcc

#------------------------------------------------------------------------------
# Directories
#------------------------------------------------------------------------------
SRCROOT  := .
OBJROOT  := ./obj
BINROOT  := ./bin
SRCDIRS  := $(shell find $(SRCROOT) -type d)
OBJDIRS  := $(addprefix $(OBJROOT)/, $(SRCDIRS))

#------------------------------------------------------------------------------
# Files
#------------------------------------------------------------------------------
SRCFILES     := $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))
OBJFILES     := $(addprefix $(OBJROOT)/, $(SRCFILES:.c=.o))
DEPENDFILES  := $(OBJFILES:.o=.d)

#------------------------------------------------------------------------------
# Flags
#------------------------------------------------------------------------------
CFLAGS  := -Ofast \
           -march=native \
           -mtune=native \
           -flto \
           -fno-stack-protector \
           -fomit-frame-pointer \
           -fno-asynchronous-unwind-tables \
           -pthread
LDLIBS  := -lssl -lcrypto -lpthread
LDFLAGS := -flto -pthread

#------------------------------------------------------------------------------
# Make rules
#------------------------------------------------------------------------------
$(BINROOT)/$(PROGRAM): $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS) $(LDFLAGS)

$(OBJROOT)/%.o: $(SRCROOT)/%.c
	if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $<

#------------------------------------------------------------------------------
# Build options
#------------------------------------------------------------------------------
.PHONY: all clean format

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

all:
	$(OBJROOT)/$(PROGRAM)

clean:
	rm -f  $(BINROOT)/$(PROGRAM)
	rm -rf $(OBJROOT)/*
	rm -rf $(DEPENDFILES)

ifneq "$(MAKECMDGOALS)" "clean"
	-include $(DEPENDFILES)
endif
