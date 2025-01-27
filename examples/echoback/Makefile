###############################################################################
# File        : Malefile
# Description : Template of makefile.
# Author      : hakkadaikon
###############################################################################

#------------------------------------------------------------------------------
# Program name
#------------------------------------------------------------------------------
PROGRAM  := wsserver
BUILD    := release

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

#------------------------------------------------------------------------------
# Files
#------------------------------------------------------------------------------
SRCFILES     := $(shell find $(SRCROOT) -name "*.c")
OBJFILES     := $(addprefix $(OBJROOT)/, $(SRCFILES:.c=.o))
DEPENDFILES  := $(OBJFILES:.o=.d)

#------------------------------------------------------------------------------
# Flags
#------------------------------------------------------------------------------
ifeq ($(BUILD), debug)
CFLAGS := \
	-I../../src \
	-O0 \
	-static-libasan \
	-g \
	-DLOG_LEVEL_DEBUG

LDLIBS  := -L../../lib
LDFLAGS := -flto -lwsserver

else
ifeq ($(BUILD), release)
CFLAGS := \
	-I../../src \
	-O3 \
	-fno-lto \
	-mtune=native \
	-ffast-math \
	-fno-math-errno \
	-falign-functions \
	-flto=auto \
	-DLOG_LEVEL_ERROR

LDLIBS  := -L../../lib
LDFLAGS := -flto -lwsserver
endif
endif

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
.PHONY: all clean

all:
	$(OBJROOT)/$(PROGRAM)

clean:
	rm -f $(BINROOT)/$(PROGRAM)
	rm -rf $(OBJROOT)/*
	rm -rf $(DEPENDFILES)

ifneq "$(MAKECMDGOALS)" "clean"
	-include $(DEPENDFILES)
endif
