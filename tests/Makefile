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
CXX      := g++

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
SRCFILES    := $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))
OBJFILES    := $(addprefix $(OBJROOT)/, $(SRCFILES:.cpp=.o))
DEPENDFILES := $(OBJFILES:.o=.d)

#------------------------------------------------------------------------------
# Flags
#------------------------------------------------------------------------------
CPPFLAGS := -O2
LDLIBS   := 
LDFLAGS  := -lgtest -lgtest_main -lpthread -lstdc++

#------------------------------------------------------------------------------
# Make rules
#------------------------------------------------------------------------------
$(BINROOT)/$(PROGRAM): $(OBJFILES)
	$(CXX) $(CPPFLAGS) -o $@ $^ $(LDLIBS) $(LDFLAGS)

$(OBJROOT)/%.o: $(SRCROOT)/%.cpp
	if [ ! -e `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(CXX) $(CPPFLAGS) $(INCLUDE) -o $@ -c $<

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
			SortIncludes: false,                       \
			DerivePointerAlignment: false,             \
			PointerAlignment: Left,                    \
			AlignOperands: true,                       \
		}"                                             \
		$(shell find . -name '*.cpp' -o -name '*.hpp') \
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
