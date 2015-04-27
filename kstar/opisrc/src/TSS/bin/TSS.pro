######################################################################
# Automatically generated by qmake (2.01a) ? 6? 23 15:20:25 2007
######################################################################

CONFIG += uitools
TEMPLATE = app
# TARGET = TSS
DEPENDPATH += .
INCLUDEPATH += . $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux
LIBS += -L$(EPICS_BASE)/lib/${EPICS_HOST_ARCH} -lCom -lca


#mysql
INCPATH += /usr/include/mysql
 
#mysql++ lib
INCPATH += /usr/local/include/mysql++
LIBS += -L/usr/local/lib -lmysqlpp

#accesskstardb lib 
#INCPATH += ../../../include/
INCPATH += /usr/local/opisrc/include
LIBS += -L/usr/local/opi/lib -laccesskstardb

RCC_DIR      = .rcc
MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH += ./include
INCLUDEPATH += ./include

#XML-RPC
#INCPATH		+= /usr/local/xerces-c-src2_4_0/include
#LIBS        += -L/usr/local/lib -lxerces-c -lxmlrpc -lxmlrpc++ -lxmlrpc_abyss -lxmlrpc_client -lxmlrpc_client++ -lxmlrpc_cpp -lxmlrpc_server -lxmlrpc_server++ -lxmlrpc_server_abyss -lxmlrpc_server_cgi -lxmlrpc_util -lxmlrpc_xmlparse -lxmlrpc_xmltok

#EPICS lib
EPICS_DIR = /usr/local/epics/base
INCPATH     += $$EPICS_DIR/include $$EPICS_DIR/include/os/Linux
#LIBS        += -L$$EPICS_DIR/lib/linux-x86 -lasHost -lcas -ldbStaticHost -lgdd -lrecIoc -lsoftDevIoc -lasIoc -lCom -ldbStaticIoc -lregistryIoc -ltestDevIoc -lca -ldbIoc -ldbtoolsIoc -lmiscIoc -lrsrvIoc


#EPICS Extensions
#EPICS_EXT_DIR = /usr/local/epics/extensions
#INCPATH     += $$EPICS_EXT_DIR/src/ChannelArchiver/Tools/ $$EPICS_EXT_DIR/src/ChannelArchiver/Storage/
#LIBS        += -L$$EPICS_EXT_DIR/src/ChannelArchiver/Tools/O.linux-x86 -lTools -L$$EPICS_EXT_DIR/src/ChannelArchiver/Storage/O.linux-x86 -lStorage

#QWT LIBS
INCPATH += /usr/local/qwt/include
LIBS += -L/usr/local/qwt/lib -lqwt

#KWT LIBS
KWTDIR = /usr/local/opi
INCPATH += $$KWTDIR/include
LIBS += -L$$KWTDIR/lib -lkwt

#Shared lib
INCPATH += /usr/local/opisrc/include
#LIBS += -L../../libshared -larchivesheet -lmultiplot


# Input
HEADERS += \
		epicsFunc.h \
		mainWindow.h \
		page_main.h \
		page_local_R1.h \
		page_local_R2.h \
		page_timingSet.h \
		page_manyTrig.h \		
		page_sqlDB.h \
		pvLists.h \
#		nodeDelegate.h \
		queueThread.h \
		passwordDialog.h \
		StringEditor.h \
		global.h \
		pvST_mng.h
FORMS += ../ui/TSS_qtmainwindow.ui ../ui/TSS_passwdDialog.ui
SOURCES += \
		epicsFunc.cpp \
		main.cpp \
		mainWindow.cpp \
		page_main.cpp \
		page_local_R1.cpp \
		page_local_R2.cpp \
		page_manyTrig.cpp \
		page_sqlDB.cpp \
#		nodeDelegate.cpp \
		queueThread.cpp \
		passwordDialog.cpp \
		StringEditor.cpp \
		page_timingSet.cpp \
		pvST_mng.cpp
RESOURCES += application.qrc

# Output
TARGET = TSS
target.path = /usr/local/opi/bin
forms.files = ../ui/*.ui
forms.path = /usr/local/opi/ui
images.files = ./images/*.png
images.files += ./images/*.xpm
images.path = /usr/local/opi/images

INSTALLS += target forms images 