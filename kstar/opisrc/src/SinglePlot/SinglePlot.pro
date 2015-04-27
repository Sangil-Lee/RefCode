CONFIG += uitools qt assistant
TEMPLATE = app

MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH  += ../../include

#EPICS lib
EPICS_BASE = /usr/local/epics/base
INCPATH     += $$EPICS_BASE/include $$EPICS_BASE/include/os/Linux
LIBS        += -L$$EPICS_BASE/lib/linux-x86 -lasHost -lcas -ldbStaticHost -lgdd -lrecIoc -lsoftDevIoc -lasIoc -lCom -ldbStaticIoc -liocsh -lregistryIoc -ltestDevIoc -lca -ldbIoc -ldbtoolsIoc -lmiscIoc -lrsrvIoc

#QWT
INCPATH		+= /usr/local/qwt/include
LIBS        += -L/usr/local/qwt/lib -lqwt

#KWT
KWT_INC	+= /usr/local/opisrc/kwt/include
KWT_LIB	+= /usr/local/opi/lib
INCPATH		+= $$KWT_INC
LIBS		+= -L$$KWT_LIB/ -lkwt

TARGET       = singleplot

SOURCES += main.cpp

# install
target.path = /usr/local/opi/bin
INSTALLS += target
