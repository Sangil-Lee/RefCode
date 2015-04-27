TEMPLATE = app

CONFIG += qt warn_on uitools
unix:contains(QT_CONFIG, qdbus):CONFIG += qdbus

DEPENDPATH  += ./include

#EPICS lib
EPICS_BASE = /usr/local/epics/base
INCPATH     += ./include/ $$EPICS_BASE/include $$EPICS_BASE/include/os/Linux
LIBS        += -L$$EPICS_BASE/lib/linux-x86 -lasHost -lcas -ldbStaticHost -lgdd -lrecIoc -lsoftDevIoc -lasIoc -lCom -ldbStaticIoc -liocsh -lregistryIoc -ltestDevIoc -lca -ldbIoc -ldbtoolsIoc -lmiscIoc -lrsrvIoc

#EPICS Extensions
EPICS_EXT = /usr/local/epics/extensions
INCPATH     += $$EPICS_EXT/include/ $$EPICS_EXT/src/ChannelArchiver/Tools/ $$EPICS_EXT/src/ChannelArchiver/Storage/
LIBS        += -L$$EPICS_EXT/src/ChannelArchiver/Tools/O.linux-x86 -lTools -L$$EPICS_EXT/src/ChannelArchiver/Storage/O.linux-x86 -lStorage

#XML-RPC
INCPATH		+= /usr/local/xerces-c-src2_4_0/include
LIBS        += -L/usr/local/lib -lxerces-c -lxmlrpc -lxmlrpc++ -lxmlrpc_abyss -lxmlrpc_client -lxmlrpc_client++ -lxmlrpc_cpp -lxmlrpc_server -lxmlrpc_server++ -lxmlrpc_server_abyss -lxmlrpc_server_cgi -lxmlrpc_util -lxmlrpc_xmlparse -lxmlrpc_xmltok

HEADERS += ArchiveSheet.h ArchiveDataClient.h ArchiveDataCommon.h ArchiveThread.h ArchiveThreadsController.h
SOURCES += main.cpp ArchiveSheet.cpp ArchiveDataClient.cpp DataServer.cpp ArchiveThread.cpp ArchiveThreadsController.cpp
RESOURCES += archivesheet.qrc

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

# install
target.path = /usr/local/opi/bin
sources.files = $$SOURCES $$RESOURCES *.pro images
sources.path = ./Archivesheet
INSTALLS += target
