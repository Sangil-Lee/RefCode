TEMPLATE     = app
CONFIG       += qt thread warn_on uitools

MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH  += ./include

#EPICS lib
INCPATH     += ./include/ $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux
LIBS        += -L$(EPICS_BASE)/lib/linux-x86 -lasHost -lcas -ldbStaticHost -lgdd -lrecIoc -lsoftDevIoc -lasIoc -lCom -ldbStaticIoc -liocsh -lregistryIoc -ltestDevIoc -lca -ldbIoc -ldbtoolsIoc -lmiscIoc -lrsrvIoc

#QWT
INCPATH		+= /usr/local/qwt/include
LIBS        += -L/usr/local/qwt/lib -lqwt

TARGET       = multiplot

HEADERS += MultiPlot.h DataModellerThread.h MultiplotMainWindow.h Screenshot.h MultiPlotInfo.h
SOURCES += main.cpp MultiPlot.cpp DataModellerThread.cpp MultiplotMainWindow.cpp Screenshot.cpp

# install
target.path = ./MultiPlot
sources.path = ./MultiPlot
INSTALLS += target sources
