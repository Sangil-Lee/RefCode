# -------------------------------------------------
# Project created by QtCreator 2010-07-12T21:36:45
# -------------------------------------------------

TARGET = IRTV
TEMPLATE = app
MOC_DIR = ../.moc
OBJECTS_DIR = ../.obj
CONFIG += uitools qt
DEPENDPATH += .

INCLUDEPATH += .
INCLUDEPATH += ./include
INCLUDEPATH += $(EPICS_BASE)/include
INCLUDEPATH += /usr/local/opisrc/include
INCLUDEPATH += /usr/local/opisrc/kwt/include
INCLUDEPATH += $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux
INCLUDEPATH += /usr/local/qwt/include

LIBS        += -L/usr/local/opisrc/kwt/lib -lkwt
LIBS        += -L$(EPICS_BASE)/lib/linux-x86 -lasHost -lcas -ldbStaticHost -lgdd -lrecIoc -lsoftDevIoc -lasIoc

SOURCES     += ../main.cpp ../mainWindow.cpp ../irtvMainWindow.cpp ../irtvDataClient.cpp ../irtvLog.cpp

HEADERS     += ../mainWindow.h ../irtvMainWindow.h
FORMS       += ../ui/DDS_IRTVMain.ui
FORMS       += ../ui/DDS_IRTV_S1.ui
FORMS       += ../ui/DDS_IRTV_D1.ui
RESOURCES   += application.qrc

# install
target.path    = /usr/local/opi/bin
forms.files    = ../ui/*.ui
forms.path     = /usr/local/opi/ui
images.files   = images/*.png images/*.tif
images.path    = /usr/local/opi/images
INSTALLS       = target forms images
