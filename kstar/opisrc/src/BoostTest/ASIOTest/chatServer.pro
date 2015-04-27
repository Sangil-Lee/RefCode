TEMPLATE     = app
CONFIG       += thread warn_on

MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH  += ./include


TARGET       = chatServer

SOURCES += chatServer.cpp chatMessage.hpp
LFLAGS = -Wformat

LIBS += -L/usr/local/lib -lboost_thread -lboost_iostreams -lboost_serialization -lboost_system

# install
target.path = ./
INSTALLS += target
