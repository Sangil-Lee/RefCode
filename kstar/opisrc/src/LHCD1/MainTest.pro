# -------------------------------------------------
# Project created by QtCreator 2012-03-20T09:44:50
# -------------------------------------------------
TARGET = LH1
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
KWTDIR = /usr/local/opisrc/kwt
INCPATH += $$KWTDIR/include
INCPATH += /usr/local/opisrc/include
LIBS += -L$$KWTDIR/lib \
    -lkwt

# EPICSQT Lib
# EPICSQT_DIR = /usr/local/epics/extensions/src/epicsqt/
# INCPATH += $$EPICSQT_DIR/ca_framework/api/include \
# $$EPICSQT_DIR/ca_framework/data/include \
# $$EPICSQT_DIR/ca_framework/plugins/include \
# $$EPICSQT_DIR/ca_framework/widgets/include
# LIBS += -L/opt/qtsdk-2010.02/qt/plugins/designer \
# -lQCaPluginDebug
SOURCES += main.cpp \
    mainwindow.cpp \
#    panel_oper.cpp \
#    panel_up.cpp \
#    panel_interlock.cpp \
#    panel_graph.cpp \
#    panel_ltudaq.cpp \
#    panel_calorimetric.cpp \
    panel_rfcon_klystron.cpp 
#    panel_rfcon_antenna.cpp \
#    panel_powersupply.cpp
HEADERS += mainwindow.h \
#    panel_oper.h \
#    panel_up.h \
#    panel_interlock.h \
#    panel_powersupply.h \
#    panel_graph.h \
#    panel_ltudaq.h \
#    panel_calorimetric.h \
    panel_rfcon_klystron.h 
#    panel_rfcon_antenna.h
FORMS += mainwindow.ui \
    LH1_oper.ui \
    LH1_up.ui \
    LH1_interlock.ui \
    LH1_powersupply.ui \
    LH1_graph.ui \
    LH1_ltudaq.ui \
    LH1_calorimetric.ui \
    LH1_graphAn.ui \
    LH1_monitor.ui \
    LH1_klystron.ui \
    LH1_watercooling.ui \
    LH1_tc.ui \
    LH1_rfcon_klystron.ui \
    LH1_rfcon_antenna.ui \
    LH1_gasfeeding.ui \
    LH1_mdsnode.ui \
    LH1_klyrfgraph.ui \
    LH1_antrfgraph.ui \
    LH1_tlrfgraph.ui \
    LH1_antfwdrfgraph.ui \
    LH1_antrevrfgraph.ui
RESOURCES += resources/resources.qrc
ui.files = *.ui
INSTALLBASE = /usr/local/opi
target.path = $$INSTALLBASE/bin
ui.path = $$INSTALLBASE/ui
INSTALLS = target \
    ui
