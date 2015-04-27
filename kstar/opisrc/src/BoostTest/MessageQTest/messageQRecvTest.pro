TEMPLATE     = app
CONFIG       += thread warn_on

MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH  += ./include

TARGET       = MesageQRecvTest

SOURCES += messageQRecvTest.cpp

# install
target.path = ./
INSTALLS += target
