TEMPLATE     = app
CONFIG       += qt thread warn_on

MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH  += ./include


#MDSObjectCPP
INCPATH		+= /usr/local/mdsplus/include
LIBS        += -L/usr/local/mdsplus/lib -lMdsObjectsCppShr

TARGET       = MDSObjectTest

SOURCES += mdsObjectTest.cpp

# install
target.path = ./MDS++Test
INSTALLS += target
