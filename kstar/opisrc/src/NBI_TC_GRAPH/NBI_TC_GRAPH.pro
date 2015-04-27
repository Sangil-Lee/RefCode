#-------------------------------------------------
#
# Project created by QtCreator 2012-09-04T11:27:04
#
#-------------------------------------------------

TARGET = NB1_TC_GRP
TEMPLATE = app
CONFIG += uitools \
    qt

# EPICS Lib
EPICS_DIR = /usr/local/epics/base
INCPATH += $$EPICS_DIR/include \
    $$EPICS_DIR/include/os/Linux

# QWT Lib
INCPATH += /usr/local/qwt/include
LIBS += -L/usr/local/qwt/lib \
    -lqwt

# KWT Lib
KWTDIR = /usr/local/epics/extensions/src/kwt-r826
#KWTDIR = /usr/local/opisrc/kwt
INCPATH += $$KWTDIR/include
#INCPATH += /usr/local/opi/include
LIBS += -L/$$KWTDIR/lib \
    -lkwt

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

ui.files = *.ui
INSTALLBASE    = /usr/local/opi
target.path    = $$INSTALLBASE/bin
ui.path        = $$INSTALLBASE/ui
INSTALLS       = target ui
