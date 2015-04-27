# -------------------------------------------------
# Project created by QtCreator 2012-04-24T11:24:00
# -------------------------------------------------
TARGET = Quench_OPI
TEMPLATE = app
CONFIG += uitools \
    qt \
    debug
INCLUDEPATH += ./include
INCLUDEPATH += .

INCLUDEPATH += /usr/local/opisrc/include

# epics base
INCLUDEPATH += /usr/local/epics/base/include
INCLUDEPATH += /usr/local/epics/base/include/os/Linux

# qwt
INCLUDEPATH += /usr/local/qwt/include
 LIBS += -L/usr/local/qwt/lib \
 -lqwt

# kwt lib
INCLUDEPATH += /usr/local/opisrc/kwt/include
LIBS += -L/usr/local/opisrc/kwt/lib \
    -lkwt
SOURCES += main.cpp \
    mainwindow.cpp \
    panel_tfdispform.cpp \
    panel_tfsetform.cpp \
    panel_cssetform.cpp \
    panel_pfsetform.cpp \
    panel_mainopiform.cpp \
    panel_pfopiform.cpp \
    panel_tfopiform.cpp
HEADERS += mainwindow.h \
    panel_tfdispform.h \
    panel_tfsetform.h \
    panel_cssetform.h \
    panel_pfsetform.h \
    panel_mainopiform.h \
    QuenchDefine.h \
    panel_pfopiform.h \
    panel_tfopiform.h
FORMS += mainwindow.ui \
    panel_maindispform.ui \
    panel_tfdispform.ui \
    panel_csdispform.ui \
    panel_pfdispform.ui \
    panel_tftestform.ui \
    panel_cstestform.ui \
    panel_pftestform.ui \
    panel_tfsetform.ui \
    panel_cssetform.ui \
    panel_pfsetform.ui \
    panel_cssetnewpvform.ui \
    panel_tfdispnewpvform.ui \
    panel_pfsetnewpvform.ui \
    panel_tfsetnewpvform.ui \
    panel_mainopiform.ui \
    panel_pfopiform.ui \
    panel_tfopiform.ui \
    panel_csgrpform.ui \
    panel_pfgrpform.ui \
    panel_tfgrpform.ui
RESOURCES += resources/rc.qrc

ui.files = *.ui
pfbusui.files = ui/pfbus/*.ui
pfcoilui.files = ui/pfcoil/*.ui
pfnewui.files = ui/pfnew/*.ui
tfui.files = ui/tf/*.ui

INSTALLBASE = /usr/local/opi

target.path = $$INSTALLBASE/bin
ui.path = $$INSTALLBASE/ui
pfbusui.path = $$INSTALLBASE/ui/pfbus
pfcoilui.path = $$INSTALLBASE/ui/pfcoil
pfnewui.path = $$INSTALLBASE/ui/pfnew
tfui.path = $$INSTALLBASE/ui/tf

INSTALLS = target \
	   ui\
    	   pfbusui \
	   pfcoilui\
	   pfnewui\
	   tfui
