#+======================================================================
# $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-iter-units-api/tags/CODAC-CORE-4.0.0/src/main/resources/newprog/Makefile $
# $Id: Makefile 34157 2013-02-15 15:51:55Z zagara $
#
# Project       : CODAC Core System
#
# Description   : sdn-monitor Makefile
#
# Author        : Anze Zagar, Cosylab
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

PROGNAME=sdn-monitor

LIBRARIES=
LIBRARY_DIRS=$(CODAC_ROOT)/lib
INCLUDE_DIRS=. ../include $(CODAC_ROOT)/include

TARGET=../../../../target
BINARY_DIR=$(TARGET)/bin
OBJECT_DIR=$(TARGET)/obj/$(PROGNAME)
SOURCE_DIR=.

EXECUTABLE=$(BINARY_DIR)/$(PROGNAME)
INCLUDES=$(foreach inc,$(INCLUDE_DIRS),-I$(inc))
LDLIBS=$(foreach libs,$(LIBRARY_DIRS),-L$(libs) -Wl,-rpath,$(libs)) $(foreach libs,$(LIBRARIES),-l$(libs))
# SOURCES=$(SOURCE_DIR)/$(PROGNAME).cpp
# to build executable from all sources in the program directory:
SOURCES=$(wildcard $(SOURCE_DIR)/*.cpp $(SOURCE_DIR)/*.c)
OBJECTS=$(addprefix $(OBJECT_DIR)/,$(patsubst %.cpp,%.o,$(notdir $(SOURCES))))

C=gcc
CC=g++
CFLAGS=-c -Wall
CCFLAGS=-c -Wall
LDFLAGS=-lrt -lpthread

.PHONY: all clean run

all: $(SOURCES) $(EXECUTABLE)

clean:
	rm -rf "$(EXECUTABLE)" "$(OBJECT_DIR)"

run: $(SOURCES) $(EXECUTABLE)
	$(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	mkdir -p $(BINARY_DIR)
	$(CC) $(LDFLAGS) $(LDLIBS) $(OBJECTS) -o $@

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	mkdir -p $(OBJECT_DIR)
	$(CC) $(CCFLAGS) $(INCLUDES) $< -o $@

$(OBJECT_DIR)/%.o: $(SOURCE_DIR)/%.c
	mkdir -p $(OBJECT_DIR)
	$(C) $(CFLAGS) $(INCLUDES) $< -o $@
