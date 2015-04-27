TEMPLATE     = app
#CONFIG       += qt thread warn_on uitools

MOC_DIR      = .moc
OBJECTS_DIR  = .obj

#MathGL
INCPATH += /usr/local/include/mgl2
LIBS += -L/usr/local/lib -lmgl -lmgl-wnd

TARGET       = mathgltest

SOURCES += mathgltest.cpp

# install
target.path = ./
INSTALLS += target
