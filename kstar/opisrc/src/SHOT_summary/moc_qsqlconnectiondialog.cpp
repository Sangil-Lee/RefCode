/****************************************************************************
** Meta object code from reading C++ file 'qsqlconnectiondialog.h'
**
** Created: Sun Jun 8 16:09:35 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qsqlconnectiondialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qsqlconnectiondialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_QSqlConnectionDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x08,
      44,   21,   21,   21, 0x08,
      70,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_QSqlConnectionDialog[] = {
    "QSqlConnectionDialog\0\0on_okButton_clicked()\0"
    "on_cancelButton_clicked()\0"
    "on_dbCheckBox_clicked()\0"
};

const QMetaObject QSqlConnectionDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QSqlConnectionDialog,
      qt_meta_data_QSqlConnectionDialog, 0 }
};

const QMetaObject *QSqlConnectionDialog::metaObject() const
{
    return &staticMetaObject;
}

void *QSqlConnectionDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QSqlConnectionDialog))
	return static_cast<void*>(const_cast< QSqlConnectionDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int QSqlConnectionDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_okButton_clicked(); break;
        case 1: on_cancelButton_clicked(); break;
        case 2: on_dbCheckBox_clicked(); break;
        }
        _id -= 3;
    }
    return _id;
}
