TEMPLATE        = app
TARGET          = SUM

QT              += sql

HEADERS         = browser.h connectionwidget.h qsqlconnectiondialog.h sqlquerytablemodel.h
SOURCES         = main.cpp browser.cpp connectionwidget.cpp qsqlconnectiondialog.cpp sqlquerytablemodel.cpp

FORMS           = browserwidget.ui qsqlconnectiondialog.ui
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

# install
target.path = /usr/local/opi/bin
sources.files = $$SOURCES $$HEADERS $$FORMS *.pro
INSTALLS += target
