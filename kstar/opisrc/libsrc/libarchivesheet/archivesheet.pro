TEMPLATE    = lib
CONFIG       += desinger qt thread warn_on uitools

MOC_DIR      = .moc
OBJECTS_DIR  = .obj
INCLUDEPATH += ../../include /usr/local/include/xmlrpc-c
DEPENDPATH  += ../../include
DESTDIR     = ../../libshared

#KWT LIBS
OPIDIR = /usr/local/opi
INCPATH += $$OPIDIR/../opisrc/include
LIBS += -L$$OPIDIR/lib -lkwt

#QWT LIBS
INCPATH += /usr/local/qwt/include
LIBS += -L/usr/local/qwt/lib -lqwt

#EPICS lib
EPICS_DIR = /usr/local/epics/base
INCPATH     += $$EPICS_DIR/include $$EPICS_DIR/include/os/Linux
#LIBS        += -L$$EPICS_DIR/lib/linux-x86 -lasHost -lcas -ldbStaticHost -lgdd -lrecIoc -lsoftDevIoc -lasIoc -lCom -ldbStaticIoc -liocsh -lregistryIoc -ltestDevIoc -lca -ldbIoc -ldbtoolsIoc -lmiscIoc -lrsrvIoc

#EPICS Extensions
EPICS_EXT_DIR = /usr/local/epics/extensions
INCPATH     += $$EPICS_EXT_DIR/src/ChannelArchiver/Tools/ $$EPICS_EXT_DIR/src/ChannelArchiver/Storage/
LIBS        += -L$$EPICS_EXT_DIR/src/ChannelArchiver/Tools/O.linux-x86 -lTools -L$$EPICS_EXT_DIR/src/ChannelArchiver/Storage/O.linux-x86 -lStorage

# Input
HEADERS += ArchiveSheet.h ArchiveDataClient.h ArchiveDataCommon.h ArchiveThread.h ArchiveThreadsController.h
SOURCES += ArchiveSheet.cpp ArchiveDataClient.cpp DataServer.cpp ArchiveThread.cpp ArchiveThreadsController.cpp
#RESOURCES += archivesheet.qrc

# install
target.path = /usr/local/opi/lib/
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = /usr/local/opi/lib/archivesheet
INSTALLS += target
