/****************************************************************************
** Meta object code from reading C++ file 'statusTab.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Widgets/statusTab.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'statusTab.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CStatusTab_t {
    QByteArrayData data[6];
    char stringdata0[74];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CStatusTab_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CStatusTab_t qt_meta_stringdata_CStatusTab = {
    {
QT_MOC_LITERAL(0, 0, 10), // "CStatusTab"
QT_MOC_LITERAL(1, 11, 18), // "HandleFrame_Signal"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 12), // "chSourceAddr"
QT_MOC_LITERAL(4, 44, 12), // "chSourcePort"
QT_MOC_LITERAL(5, 57, 16) // "HandleFrame_Slot"

    },
    "CStatusTab\0HandleFrame_Signal\0\0"
    "chSourceAddr\0chSourcePort\0HandleFrame_Slot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CStatusTab[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,   24,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       5,    3,   31,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::UChar, QMetaType::UChar,    2,    3,    4,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::UChar, QMetaType::UChar,    2,    3,    4,

       0        // eod
};

void CStatusTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CStatusTab *_t = static_cast<CStatusTab *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->HandleFrame_Signal((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2])),(*reinterpret_cast< unsigned char(*)>(_a[3]))); break;
        case 1: _t->HandleFrame_Slot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2])),(*reinterpret_cast< unsigned char(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CStatusTab::*_t)(const QString & , unsigned char , unsigned char );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CStatusTab::HandleFrame_Signal)) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject CStatusTab::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CStatusTab.data,
      qt_meta_data_CStatusTab,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CStatusTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CStatusTab::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CStatusTab.stringdata0))
        return static_cast<void*>(const_cast< CStatusTab*>(this));
    return QWidget::qt_metacast(_clname);
}

int CStatusTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 2)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void CStatusTab::HandleFrame_Signal(const QString & _t1, unsigned char _t2, unsigned char _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
