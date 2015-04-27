TEMPLATE    = lib
CONFIG       += desinger qt thread warn_on uitools

MOC_DIR      = .moc
OBJECTS_DIR  = .obj
INCLUDEPATH += ../../include
DEPENDPATH  += ../../include

#mysql
INCPATH += /usr/include/mysql

#EPICS lib
INCPATH     += ./include/ $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux
LIBS        += -L$(EPICS_BASE)/lib/linux-x86 -lasHost -lcas -ldbStaticHost -lgdd -lrecIoc -lsoftDevIoc -lasIoc -lCom -ldbStaticIoc -lregistryIoc -ltestDevIoc -lca -ldbIoc -ldbtoolsIoc -lmiscIoc -lrsrvIoc

#accesskstardb lib
INCPATH += ../../include/
LIBS += -L/usr/local/opi/lib -laccesskstardb

#mysql++ lib 
INCPATH += /usr/local/include/mysql++
LIBS += -L/usr/local/lib -lmysqlpp


# Input
HEADERS += pvlistviewer.h 
SOURCES += pvlistviewer.cpp 

# install
target.path = /usr/local/opi/lib/
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = /usr/local/opi/lib/libpvlistviewer
headers.files = ./*.h
headers.path = /usr/local/opisrc/include
INSTALLS += target headers
