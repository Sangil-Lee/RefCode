# -*- mode: sh -*- ###########################
# Kwt Widget Library
# Copyright (C) 1997   Josef Wilgen
# Copyright (C) 2002   Uwe Rathmann
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Kwt License, Version 1.0
##############################################

# pro file for the Kwt designer plugin

TEMPLATE        = lib
MOC_DIR         = moc
OBJECTS_DIR     = obj 
QWTDIR		= /usr/local/qwt
MDSPLUSDIR      = /usr/local/mdsplus
DESTDIR         = plugins/designer
INCLUDEPATH    += ../include  
INCLUDEPATH    += $$QWTDIR/include $$MDSPLUSDIR/include
INCLUDEPATH    += $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux 
#LIBS += -L$$QWTDIR/lib -lqwt

win32::DEFINES += KWT_DLL

unix:LIBS      += -L../lib -lkwt
win32-msvc:LIBS  += ../lib/kwt5.lib
win32-msvc.net:LIBS  += ../lib/kwt5.lib
win32-msvc2005:LIBS += ../lib/kwt5.lib
win32-g++:LIBS   += -L../lib -lkwt5

# isEmpty(QT_VERSION) does not work with Qt-4.1.0/MinGW

VVERSION = $$[QT_VERSION]
isEmpty(VVERSION) {
    # Qt 3 
     TARGET    = kwtplugin
     CONFIG   += qt warn_on thread release plugin

     UI_DIR    = ui

    HEADERS  += kwtplugin.h
    SOURCES  += kwtplugin.cpp

    target.path = $(QTDIR)/plugins/designer
    INSTALLS += target

    IMAGES  += \
         images/kstar.png 

} else {

    # Qt 4

    TARGET    = kwt_designer_plugin
    CONFIG    += designer warn_on thread plugin 
    CONFIG    += release
    #CONFIG    += debug_and_release

    RCC_DIR   = resources

    HEADERS  += kwt_designer_plugin.h
    SOURCES  += kwt_designer_plugin.cpp
    RESOURCES += kwt_designer_plugin.qrc

    target.path = $$[QT_INSTALL_PLUGINS]/designer
    INSTALLS += target

    CONFIG(debug, debug|release) {
        unix: TARGET = $$join(TARGET,,,_debug)
        else: TARGET = $$join(TARGET,,d)
    }
}
