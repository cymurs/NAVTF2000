/****************************************************************************
** Meta object code from reading C++ file 'gnssTab.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Widgets/gnssTab.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gnssTab.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CGNSSTab_t {
    QByteArrayData data[21];
    char stringdata0[330];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CGNSSTab_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CGNSSTab_t qt_meta_stringdata_CGNSSTab = {
    {
QT_MOC_LITERAL(0, 0, 8), // "CGNSSTab"
QT_MOC_LITERAL(1, 9, 18), // "HandleFrame_Signal"
QT_MOC_LITERAL(2, 28, 0), // ""
QT_MOC_LITERAL(3, 29, 12), // "chSourceAddr"
QT_MOC_LITERAL(4, 42, 12), // "chSourcePort"
QT_MOC_LITERAL(5, 55, 15), // "ShowInfo_Signal"
QT_MOC_LITERAL(6, 71, 7), // "strInfo"
QT_MOC_LITERAL(7, 79, 19), // "CmdNameChanged_Slot"
QT_MOC_LITERAL(8, 99, 19), // "CmdDescChanged_Slot"
QT_MOC_LITERAL(9, 119, 18), // "CmdUseClicked_Slot"
QT_MOC_LITERAL(10, 138, 18), // "Param0Changed_Slot"
QT_MOC_LITERAL(11, 157, 18), // "Param1Changed_Slot"
QT_MOC_LITERAL(12, 176, 18), // "Param2Changed_Slot"
QT_MOC_LITERAL(13, 195, 18), // "Param3Changed_Slot"
QT_MOC_LITERAL(14, 214, 18), // "Param4Changed_Slot"
QT_MOC_LITERAL(15, 233, 18), // "Param5Changed_Slot"
QT_MOC_LITERAL(16, 252, 18), // "Param6Changed_Slot"
QT_MOC_LITERAL(17, 271, 12), // "SendCmd_Slot"
QT_MOC_LITERAL(18, 284, 16), // "HandleFrame_Slot"
QT_MOC_LITERAL(19, 301, 14), // "ClearInfo_Slot"
QT_MOC_LITERAL(20, 316, 13) // "ShowInfo_Slot"

    },
    "CGNSSTab\0HandleFrame_Signal\0\0chSourceAddr\0"
    "chSourcePort\0ShowInfo_Signal\0strInfo\0"
    "CmdNameChanged_Slot\0CmdDescChanged_Slot\0"
    "CmdUseClicked_Slot\0Param0Changed_Slot\0"
    "Param1Changed_Slot\0Param2Changed_Slot\0"
    "Param3Changed_Slot\0Param4Changed_Slot\0"
    "Param5Changed_Slot\0Param6Changed_Slot\0"
    "SendCmd_Slot\0HandleFrame_Slot\0"
    "ClearInfo_Slot\0ShowInfo_Slot"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CGNSSTab[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    3,  129,    2, 0x06 /* Public */,
       5,    2,  136,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    1,  141,    2, 0x08 /* Private */,
       8,    1,  144,    2, 0x08 /* Private */,
       9,    0,  147,    2, 0x08 /* Private */,
      10,    0,  148,    2, 0x08 /* Private */,
      11,    0,  149,    2, 0x08 /* Private */,
      12,    0,  150,    2, 0x08 /* Private */,
      13,    0,  151,    2, 0x08 /* Private */,
      14,    0,  152,    2, 0x08 /* Private */,
      15,    0,  153,    2, 0x08 /* Private */,
      16,    0,  154,    2, 0x08 /* Private */,
      10,    1,  155,    2, 0x08 /* Private */,
      11,    1,  158,    2, 0x08 /* Private */,
      12,    1,  161,    2, 0x08 /* Private */,
      13,    1,  164,    2, 0x08 /* Private */,
      14,    1,  167,    2, 0x08 /* Private */,
      15,    1,  170,    2, 0x08 /* Private */,
      16,    1,  173,    2, 0x08 /* Private */,
      17,    0,  176,    2, 0x08 /* Private */,
      18,    3,  177,    2, 0x08 /* Private */,
      19,    0,  184,    2, 0x08 /* Private */,
      20,    2,  185,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::UChar, QMetaType::UChar,    2,    3,    4,
    QMetaType::Void, QMetaType::QString, QMetaType::QColor,    6,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::UChar, QMetaType::UChar,    2,    3,    4,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QColor,    6,    2,

       0        // eod
};

void CGNSSTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CGNSSTab *_t = static_cast<CGNSSTab *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->HandleFrame_Signal((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2])),(*reinterpret_cast< unsigned char(*)>(_a[3]))); break;
        case 1: _t->ShowInfo_Signal((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 2: _t->CmdNameChanged_Slot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->CmdDescChanged_Slot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->CmdUseClicked_Slot(); break;
        case 5: _t->Param0Changed_Slot(); break;
        case 6: _t->Param1Changed_Slot(); break;
        case 7: _t->Param2Changed_Slot(); break;
        case 8: _t->Param3Changed_Slot(); break;
        case 9: _t->Param4Changed_Slot(); break;
        case 10: _t->Param5Changed_Slot(); break;
        case 11: _t->Param6Changed_Slot(); break;
        case 12: _t->Param0Changed_Slot((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: _t->Param1Changed_Slot((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 14: _t->Param2Changed_Slot((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: _t->Param3Changed_Slot((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 16: _t->Param4Changed_Slot((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 17: _t->Param5Changed_Slot((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 18: _t->Param6Changed_Slot((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 19: _t->SendCmd_Slot(); break;
        case 20: _t->HandleFrame_Slot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2])),(*reinterpret_cast< unsigned char(*)>(_a[3]))); break;
        case 21: _t->ClearInfo_Slot(); break;
        case 22: _t->ShowInfo_Slot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CGNSSTab::*_t)(const QString & , unsigned char , unsigned char );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CGNSSTab::HandleFrame_Signal)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (CGNSSTab::*_t)(const QString , QColor );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CGNSSTab::ShowInfo_Signal)) {
                *result = 1;
                return;
            }
        }
    }
}

const QMetaObject CGNSSTab::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CGNSSTab.data,
      qt_meta_data_CGNSSTab,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CGNSSTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CGNSSTab::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CGNSSTab.stringdata0))
        return static_cast<void*>(const_cast< CGNSSTab*>(this));
    return QWidget::qt_metacast(_clname);
}

int CGNSSTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 23)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 23;
    }
    return _id;
}

// SIGNAL 0
void CGNSSTab::HandleFrame_Signal(const QString & _t1, unsigned char _t2, unsigned char _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CGNSSTab::ShowInfo_Signal(const QString _t1, QColor _t2)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
