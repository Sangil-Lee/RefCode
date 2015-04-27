TEMPLATE     = app
CONFIG       += qt thread warn_on uitools

MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH  += ../../include

#EPICS lib
INCPATH     += ./include/ $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux
LIBS        += -L$(EPICS_BASE)/lib/linux-x86 -lasHost -lcas -ldbStaticHost -lgdd -lrecIoc -lsoftDevIoc -lasIoc -lCom -ldbStaticIoc -liocsh -lregistryIoc -ltestDevIoc -lca -ldbIoc -ldbtoolsIoc -lmiscIoc -lrsrvIoc

TARGET       = DBConncheck

SOURCES += main.cpp

# install
target.path = /usr/local/opi/bin
sources.path = ./DBConncheck
INSTALLS += target
