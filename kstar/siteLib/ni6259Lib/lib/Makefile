#+======================================================================
# $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-kmod-pxi6259/branches/codac-core-4.1/src/main/c/lib/Makefile $
# $Id: Makefile 34216 2013-02-20 09:00:23Z cesnikt $
#
# Project       : CODAC Core System
#
# Description   : Makefile
#
# Author        : Cosylab
#
# Copyright (c) : 2010-2013 ITER Organization,
#                 CS 90 046
#                 13067 St. Paul-lez-Durance Cedex
#                 France
#
# This file is part of ITER CODAC software.
# For the terms and conditions of redistribution or use of this software
# refer to the file ITER-LICENSE.TXT located in the top level directory
# of the distribution package.
#
#-======================================================================

# shared liblary name
NAME := pxi6259

# variables needed for packaging
LIB_SPEC_NAME := $(NAME)-kmod-lib.spec
COMMON_SPEC_NAME := $(NAME)-kmod-common.spec

# lib variables
INCLUDE := .
LIB_HDR := pxi-6259-lib.h pxi-6259-lib-utils.h

# output destinations
TARGET := lib$(NAME).so
OBJDIR := obj

# compiler/linker flags
# TODO remove gdb flags for now using no deamon
# because this will deatach also our process
# we will run it in sepcial thread...   
# debug flag: -D PXI6259_DEBUG
CCFLAGS := -fPIC -Wall -g -ggdb -O3 #-D PXI6259_DEBUG
													
LDFLAGS := -Wall -O3 -l pthread

# files
OBJS := pxi-6259-lib \
		pxi-6259-lib-utils
	
INCLUDES := $(INCLUDE) ../include

LIBS :=
ALIBS :=

###############################################################################

# rules

all: $(TARGET)

clean:
	@rm -rf *~ $(TARGET) $(OBJDIR)

$(TARGET): $(OBJS:%=$(OBJDIR)/%.o) 
	gcc -shared $(LDFLAGS) $(LIBS:%=-l%) -o $@ $^ $(ALIBS)

$(OBJDIR)/%.o: %.c $(LIB_HDR)
	@mkdir -p $(OBJDIR)
	gcc -c $(CCFLAGS) $(INCLUDES:%=-I%) -o $@ $<
	
install:
	make all
	cp $(TARGET) ../../lib/$(EPICS_HOST_ARCH)
	cp ../include/pxi6259-enums.h ../include/pxi6259.h pxi-6259-lib.h ../../include

