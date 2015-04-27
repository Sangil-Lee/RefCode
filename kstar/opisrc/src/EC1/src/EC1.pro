#
#
#
#

CONFIG      += uitools qt
TEMPLATE     = app
TARGET       = NFRI
DEPENDPATH  += ..
INCLUDEPATH += ..

RCC_DIR      = .rcc
MOC_DIR      = .moc
OBJECTS_DIR  = .obj

# Input
HEADERS   += *.h
SOURCES   += *.cpp 
FORMS     += ../ui/*.ui
RESOURCES += 

KWTDIR   = /usr/local/opisrc/kwt
#KWTDIR   = /usr/local/opi
INCPATH += $$KWTDIR/include /usr/local/opi/include
#LIBS    += -L$$KWTDIR/lib -lkwt
LIBS    += -L/usr/local/opi/lib -lkwt

#EPICS lib
INCPATH  += $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux

#QWT
INCPATH  += $(QWT_HOME)/include

# Output
TARGET = ../bin/EC1
target.path = /usr/local/opi/bin
forms.files = ../ui/*.ui
forms.path = /usr/local/opi/ui
images.files = ../images/*.png ../images/*.PNG ../images/*.gif
images.path = /usr/local/opi/images
sources.files = $$SOURCES $$HEADERS $$FORMS *.pro
sources.path = ./

# install
INSTALLS += target forms images

