TEMPLATE     = app
CONFIG       += qt thread warn_on uitools

MOC_DIR      = .moc
OBJECTS_DIR  = .obj
DEPENDPATH  += ../../include

TARGET       = MakeArchiveList

SOURCES += main.cpp

# install
target.path = /usr/local/opi/bin
sources.path = ./DBMerge
INSTALLS += target
