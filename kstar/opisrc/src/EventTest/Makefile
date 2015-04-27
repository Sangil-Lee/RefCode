####### Compiler, tools and options

CC            = gcc
CXX           = g++
CFLAGS        = -pipe -O2 -Wall -W -D_REENTRANT
CXXFLAGS      = -pipe -O2 -Wall -W -D_REENTRANT
INCPATH       = 
LINK          = g++
LFLAGS        = -Wl,-rpath,
LIBS          = $(SUBLIBS)  -L/usr/local/lib -levent -lpthread
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -sf
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = .obj/

####### Files

#SOURCES       = eventLoop2.c
SOURCES       = eventLoop2.cpp
OBJECTS       = eventLoop2.o
DESTDIR       = 
TARGET        = eventLoop2

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

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

clean: 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

####### Compile

.obj/eventLoop2.o: eventLoop2.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o .obj/eventLoop2.o eventLoop2.cpp

####### Install

