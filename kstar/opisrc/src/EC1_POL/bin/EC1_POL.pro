#
#
#
#

CONFIG      += uitools qt
TEMPLATE     = app
DEPENDPATH  += ..
INCLUDEPATH += ..

RCC_DIR      = .rcc
MOC_DIR      = .moc
OBJECTS_DIR  = .obj

#KWT
KWTDIR		= /usr/local/opisrc/kwt
INCPATH		+= $$KWTDIR/include
LIBS		+= -L$$KWTDIR/lib -lkwt

# for cachedchannelaccess.h
INCPATH		+= /usr/local/opisrc/include

#EPICS lib
INCPATH		+= $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux
LIBS		+= -L$(EPICS_BASE)/lib/${EPICS_HOST_ARCH} -lCom -lca

#QWT
INCPATH		+= $(QWT_HOME)/include
LIBS		+= -L$(QWT_HOME)/lib -lqwt

# Input
HEADERS		+= mainWindow.h
SOURCES		+= main.cpp mainWindow.cpp
FORMS		+= ../ui/EC1_POL_Main.ui
RESOURCES	+= application.qrc

# Output
TARGET = EC1_POL
target.path = /usr/local/opi/bin
forms.files = ../ui/EC1_POL_Polarization.ui ../ui/EC1_POL_TC.ui
forms.path = /usr/local/opi/ui
#images.files = ../images/*.png
images.path = /usr/local/opi/images
sources.files = $$SOURCES $$HEADERS $$FORMS *.pro
sources.path = ./

# install
INSTALLS += target forms images

