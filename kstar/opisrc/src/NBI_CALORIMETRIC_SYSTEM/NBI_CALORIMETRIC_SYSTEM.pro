# -------------------------------------------------
# Project created by QtCreator 2012-07-26T11:17:34
# -------------------------------------------------
TARGET = NB1_CAL
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
#KWTDIR = /usr/local/epics/extensions/src/kwt-r826
KWTDIR = /usr/local/opisrc/kwt
INCPATH += $$KWTDIR/include
INCPATH += /usr/local/opisrc/include
LIBS += -L/$$KWTDIR/lib \
    -lkwt
SOURCES += main.cpp \
    mainwindow.cpp \
    panel_calorimetric.cpp
HEADERS += mainwindow.h \
    panel_calorimetric.h
FORMS += mainwindow.ui \
    panel_calorimetric.ui
RESOURCES += resources/resources.qrc

ui.files = *.ui
INSTALLBASE    = /usr/local/opi
target.path    = $$INSTALLBASE/bin
ui.path        = $$INSTALLBASE/ui
INSTALLS       = target ui
