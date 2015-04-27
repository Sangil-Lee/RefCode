# -------------------------------------------------
# Project created by QtCreator 2012-07-03T17:07:04
# -------------------------------------------------
TARGET = NB1_TC
TEMPLATE = app
CONFIG += uitools \
    qt

# EPICS Lib
EPICS_DIR = /usr/local/epics/R3.14.12.2/base
INCPATH += $$EPICS_DIR/include \
    $$EPICS_DIR/include/os/Linux

# QWT Lib
INCPATH += /usr/local/qwt-5.0.2/include
LIBS += -L/usr/local/qwt-5.0.2/lib \
    -lqwt

# KWT Lib
KWTDIR = /usr/local/epics/extensions/src/kwt-r826
#KWTDIR = /usr/local/opisrc/kwt
INCPATH += $$KWTDIR/include
#INCPATH += /usr/local/opi/include
LIBS += -L/$$KWTDIR/lib \
    -lkwt
SOURCES += main.cpp \
    mainwindow.cpp
HEADERS += mainwindow.h
FORMS += mainwindow.ui \
    ui/NBI1_Neutral1-1.ui \
    ui/NBI1_Neutral1-2.ui \
    ui/NBI1_Bending1.ui \
    ui/NBI1_IonDump.ui \
    ui/NBI1_Calorimeter.ui \
    ui/NBI1_BLScraper.ui \
    ui/NBI1_BeamDuct.ui \
    ui/NBI1_CryoShield.ui \
    ui/NBI1_CryoBaffle.ui \
    ui/NBI1_TileProtector.ui
RESOURCES += NBI1_Resource.qrc

ui.files = ./ui/*.ui
INSTALLBASE    = /usr/local/opi
target.path    = $$INSTALLBASE/bin
ui.path        = $$INSTALLBASE/ui
INSTALLS       = target ui

