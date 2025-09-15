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
OS := $(shell uname -s)
ifeq ($(OS), Darwin)
  CC := clang
else
  CC := gcc
endif

#------------------------------------------------------------------------------
# Directories
#------------------------------------------------------------------------------
SRCROOT  := .
OBJROOT  := ./obj
LDLIBS   := -L../../build/lib

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
ifeq ($(OS), Darwin)
CFLAGS := \
	-I../../src \
	-O0 \
	-g \
	-DLOG_LEVEL_DEBUG

LDFLAGS := -lwsserver
else
CFLAGS := \
	-I../../src \
	-O0 \
	-static-libasan \
	-g \
	-fPIC \
	-DLOG_LEVEL_DEBUG

LDFLAGS := -lwsserver
endif

else
ifeq ($(BUILD), release)
ifeq ($(OS), Darwin)
CFLAGS := \
	-I../../src \
	-O3 \
	-flto=auto \
	-mtune=native \
	-ffast-math \
	-fno-math-errno \
	-falign-functions \
	-DLOG_LEVEL_ERROR

LDFLAGS := -lwsserver
else
CFLAGS := \
	-I../../src \
	-O3 \
	-fno-lto \
	-mtune=native \
	-ffast-math \
	-fno-math-errno \
	-falign-functions \
	-fPIC \
	-DLOG_LEVEL_ERROR
endif

LDFLAGS := -lwsserver
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
	rm -rf $(DEPENDFILES)

ifneq "$(MAKECMDGOALS)" "clean"
	-include $(DEPENDFILES)
endif
