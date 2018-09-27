/****************************************************************************
** Meta object code from reading C++ file 'gnssTab.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Widgets/gnssTab.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gnssTab.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CGNSSTab[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      37,   10,    9,    9, 0x05,
     102,   93,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
     134,    9,    9,    9, 0x08,
     159,    9,    9,    9, 0x08,
     184,    9,    9,    9, 0x08,
     205,    9,    9,    9, 0x08,
     226,    9,    9,    9, 0x08,
     247,    9,    9,    9, 0x08,
     268,    9,    9,    9, 0x08,
     289,    9,    9,    9, 0x08,
     310,    9,    9,    9, 0x08,
     331,    9,    9,    9, 0x08,
     352,    9,    9,    9, 0x08,
     380,    9,    9,    9, 0x08,
     408,    9,    9,    9, 0x08,
     436,    9,    9,    9, 0x08,
     464,    9,    9,    9, 0x08,
     492,    9,    9,    9, 0x08,
     520,    9,    9,    9, 0x08,
     548,    9,    9,    9, 0x08,
     563,   10,    9,    9, 0x08,
     617,    9,    9,    9, 0x08,
     634,   93,    9,    9, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CGNSSTab[] = {
    "CGNSSTab\0\0,chSourceAddr,chSourcePort\0"
    "HandleFrame_Signal(QString,unsigned char,unsigned char)\0"
    "strInfo,\0ShowInfo_Signal(QString,QColor)\0"
    "CmdNameChanged_Slot(int)\0"
    "CmdDescChanged_Slot(int)\0CmdUseClicked_Slot()\0"
    "Param0Changed_Slot()\0Param1Changed_Slot()\0"
    "Param2Changed_Slot()\0Param3Changed_Slot()\0"
    "Param4Changed_Slot()\0Param5Changed_Slot()\0"
    "Param6Changed_Slot()\0Param0Changed_Slot(QString)\0"
    "Param1Changed_Slot(QString)\0"
    "Param2Changed_Slot(QString)\0"
    "Param3Changed_Slot(QString)\0"
    "Param4Changed_Slot(QString)\0"
    "Param5Changed_Slot(QString)\0"
    "Param6Changed_Slot(QString)\0SendCmd_Slot()\0"
    "HandleFrame_Slot(QString,unsigned char,unsigned char)\0"
    "ClearInfo_Slot()\0ShowInfo_Slot(QString,QColor)\0"
};

void CGNSSTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CGNSSTab *_t = static_cast<CGNSSTab *>(_o);
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
    }
}

const QMetaObjectExtraData CGNSSTab::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CGNSSTab::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CGNSSTab,
      qt_meta_data_CGNSSTab, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CGNSSTab::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CGNSSTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CGNSSTab::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CGNSSTab))
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
    }
    return _id;
}

// SIGNAL 0
void CGNSSTab::HandleFrame_Signal(const QString & _t1, unsigned char _t2, unsigned char _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CGNSSTab::ShowInfo_Signal(const QString _t1, QColor _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
