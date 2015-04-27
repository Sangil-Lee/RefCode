#############################################################################
# Makefile for building: libcaLabShared.so
# Template: lib
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = 
CFLAGS        = -pipe -O2 -Wall -W -D_REENTRANT -fPIC $(DEFINES)
CXXFLAGS      = -pipe -O2 -Wall -W -D_REENTRANT -fPIC $(DEFINES)
INCPATH       = -I$(EPICS_BASE)/include -I$(EPICS_BASE)/include/os/Linux -I.moc -I.
LINK          = g++
LFLAGS        = -shared -Wl,-soname,-rpath
LIBS          = $(SUBLIBS) -L$(EPICS_BASE)/lib/linux-x86 -lasHost -lcas -ldbStaticHost -lgdd -lrecIoc -lsoftDevIoc -lasIoc -lCom -ldbStaticIoc -lregistryIoc -ltestDevIoc -lca -ldbIoc -ldbtoolsIoc -lmiscIoc -lrsrvIoc -pthread 
AR            = ar cqs
COPY          = cp -f
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f

####### Output directory
####### Files
SOURCES       = caLab_dll.cpp 
OBJECTS       = caLab_dll.o
DESTDIR       = 
TARGET        = libcaLabShared.so
TARGETA       = libcaLabShared.a

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile  $(TARGET)

$(TARGET):  $(OBJECTS) $(SUBLIBS) $(OBJCOMP)  
	-$(DEL_FILE) $(TARGET)
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS) $(OBJCOMP)

staticlib: $(TARGETA)

$(TARGETA):  $(OBJECTS) $(OBJCOMP) 
	-$(DEL_FILE) $(TARGETA) 
	$(AR) $(TARGETA) $(OBJECTS)

clean: 
	-$(DEL_FILE) $(OBJECTS) $(TARGET)
	-$(DEL_FILE) *~ core *.core

####### Compile
.obj/caLab_dll.o: caLab_dll.cpp extcode.h \
		platdefines.h \
		fundtypes.h \
		caLab.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/caLab_dll.o caLab_dll.cpp
