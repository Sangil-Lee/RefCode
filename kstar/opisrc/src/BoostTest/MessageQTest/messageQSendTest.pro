TEMPLATE     = app
CONFIG       += thread warn_on

MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH  += ./include


#MDSObjectCPP
INCPATH		+= /usr/local/include/boost
LIBS        += -L/usr/local/mdsplus/lib -lMdsObjectsCppShr

TARGET       = MesageQSendTest

SOURCES += messageQSendTest.cpp

# install
target.path = ./MDS++Test
INSTALLS += target
