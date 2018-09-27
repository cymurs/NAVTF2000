/****************************************************************************
** Meta object code from reading C++ file 'adjustTab.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../ComAssist/QT/Widgets/adjustTab.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'adjustTab.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CMyAdjustTab[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   14,   13,   13, 0x05,
      75,   48,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     131,   14,   13,   13, 0x08,
     161,   48,   13,   13, 0x08,
     215,   13,   13,   13, 0x08,
     234,   13,   13,   13, 0x08,
     250,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CMyAdjustTab[] = {
    "CMyAdjustTab\0\0,\0ShowInfo_Signal(QString,QColor)\0"
    ",chSourceAddr,chSourcePort\0"
    "HandleFrame_Signal(QString,unsigned char,unsigned char)\0"
    "ShowInfo_Slot(QString,QColor)\0"
    "HandleFrame_Slot(QString,unsigned char,unsigned char)\0"
    "AdjustMapFunc(int)\0ClearInfoFunc()\0"
    "PswLockFunc()\0"
};

void CMyAdjustTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CMyAdjustTab *_t = static_cast<CMyAdjustTab *>(_o);
        switch (_id) {
        case 0: _t->ShowInfo_Signal((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 1: _t->HandleFrame_Signal((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2])),(*reinterpret_cast< unsigned char(*)>(_a[3]))); break;
        case 2: _t->ShowInfo_Slot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 3: _t->HandleFrame_Slot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2])),(*reinterpret_cast< unsigned char(*)>(_a[3]))); break;
        case 4: _t->AdjustMapFunc((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->ClearInfoFunc(); break;
        case 6: _t->PswLockFunc(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CMyAdjustTab::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CMyAdjustTab::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CMyAdjustTab,
      qt_meta_data_CMyAdjustTab, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CMyAdjustTab::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CMyAdjustTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CMyAdjustTab::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CMyAdjustTab))
        return static_cast<void*>(const_cast< CMyAdjustTab*>(this));
    return QWidget::qt_metacast(_clname);
}

int CMyAdjustTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void CMyAdjustTab::ShowInfo_Signal(const QString & _t1, QColor _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CMyAdjustTab::HandleFrame_Signal(const QString & _t1, unsigned char _t2, unsigned char _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
