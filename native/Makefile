###############################################################################
# File        : Malefile
# Description : Template of makefile.
# Author      : hakkadaikon
###############################################################################

#------------------------------------------------------------------------------
# Program name
#------------------------------------------------------------------------------
PROGRAM  := libwsserver
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
LIBROOT  := ./lib

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
	-O0 \
	-static-libasan \
	-g \
	-DLOG_LEVEL_DEBUG

LDLIBS  :=
LDFLAGS := -flto

else
ifeq ($(BUILD), release)
CFLAGS := \
	-O3 \
	-fno-lto \
	-mtune=native \
	-ffast-math \
	-fno-math-errno \
	-falign-functions \
	-flto=auto \
	-DLOG_LEVEL_ERROR

LDLIBS  :=
LDFLAGS := -flto
endif
endif

#------------------------------------------------------------------------------
# Make rules
#------------------------------------------------------------------------------
$(LIBROOT)/$(PROGRAM): $(OBJFILES)
	ar rcs $@.a $^

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
	rm -rf $(LIBROOT)
	rm -rf $(OBJROOT)
	rm -rf ./src
	cp -rp ../src $(SRCROOT)
	mkdir -p $(LIBROOT) $(OBJROOT)
	rm -rf $(DEPENDFILES)

ifneq "$(MAKECMDGOALS)" "clean"
	-include $(DEPENDFILES)
endif
