######################################################################
# Automatically generated by qmake (2.01a) Tue Jan 23 16:13:55 2007
######################################################################

CONFIG += uitools qt
TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp

INCPATH     += /usr/local/opisrc/kwt/include
INCPATH     += /usr/local/opi/include
LIBS        += -L/usr/local/opisrc/kwt/lib -lkwt

#EPICS lib
INCPATH     += ./include/ $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux

#QWT
INCPATH		+= /usr/local/qwt/include

target.path = /usr/local/opi/bin
forms.files = ./*.ui
forms.path = /usr/local/opi/ui
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.ui *.pro
sources.path = ./CAMultiplotTest
# install
INSTALLS += target forms

