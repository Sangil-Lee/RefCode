TEMPLATE     = app
CONFIG       += qt thread warn_on uitools

MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH  += ../../include

#EPICS inc
INCPATH     += $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux

#QWT
#INCPATH		+= /usr/local/qwt/include
#LIBS        += -L/usr/local/qwt/lib -lqwt

#KWT
INCPATH		+= /usr/local/opisrc/kwt/include
LIBS		+= -L/usr/local/opi/lib -lkwt


#pvlist inc
INCPATH += /usr/local/opisrc/include
LIBS += -L/usr/local/opi/lib -lpvlistviewer

#mysql
INCPATH += /usr/include/mysql

#mysql++ lib 
INCPATH += /usr/local/include/mysql++
LIBS += -L/usr/local/lib -lmysqlpp

TARGET       = PVListViewer

SOURCES += main.cpp

# install
target.path = /usr/local/opi/bin
sources.path = ./PVListViewer
INSTALLS += target
