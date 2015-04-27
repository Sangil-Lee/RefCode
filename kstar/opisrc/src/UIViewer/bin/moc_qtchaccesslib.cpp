/****************************************************************************
** Meta object code from reading C++ file 'qtchaccesslib.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.3.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../portedkwt/include/qtchaccesslib.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qtchaccesslib.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.3.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_AttachChannelAccess_t {
    QByteArrayData data[24];
    char stringdata[277];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_AttachChannelAccess_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_AttachChannelAccess_t qt_meta_stringdata_AttachChannelAccess = {
    {
QT_MOC_LITERAL(0, 0, 19),
QT_MOC_LITERAL(1, 20, 12),
QT_MOC_LITERAL(2, 33, 0),
QT_MOC_LITERAL(3, 34, 7),
QT_MOC_LITERAL(4, 42, 8),
QT_MOC_LITERAL(5, 51, 9),
QT_MOC_LITERAL(6, 61, 8),
QT_MOC_LITERAL(7, 70, 21),
QT_MOC_LITERAL(8, 92, 23),
QT_MOC_LITERAL(9, 116, 14),
QT_MOC_LITERAL(10, 131, 5),
QT_MOC_LITERAL(11, 137, 14),
QT_MOC_LITERAL(12, 152, 16),
QT_MOC_LITERAL(13, 169, 2),
QT_MOC_LITERAL(14, 172, 14),
QT_MOC_LITERAL(15, 187, 5),
QT_MOC_LITERAL(16, 193, 14),
QT_MOC_LITERAL(17, 208, 12),
QT_MOC_LITERAL(18, 221, 10),
QT_MOC_LITERAL(19, 232, 4),
QT_MOC_LITERAL(20, 237, 9),
QT_MOC_LITERAL(21, 247, 10),
QT_MOC_LITERAL(22, 258, 10),
QT_MOC_LITERAL(23, 269, 7)
    },
    "AttachChannelAccess\0updatesignal\0\0"
    "objname\0severity\0precision\0getValue\0"
    "capushbuttonpressSlot\0capushbuttonreleaseSlot\0"
    "cabobuttonSlot\0check\0cacheckboxSlot\0"
    "cambbobuttonSlot\0id\0cacomboboxSlot\0"
    "value\0calineeditSlot\0casliderSlot\0"
    "actionSlot\0type\0updateobj\0getControl\0"
    "setControl\0control"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_AttachChannelAccess[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   89,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       6,    0,   96,    2, 0x0a /* Public */,
       7,    0,   97,    2, 0x0a /* Public */,
       8,    0,   98,    2, 0x0a /* Public */,
       9,    1,   99,    2, 0x0a /* Public */,
      11,    1,  102,    2, 0x0a /* Public */,
      12,    1,  105,    2, 0x0a /* Public */,
      14,    1,  108,    2, 0x0a /* Public */,
      16,    1,  111,    2, 0x0a /* Public */,
      17,    0,  114,    2, 0x0a /* Public */,
      18,    1,  115,    2, 0x0a /* Public */,
      16,    1,  118,    2, 0x0a /* Public */,
      20,    3,  121,    2, 0x0a /* Public */,
      21,    0,  128,    2, 0x0a /* Public */,
      22,    1,  129,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Short, QMetaType::Short,    3,    4,    5,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::Int,   13,
    QMetaType::Void, QMetaType::Int,   15,
    QMetaType::Void, QMetaType::Double,   15,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   19,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void, QMetaType::QString, QMetaType::Short, QMetaType::Short,    3,    4,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   23,

       0        // eod
};

void AttachChannelAccess::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        AttachChannelAccess *_t = static_cast<AttachChannelAccess *>(_o);
        switch (_id) {
        case 0: _t->updatesignal((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const short(*)>(_a[2])),(*reinterpret_cast< const short(*)>(_a[3]))); break;
        case 1: _t->getValue(); break;
        case 2: _t->capushbuttonpressSlot(); break;
        case 3: _t->capushbuttonreleaseSlot(); break;
        case 4: _t->cabobuttonSlot((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->cacheckboxSlot((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->cambbobuttonSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->cacomboboxSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->calineeditSlot((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 9: _t->casliderSlot(); break;
        case 10: _t->actionSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->calineeditSlot((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 12: _t->updateobj((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const short(*)>(_a[2])),(*reinterpret_cast< const short(*)>(_a[3]))); break;
        case 13: _t->getControl(); break;
        case 14: _t->setControl((*reinterpret_cast< const bool(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (AttachChannelAccess::*_t)(const QString & , const short & , const short & );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&AttachChannelAccess::updatesignal)) {
                *result = 0;
            }
        }
    }
}

const QMetaObject AttachChannelAccess::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_AttachChannelAccess.data,
      qt_meta_data_AttachChannelAccess,  qt_static_metacall, 0, 0}
};


const QMetaObject *AttachChannelAccess::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AttachChannelAccess::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AttachChannelAccess.stringdata))
        return static_cast<void*>(const_cast< AttachChannelAccess*>(this));
    return QWidget::qt_metacast(_clname);
}

int AttachChannelAccess::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void AttachChannelAccess::updatesignal(const QString & _t1, const short & _t2, const short & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
