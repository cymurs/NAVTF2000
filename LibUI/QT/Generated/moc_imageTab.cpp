/****************************************************************************
** Meta object code from reading C++ file 'imageTab.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../ComAssist/QT/Widgets/imageTab.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'imageTab.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CMyImageTab[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   13,   12,   12, 0x05,
      76,   49,   12,   12, 0x05,
     176,  132,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     225,   13,   12,   12, 0x08,
     257,   49,   12,   12, 0x08,
     311,   12,   12,   12, 0x08,
     324,   12,   12,   12, 0x08,
     340,   12,   12,   12, 0x08,
     352,   12,   12,   12, 0x08,
     373,   12,   12,   12, 0x08,
     392,  132,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CMyImageTab[] = {
    "CMyImageTab\0\0,\0ShowPrioInfo_Signal(QString,bool)\0"
    ",chSourceAddr,chSourcePort\0"
    "HandleFrame_Signal(QString,unsigned char,unsigned char)\0"
    "iLocalLock,iRefLock,i10MLock,i1023MLock,,,,\0"
    "ShowLock_Signal(int,int,int,int,int,int,int,int)\0"
    "ShowPrioInfo_Slot(QString,bool)\0"
    "HandleFrame_Slot(QString,unsigned char,unsigned char)\0"
    "TimerReach()\0ComNetFunc(int)\0AlignFunc()\0"
    "PriorityQuery_Slot()\0PrioritySet_Slot()\0"
    "ShowLock_Slot(int,int,int,int,int,int,int,int)\0"
};

void CMyImageTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CMyImageTab *_t = static_cast<CMyImageTab *>(_o);
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
    }
}

const QMetaObjectExtraData CMyImageTab::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CMyImageTab::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CMyImageTab,
      qt_meta_data_CMyImageTab, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CMyImageTab::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CMyImageTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CMyImageTab::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CMyImageTab))
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
    }
    return _id;
}

// SIGNAL 0
void CMyImageTab::ShowPrioInfo_Signal(const QString & _t1, bool _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CMyImageTab::HandleFrame_Signal(const QString & _t1, unsigned char _t2, unsigned char _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CMyImageTab::ShowLock_Signal(int _t1, int _t2, int _t3, int _t4, int _t5, int _t6, int _t7, int _t8)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)), const_cast<void*>(reinterpret_cast<const void*>(&_t8)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
