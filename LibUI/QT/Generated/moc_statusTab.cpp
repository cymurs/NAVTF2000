/****************************************************************************
** Meta object code from reading C++ file 'statusTab.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../ComAssist/QT/Widgets/statusTab.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'statusTab.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CStatusTab[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      39,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      95,   12,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CStatusTab[] = {
    "CStatusTab\0\0,chSourceAddr,chSourcePort\0"
    "HandleFrame_Signal(QString,unsigned char,unsigned char)\0"
    "HandleFrame_Slot(QString,unsigned char,unsigned char)\0"
};

void CStatusTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CStatusTab *_t = static_cast<CStatusTab *>(_o);
        switch (_id) {
        case 0: _t->HandleFrame_Signal((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2])),(*reinterpret_cast< unsigned char(*)>(_a[3]))); break;
        case 1: _t->HandleFrame_Slot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2])),(*reinterpret_cast< unsigned char(*)>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CStatusTab::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CStatusTab::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CStatusTab,
      qt_meta_data_CStatusTab, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CStatusTab::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CStatusTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CStatusTab::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CStatusTab))
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
    }
    return _id;
}

// SIGNAL 0
void CStatusTab::HandleFrame_Signal(const QString & _t1, unsigned char _t2, unsigned char _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
