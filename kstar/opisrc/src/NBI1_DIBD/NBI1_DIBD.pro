#-------------------------------------------------
#
# Project created by QtCreator 2012-07-12T21:41:17
#
#-------------------------------------------------

TARGET = NB1_LED
TEMPLATE = app

CONFIG += uitools \
    qt\
    debug

INCPATH += /usr/local/epics/R3.14.12.2/base/include
INCPATH += /usr/local/epics/R3.14.12.2/base/include/os/Linux
LIBS += -L/usr/local/epics/R3.14.12.2/base/lib/linux-x86 \
    -lCom -lca

# QWT Lib
INCPATH += /usr/local/qwt-5.0.2/include
LIBS += -L/usr/local/qwt-5.0.2/lib \
    -lqwt

# KWT Lib
KWTDIR = /usr/local/epics/extensions/src/kwt-r826
INCPATH += $$KWTDIR/include
LIBS += -L/$$KWTDIR/lib \
    -lkwt

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

INSTALLBASE = /usr/local/opi
target.path = $$INSTALLBASE/bin
INSTALLS    = target

