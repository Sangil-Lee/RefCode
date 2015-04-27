TEMPLATE     = app
CONFIG       += thread warn_on

MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH  += ./include


TARGET       = MutextLockTest

SOURCES += mutextLockTest.cpp

# install
target.path = ./
INSTALLS += target
