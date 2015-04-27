TEMPLATE    = lib
CONFIG       += desinger qt thread warn_on uitools

MOC_DIR      = .moc
OBJECTS_DIR  = .obj
INCLUDEPATH += ../../include
DEPENDPATH  += ../../include
#DESTDIR     = $$QT_BUILD_TREE/plugins/designer

HEADERS += password.h
SOURCES += password.cpp 

# install
target.path = ../../libshared/
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = ../../libshared/libpasswd
INSTALLS += target
