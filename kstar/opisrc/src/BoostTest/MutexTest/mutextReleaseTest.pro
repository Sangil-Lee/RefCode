TEMPLATE     = app
CONFIG       += thread warn_on

MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH  += ./include


TARGET       = MutextReleaseTest

SOURCES += mutextReleaseTest.cpp

# install
target.path = ./
INSTALLS += target
