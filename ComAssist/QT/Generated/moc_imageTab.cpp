/****************************************************************************
** Meta object code from reading C++ file 'imageTab.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Widgets/imageTab.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'imageTab.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CMyImageTab_t {
    QByteArrayData data[19];
    char stringdata0[251];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CMyImageTab_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CMyImageTab_t qt_meta_stringdata_CMyImageTab = {
    {
QT_MOC_LITERAL(0, 0, 11), // "CMyImageTab"
QT_MOC_LITERAL(1, 12, 19), // "ShowPrioInfo_Signal"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 18), // "HandleFrame_Signal"
QT_MOC_LITERAL(4, 52, 12), // "chSourceAddr"
QT_MOC_LITERAL(5, 65, 12), // "chSourcePort"
QT_MOC_LITERAL(6, 78, 15), // "ShowLock_Signal"
QT_MOC_LITERAL(7, 94, 10), // "iLocalLock"
QT_MOC_LITERAL(8, 105, 8), // "iRefLock"
QT_MOC_LITERAL(9, 114, 8), // "i10MLock"
QT_MOC_LITERAL(10, 123, 10), // "i1023MLock"
QT_MOC_LITERAL(11, 134, 17), // "ShowPrioInfo_Slot"
QT_MOC_LITERAL(12, 152, 16), // "HandleFrame_Slot"
QT_MOC_LITERAL(13, 169, 10), // "TimerReach"
QT_MOC_LITERAL(14, 180, 10), // "ComNetFunc"
QT_MOC_LITERAL(15, 191, 9), // "AlignFunc"
QT_MOC_LITERAL(16, 201, 18), // "PriorityQuery_Slot"
QT_MOC_LITERAL(17, 220, 16), // "PrioritySet_Slot"
QT_MOC_LITERAL(18, 237, 13) // "ShowLock_Slot"

    },
    "CMyImageTab\0ShowPrioInfo_Signal\0\0"
    "HandleFrame_Signal\0chSourceAddr\0"
    "chSourcePort\0ShowLock_Signal\0iLocalLock\0"
    "iRefLock\0i10MLock\0i1023MLock\0"
    "ShowPrioInfo_Slot\0HandleFrame_Slot\0"
    "TimerReach\0ComNetFunc\0AlignFunc\0"
    "PriorityQuery_Slot\0PrioritySet_Slot\0"
    "ShowLock_Slot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CMyImageTab[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   69,    2, 0x06 /* Public */,
       3,    3,   74,    2, 0x06 /* Public */,
       6,    8,   81,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      11,    2,   98,    2, 0x08 /* Private */,
      12,    3,  103,    2, 0x08 /* Private */,
      13,    0,  110,    2, 0x08 /* Private */,
      14,    1,  111,    2, 0x08 /* Private */,
      15,    0,  114,    2, 0x08 /* Private */,
      16,    0,  115,    2, 0x08 /* Private */,
      17,    0,  116,    2, 0x08 /* Private */,
      18,    8,  117,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    2,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::UChar, QMetaType::UChar,    2,    4,    5,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    7,    8,    9,   10,    2,    2,    2,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::Bool,    2,    2,
    QMetaType::Void, QMetaType::QString, QMetaType::UChar, QMetaType::UChar,    2,    4,    5,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int, QMetaType::Int,    7,    8,    9,   10,    2,    2,    2,    2,

       0        // eod
};

void CMyImageTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CMyImageTab *_t = static_cast<CMyImageTab *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->ShowPrioInfo_Signal((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 1: _t->HandleFrame_Signal((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2])),(*reinterpret_cast< unsigned char(*)>(_a[3]))); break;
        case 2: _t->ShowLock_Signal((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7])),(*reinterpret_cast< int(*)>(_a[8]))); break;
        case 3: _t->ShowPrioInfo_Slot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 4: _t->HandleFrame_Slot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2])),(*reinterpret_cast< unsigned char(*)>(_a[3]))); break;
        case 5: _t->TimerReach(); break;
        case 6: _t->ComNetFunc((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->AlignFunc(); break;
        case 8: _t->PriorityQuery_Slot(); break;
        case 9: _t->PrioritySet_Slot(); break;
        case 10: _t->ShowLock_Slot((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7])),(*reinterpret_cast< int(*)>(_a[8]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CMyImageTab::*_t)(const QString & , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CMyImageTab::ShowPrioInfo_Signal)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (CMyImageTab::*_t)(const QString & , unsigned char , unsigned char );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CMyImageTab::HandleFrame_Signal)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (CMyImageTab::*_t)(int , int , int , int , int , int , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CMyImageTab::ShowLock_Signal)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject CMyImageTab::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CMyImageTab.data,
      qt_meta_data_CMyImageTab,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CMyImageTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CMyImageTab::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CMyImageTab.stringdata0))
        return static_cast<void*>(const_cast< CMyImageTab*>(this));
    return QWidget::qt_metacast(_clname);
}

int CMyImageTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void CMyImageTab::ShowPrioInfo_Signal(const QString & _t1, bool _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CMyImageTab::HandleFrame_Signal(const QString & _t1, unsigned char _t2, unsigned char _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CMyImageTab::ShowLock_Signal(int _t1, int _t2, int _t3, int _t4, int _t5, int _t6, int _t7, int _t8)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)), const_cast<void*>(reinterpret_cast<const void*>(&_t8)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
