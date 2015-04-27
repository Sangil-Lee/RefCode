/****************************************************************************
** Meta object code from reading C++ file 'browser.h'
**
** Created: Thu Jun 12 11:34:03 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "browser.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'browser.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_Browser[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      17,    9,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
      40,    8,    8,    8, 0x0a,
      53,   47,    8,    8, 0x0a,
      73,   67,    8,    8, 0x0a,
      92,   67,    8,    8, 0x0a,
     114,    8,    8,    8, 0x0a,
     130,    8,    8,    8, 0x0a,
     147,    8,    8,    8, 0x0a,
     155,    8,    8,    8, 0x0a,
     186,    8,    8,    8, 0x0a,
     217,   67,    8,    8, 0x0a,
     261,   67,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_Browser[] = {
    "Browser\0\0message\0statusMessage(QString)\0"
    "exec()\0query\0exec(QString)\0table\0"
    "showTable(QString)\0showMetaData(QString)\0"
    "addConnection()\0currentChanged()\0"
    "about()\0on_insertRowAction_triggered()\0"
    "on_deleteRowAction_triggered()\0"
    "on_connectionWidget_tableActivated(QString)\0"
    "on_connectionWidget_metaDataRequested(QString)\0"
};

const QMetaObject Browser::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Browser,
      qt_meta_data_Browser, 0 }
};

const QMetaObject *Browser::metaObject() const
{
    return &staticMetaObject;
}

void *Browser::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Browser))
	return static_cast<void*>(const_cast< Browser*>(this));
    return QWidget::qt_metacast(_clname);
}

int Browser::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: statusMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: exec(); break;
        case 2: exec((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: showTable((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: showMetaData((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: addConnection(); break;
        case 6: currentChanged(); break;
        case 7: about(); break;
        case 8: on_insertRowAction_triggered(); break;
        case 9: on_deleteRowAction_triggered(); break;
        case 10: on_connectionWidget_tableActivated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 11: on_connectionWidget_metaDataRequested((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        }
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void Browser::statusMessage(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
