/****************************************************************************
** Meta object code from reading C++ file 'sqlquerytablemodel.h'
**
** Created: Mon Jun 9 16:46:19 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "sqlquerytablemodel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'sqlquerytablemodel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_SqlQueryTableModel[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_SqlQueryTableModel[] = {
    "SqlQueryTableModel\0"
};

const QMetaObject SqlQueryTableModel::staticMetaObject = {
    { &QSqlTableModel::staticMetaObject, qt_meta_stringdata_SqlQueryTableModel,
      qt_meta_data_SqlQueryTableModel, 0 }
};

const QMetaObject *SqlQueryTableModel::metaObject() const
{
    return &staticMetaObject;
}

void *SqlQueryTableModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SqlQueryTableModel))
	return static_cast<void*>(const_cast< SqlQueryTableModel*>(this));
    return QSqlTableModel::qt_metacast(_clname);
}

int SqlQueryTableModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSqlTableModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
