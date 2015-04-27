######################################################################
# Project file to make Makefile for KWT example
# Written by Sulhee Baek <shbaek@nfri.re.kr> (20 Jan 2010)
######################################################################

QT += uitools widgets core
TEMPLATE	= app
TARGET 		= UIViewer
KWTINC     = /usr/local/kstar/portedkwt/include

# Input
RESOURCES	+= ../bin/application.qrc
SOURCES		+= ../main.cpp ../MainWindow.cpp
HEADERS		+= ../MainWindow.h
HEADERS		+= $$KWTINC/qtchaccesslib.h 

#HEADERS		+= /usr/local/kstar/portedkwt/include/qtchaccesslib.h 


KWTDIR 		= /usr/local/opi
INCPATH     += /usr/local/kstar/portedkwt/include /usr/local/kstar/opisrc/include
LIBS        += -L$$KWTDIR/lib -lkwt

#EPICS lib
INCPATH     += $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux

#QWT
INCPATH		+= $(QWT_HOME)/include

# Output
target.path = /usr/local/opi/bin
forms.files = ../ui/*.ui
forms.path 	= /usr/local/opi/ui
images.files = ../images/*.png
images.path = /usr/local/opi/images
sources.files = $$SOURCES $$HEADERS *.ui *.pro
sources.path = ../
# install
INSTALLS += target
