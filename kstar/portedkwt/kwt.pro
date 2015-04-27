# -*- mode: sh -*- ###########################
# Qwt Widget Library
# Copyright (C) 1997   Josef Wilgen
# Copyright (C) 2002   Uwe Rathmann
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Qwt License, Version 1.0
##############################################

# pro file for building the makefile for qwt
#
# Please read the instructions in INSTALL, if you don´t know
# how to use it.
#
# HEADERS/SOURCES are separated into plot
# relevant files and others. If you are 
# interested in QwtPlot only, you can remove
# the lists at the end of this file.

TARGET            = kwt
TEMPLATE          = lib

VERSION      = 1.0.0

# Only one of the following flags is allowed !
QT           += uitools
QT           += core 
QT           += widgets concurrent designer gui

#QT           += thread
#QT           += qt

# Only one of the following flags is allowed !
CONFIG           += dll 
# CONFIG           += staticlib

# DEFINES          += QT_NO_CAST_ASCII
# DEFINES          += QT_NO_CAST_TO_ASCII

MOC_DIR           = src/moc
OBJECTS_DIR       = src/obj
QWTDIR		= /usr/local/qwt
DESTDIR           = lib
INCLUDEPATH      += include
INCLUDEPATH      += $$QWTDIR/include
INCLUDEPATH      += $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux
DEPENDPATH       += $$INCLUDEPATH src
#LIBS += -L$$QWTDIR/lib -lqwt

#Add leesi
INCLUDEPATH += ../../include
DEPENDPATH  += ../../include

#EPICS lib
INCPATH     += ./include/ $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux
LIBS        += -L$(EPICS_BASE)/lib/linux-x86_64 -lasHost -lcas -ldbStaticHost -lgdd -lrecIoc -lsoftDevIoc -lasIoc -lCom -ldbStaticIoc -lregistryIoc -ltestDevIoc -lca -ldbIoc -ldbtoolsIoc -lmiscIoc -lrsrvIoc

#mysql
INCPATH += /usr/include/mysql

#mysql++ lib
INCPATH += /usr/include/mysql++
LIBS += -L/usr/local/lib -lmysqlpp

#accesskstardb lib
INCPATH += /usr/local/opisrc/include

#QWT
INCPATH		+= /usr/local/qwt/include
LIBS        += -L/usr/local/qwt/lib -lqwt

#SEQ
INCPATH		+= /usr/local/epics/siteLib/include
LIBS        += -L/usr/local/epics/siteLib/lib/linux-x86_64 -lpv -lseq

win32:dll:DEFINES    += QT_DLL KWT_DLL KWT_MAKEDLL

# debug:QMAKE_CXXFLAGS += -O0

HEADERS = \
	include/kwt.h \
	include/kwt_global.h \
	include/kstardb.h \
	include/kstardbconn.h \
	include/caDisplayer.h \
	include/caComboBox.h \
	include/caBobutton.h \
	include/caCheckbox.h \
	include/caLabel.h \
	include/caLineedit.h \
	include/caQtSignal.h \
	include/staticGraphic.h \
	include/caGraphic.h \
	include/caScheduler.h \
	include/caScreenShot.h \
	include/caListBox.h \ 
	include/caSlider.h \
	include/caMbbobutton.h \
	include/caTableList.h \ 
	include/caBlinkLabel.h \
	include/caUitime.h \
	include/caPushbutton.h \
	include/caColorcheckbox.h \
	include/caMultiwaveplot.h \
	include/caColorTilebutton.h \
	include/caPopup.h \
	include/caFanOut.h \
	include/caProgressBar.h \
	include/caImageMbbi.h \
	include/caArrayThread.h \ 
	include/caMultiplot.h \ 
	include/caWclock.h \
	#include/caWaveformput.h \
	#include/wavePicker.h \
	include/qtchaccesslib.h \
	include/qtchaccessthr.h \
	include/SinglePlotThread.h \
	include/SinglePlotWindow.h \
	include/SinglePlot.h \
	include/password.h \

SOURCES = \
	src/caDisplayer.cpp \
	src/caComboBox.cpp \
	src/caBobutton.cpp \
	src/caCheckbox.cpp \
	src/caLabel.cpp \
	src/caLineedit.cpp \
	src/staticGraphic.cpp \
	src/caGraphic.cpp \
	src/caQtSignal.cpp \
	src/caScheduler.cpp \
	src/caScreenShot.cpp \
	src/caListBox.cpp \ 
	src/caSlider.cpp \
	src/caMbbobutton.cpp \
	src/caTableList.cpp \ 
	src/caBlinkLabel.cpp \
	src/caUitime.cpp \
	src/caPushbutton.cpp \
	src/caColorcheckbox.cpp \
	src/caMultiwaveplot.cpp \
	src/caColorTilebutton.cpp \
	src/caPopup.cpp \
	src/caFanOut.cpp \
	src/caProgressBar.cpp \
	src/caImageMbbi.cpp \
	src/caArrayThread.cpp \ 
	src/caMultiplot.cpp \ 
	src/caWclock.cpp \
#src/caMDSplusplot.cpp \
#src/caMDSplusThread.cpp \ 
#src/caWaveformput.cpp \
#src/wavePicker.cpp \
	src/qtchaccesslib.cpp \
	src/qtchaccessthr.cpp \
	src/SinglePlotThread.cpp \
	src/SinglePlotWindow.cpp \
	src/SinglePlot.cpp \
	src/kstardbconn.cpp \
	src/password.cpp \
 
##############################################
# If you don't need to display svg images
# on the plot canvas, remove the following 
# lines
##############################################

# QT += svg
# HEADERS += include/qwt_plot_svgitem.h
# SOURCES += src/qwt_plot_svgitem.cpp 

##############################################
# If you are interested in the plot widget
# only, you can remove the following 
# HEADERS/SOURCES lists.
##############################################

# HEADERS += \

# SOURCES += \

unix {
    INSTALLBASE    = /usr/local/opi
    target.path    = $$INSTALLBASE/lib
    headers.path   = $$INSTALLBASE/include
    headers.files  = $$HEADERS
    INSTALLS       = target headers
}
