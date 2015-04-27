TEMPLATE     = app
CONFIG       += thread warn_on

MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH  += ./include


TARGET       = ThreadTest

SOURCES += threadTest.cpp

LIBS += -L/usr/local/lib -lboost_thread

# install
target.path = ./
INSTALLS += target
