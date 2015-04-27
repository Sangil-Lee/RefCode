####### Compiler, tools and options
CC            = gcc
CXX           = g++
CFLAGS        = -pipe -O2 -Wall -W -D_REENTRANT
CXXFLAGS      = -pipe -O2 -Wall -W -D_REENTRANT
INCPATH       = 
LINK          = g++
LFLAGS        = -L/usr/local/lib
LIBS          = -lboost_thread -lboost_iostreams -lboost_serialization -lboost_system
DEL_FILE      = rm -f

####### Files
#SOURCES       = shared_ptr_example2_test.cpp shared_ptr_example2.cpp shared_ptr_example2.hpp 
SOURCES       = shared_ptr_example2_test.cpp
OBJECTS       = shared_ptr_example2.o 
TARGET        = shardPtrTest2

####### Implicit rules
.SUFFIXES: .o .c .cpp

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules
$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(SOURCES) $(OBJCOMP) $(LIBS)
	$(DEL_FILE) $(OBJECTS)

clean: 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core

####### Compile
$(OBJECTS): shared_ptr_example2.cpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $(OBJECTS) shared_ptr_example2.cpp
