TEMPLATE     = app
#CONFIG       += qt thread warn_on uitools

MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH  += ../../include

#EPICS inc
INCPATH     += $(EPICS_BASE)/include $(EPICS_BASE)/include/os/Linux

#mysql
INCPATH += /usr/include/mysql

#mysql++ lib 
INCPATH += /usr/local/include/mysql++
LIBS += -L/usr/local/lib -lmysqlpp

TARGET       = DBMerge

SOURCES += main.cpp

# install
target.path = /usr/local/opi/bin
sources.path = ./DBMerge
INSTALLS += target
