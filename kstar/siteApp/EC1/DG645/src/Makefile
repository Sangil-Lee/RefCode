# Example EPICS Makefile

# If you don't modify this file it will create
# a program with the name of the current directory
# from all C and C++ source files found and link
# it to the EPICS client libraries.

# Where is EPICS base?
EPICS = /usr/local/epics/base

# Where to install the program)?
BINDIR = ../bin
#BINDIR = bin/$(EPICS_HOST_ARCH)

# What is the name of the program?
# Add one line for each program if the program name
# is not equal to the directory name
PROGRAM +=DG645

# List all sources of the program if not simply
# all *.c *.cc *.C *.cxx *.cpp files in this
# directory should be used.
# Add one line for each source file.
# If you build more than one PROGRAM, list
# the sources separately for each program like
# SRCS_<program> += <filename>
SRCS +=

# list all include directories
INCDIRS += $(EPICS)/include/os/Linux
INCDIRS += $(EPICS)/include

# list all library directories
LIBDIRS += $(EPICS)/lib/$(EPICS_HOST_ARCH)

# list all libraries (ca and Com are EPICS)
LIBS += ca Com

#optimize:
CFLAGS += -O3
#debug:
CFLAGS += -g

# don't touch the code below this line unless you know what you're doing.
CPPFLAGS += $(INCDIRS:%=-I %)

CFLAGS += -MMD
CFLAGS += -Wall 
CFLAGS += $(USR_CFLAGS)

LDFLAGS += $(LIBDIRS:%=-L %)
LDFLAGS += $(LIBDIRS:%=-Wl,-rpath,%)
LDFLAGS += $(LIBS:%=-l %)

ifeq ($(words $(PROGRAM)),0)
PROGRAM = $(notdir $(PWD))
endif

SRCS += $(SRCS_$(PROGRAM))
ifeq ($(words $(SRCS)),0)
SRCS += $(wildcard *.c)
SRCS += $(wildcard *.cc)
SRCS += $(wildcard *.C)
SRCS += $(wildcard *.cxx)
SRCS += $(wildcard *.cpp)
endif

OBJS = $(addprefix O.$(EPICS_HOST_ARCH)/,$(addsuffix .o,$(basename $(SRCS))))

ifndef EPICS_HOST_ARCH
$(error EPICS_HOST_ARCH variable is missing on your system!)
endif

.PHONY:
.PHONY: build clean realclean
build:

clean:
	rm -rf O.*

realclean: clean
	rm -f $(foreach prog,$(PROGRAM),$(BINDIR)/$(prog))

O.%:
	mkdir $@

$(BINDIR):
	mkdir -p $@

ifeq ($(words $(PROGRAM)),1)

build: $(BINDIR)/$(PROGRAM)

ifneq ($(BINDIR),.)
$(PROGRAM): $(BINDIR)/$(PROGRAM)
endif

$(BINDIR)/$(PROGRAM): $(BINDIR) O.$(EPICS_HOST_ARCH) O.$(EPICS_HOST_ARCH)/$(PROGRAM)
	rm -f $@
	cp O.$(EPICS_HOST_ARCH)/$(@F) $@

O.$(EPICS_HOST_ARCH)/$(PROGRAM): $(OBJS)
	$(CXX) -o $@ $(LDFLAGS) $^

else

build:
	for prog in $(PROGRAM); do make PROGRAM=$$prog; done

$(PROGRAM):
	make PROGRAM=$@

endif

O.$(EPICS_HOST_ARCH)/%.o: %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

O.$(EPICS_HOST_ARCH)/%.o: %.cc
	$(CXX) -c $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) $< -o $@

O.$(EPICS_HOST_ARCH)/%.o: %.C
	$(CXX) -c $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) $< -o $@

O.$(EPICS_HOST_ARCH)/%.o: %.cxx
	$(CXX) -c $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) $< -o $@

O.$(EPICS_HOST_ARCH)/%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CFLAGS) $(CXXFLAGS) $< -o $@

-include O.$(EPICS_HOST_ARCH)/*.d
