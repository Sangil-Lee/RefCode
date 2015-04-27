/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created: Mon Aug 13 09:08:25 2012
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../include/MainWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_MainWindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      16,   12,   11,   11, 0x08,
      37,   11,   11,   11, 0x08,
      53,   11,   11,   11, 0x08,
      74,   11,   11,   11, 0x08,
      89,   11,   11,   11, 0x08,
     102,   11,   11,   11, 0x08,
     117,   11,   11,   11, 0x08,
     142,  138,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0top\0showFullWindow(bool)\0"
    "showMultiplot()\0showArchiverviewer()\0"
    "showSignalDB()\0showManual()\0showAboutMPS()\0"
    "setDefaultIndex(int)\0str\0changeText(QString)\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

const QMetaObject *MainWindow::metaObject() const
{
    return &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
	return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: showFullWindow((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: showMultiplot(); break;
        case 2: showArchiverviewer(); break;
        case 3: showSignalDB(); break;
        case 4: showManual(); break;
        case 5: showAboutMPS(); break;
        case 6: setDefaultIndex((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: changeText((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        }
        _id -= 8;
    }
    return _id;
}
