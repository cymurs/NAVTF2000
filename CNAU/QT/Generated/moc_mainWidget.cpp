/****************************************************************************
** Meta object code from reading C++ file 'mainWidget.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Widgets/mainWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CMainWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,
      34,   12,   12,   12, 0x08,
      55,   12,   12,   12, 0x08,
      90,   12,   12,   12, 0x08,
     128,   12,   12,   12, 0x08,
     167,   12,   12,   12, 0x08,
     182,   12,   12,   12, 0x08,
     201,   12,   12,   12, 0x08,
     221,   12,   12,   12, 0x08,
     245,   12,   12,   12, 0x08,
     263,   12,   12,   12, 0x08,
     283,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CMainWidget[] = {
    "CMainWidget\0\0SelectOneFile_Slot()\0"
    "SelectOnePath_Slot()\0"
    "PageClicked_Slot(QListWidgetItem*)\0"
    "PageUseClicked_Slot(QListWidgetItem*)\0"
    "PageItemClicked_Slot(QListWidgetItem*)\0"
    "PageAdd_Slot()\0PageItemAdd_Slot()\0"
    "PageUseClear_Slot()\0PageItemUseClear_Slot()\0"
    "AdminCheck_Slot()\0FileOpenRead_Slot()\0"
    "PathFileSave_Slot()\0"
};

void CMainWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CMainWidget *_t = static_cast<CMainWidget *>(_o);
        switch (_id) {
        case 0: _t->SelectOneFile_Slot(); break;
        case 1: _t->SelectOnePath_Slot(); break;
        case 2: _t->PageClicked_Slot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 3: _t->PageUseClicked_Slot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 4: _t->PageItemClicked_Slot((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 5: _t->PageAdd_Slot(); break;
        case 6: _t->PageItemAdd_Slot(); break;
        case 7: _t->PageUseClear_Slot(); break;
        case 8: _t->PageItemUseClear_Slot(); break;
        case 9: _t->AdminCheck_Slot(); break;
        case 10: _t->FileOpenRead_Slot(); break;
        case 11: _t->PathFileSave_Slot(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CMainWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CMainWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CMainWidget,
      qt_meta_data_CMainWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CMainWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CMainWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CMainWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CMainWidget))
        return static_cast<void*>(const_cast< CMainWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int CMainWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
