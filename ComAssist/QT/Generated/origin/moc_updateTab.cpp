/****************************************************************************
** Meta object code from reading C++ file 'updateTab.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../Widgets/updateTab.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'updateTab.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CUpdateTab[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      46,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   12,   11,   11, 0x05,
     108,   93,   11,   11, 0x05,
     167,  165,   11,   11, 0x05,
     202,   11,   11,   11, 0x05,
     225,   11,   11,   11, 0x05,
     250,   11,   11,   11, 0x05,
     271,   11,   11,   11, 0x05,
     305,  302,   11,   11, 0x05,
     356,  165,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
     394,   11,   11,   11, 0x08,
     409,   11,   11,   11, 0x08,
     429,  424,   11,   11, 0x08,
     457,  453,   11,   11, 0x08,
     472,   11,   11,   11, 0x08,
     491,   11,   11,   11, 0x08,
     508,   11,   11,   11, 0x08,
     525,   11,   11,   11, 0x08,
     536,   11,   11,   11, 0x08,
     552,   12,   11,   11, 0x08,
     624,   93,   11,   11, 0x08,
     679,  165,   11,   11, 0x08,
     712,   11,   11,   11, 0x08,
     738,   11,   11,   11, 0x08,
     763,   11,   11,   11, 0x08,
     784,   11,   11,   11, 0x08,
     807,   11,   11,   11, 0x08,
     826,   11,   11,   11, 0x08,
     846,   11,   11,   11, 0x08,
     863,   11,   11,   11, 0x08,
     886,   11,   11,   11, 0x08,
     911,   11,   11,   11, 0x08,
     936,   11,   11,   11, 0x08,
     956,   11,   11,   11, 0x08,
     971,   11,   11,   11, 0x08,
     989,   11,   11,   11, 0x08,
    1004,   11,   11,   11, 0x08,
    1019,   11,   11,   11, 0x08,
    1033,   11,   11,   11, 0x08,
    1046,   11,   11,   11, 0x08,
    1059,   11,   11,   11, 0x08,
    1082,   11,   11,   11, 0x08,
    1103,   11,   11,   11, 0x08,
    1125,   11,   11,   11, 0x08,
    1151,   11,   11,   11, 0x08,
    1180,  302,   11,   11, 0x08,
    1229,  165,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CUpdateTab[] = {
    "CUpdateTab\0\0,,,,,,\0"
    "ShowRecvText_Signal(unsigned char,unsigned char,QString,bool,int,int,i"
    "nt)\0"
    "chAddr,chPort,\0"
    "ShowSendText_Signal(unsigned char,unsigned char,QString)\0"
    ",\0ShowTipText_Signal(QString,QColor)\0"
    "FrameNoInc_Signal(int)\0BoardSelectFill_Signal()\0"
    "CmdFinished_Signal()\0"
    "DeviceSignalStatus_Signal(int)\0,,\0"
    "CmdLog_Signal(unsigned char,unsigned char,QString)\0"
    "CheckSendedOneCmd_Signal(QString,int)\0"
    "OpenCloseCom()\0OpenCloseNet()\0text\0"
    "ComNameChanged(QString)\0iID\0SaveInput(int)\0"
    "ComNetChanged(int)\0SelectCom_Slot()\0"
    "SelectNet_Slot()\0SendData()\0ClearShowInfo()\0"
    "ShowRecvText_Slot(unsigned char,unsigned char,QString,bool,int,int,int"
    ")\0"
    "ShowSendText_Slot(unsigned char,unsigned char,QString)\0"
    "ShowTipText_Slot(QString,QColor)\0"
    "ShowRecvAutoScroll_Slot()\0"
    "ShowTipAutoScroll_Slot()\0FrameNoInc_Slot(int)\0"
    "BoardSelectFill_Slot()\0CmdFinished_Slot()\0"
    "AutoReSendClicked()\0RecvAllClicked()\0"
    "AutoMakeFrameClicked()\0CheckSend16DataClicked()\0"
    "CheckRecv16DataClicked()\0CheckDebugClicked()\0"
    "EnterClicked()\0ReSendTimeReach()\0"
    "OpenFilePath()\0VersionQuery()\0"
    "StartUpdate()\0StopUpdate()\0AutoUpdate()\0"
    "SelectExportFilePath()\0NetConnectTestFunc()\0"
    "BoardTypeChanged(int)\0BoardSelect_Slot(QString)\0"
    "DeviceSignalStatus_Slot(int)\0"
    "CmdLog_Slot(unsigned char,unsigned char,QString)\0"
    "CheckSendedOneCmd_Slot(QString,int)\0"
};

void CUpdateTab::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CUpdateTab *_t = static_cast<CUpdateTab *>(_o);
        switch (_id) {
        case 0: _t->ShowRecvText_Signal((*reinterpret_cast< unsigned char(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7]))); break;
        case 1: _t->ShowSendText_Signal((*reinterpret_cast< unsigned char(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 2: _t->ShowTipText_Signal((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 3: _t->FrameNoInc_Signal((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->BoardSelectFill_Signal(); break;
        case 5: _t->CmdFinished_Signal(); break;
        case 6: _t->DeviceSignalStatus_Signal((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->CmdLog_Signal((*reinterpret_cast< unsigned char(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 8: _t->CheckSendedOneCmd_Signal((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 9: _t->OpenCloseCom(); break;
        case 10: _t->OpenCloseNet(); break;
        case 11: _t->ComNameChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: _t->SaveInput((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->ComNetChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->SelectCom_Slot(); break;
        case 15: _t->SelectNet_Slot(); break;
        case 16: _t->SendData(); break;
        case 17: _t->ClearShowInfo(); break;
        case 18: _t->ShowRecvText_Slot((*reinterpret_cast< unsigned char(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5])),(*reinterpret_cast< int(*)>(_a[6])),(*reinterpret_cast< int(*)>(_a[7]))); break;
        case 19: _t->ShowSendText_Slot((*reinterpret_cast< unsigned char(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 20: _t->ShowTipText_Slot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< QColor(*)>(_a[2]))); break;
        case 21: _t->ShowRecvAutoScroll_Slot(); break;
        case 22: _t->ShowTipAutoScroll_Slot(); break;
        case 23: _t->FrameNoInc_Slot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 24: _t->BoardSelectFill_Slot(); break;
        case 25: _t->CmdFinished_Slot(); break;
        case 26: _t->AutoReSendClicked(); break;
        case 27: _t->RecvAllClicked(); break;
        case 28: _t->AutoMakeFrameClicked(); break;
        case 29: _t->CheckSend16DataClicked(); break;
        case 30: _t->CheckRecv16DataClicked(); break;
        case 31: _t->CheckDebugClicked(); break;
        case 32: _t->EnterClicked(); break;
        case 33: _t->ReSendTimeReach(); break;
        case 34: _t->OpenFilePath(); break;
        case 35: _t->VersionQuery(); break;
        case 36: _t->StartUpdate(); break;
        case 37: _t->StopUpdate(); break;
        case 38: _t->AutoUpdate(); break;
        case 39: _t->SelectExportFilePath(); break;
        case 40: _t->NetConnectTestFunc(); break;
        case 41: _t->BoardTypeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 42: _t->BoardSelect_Slot((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 43: _t->DeviceSignalStatus_Slot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 44: _t->CmdLog_Slot((*reinterpret_cast< unsigned char(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3]))); break;
        case 45: _t->CheckSendedOneCmd_Slot((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CUpdateTab::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CUpdateTab::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CUpdateTab,
      qt_meta_data_CUpdateTab, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CUpdateTab::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CUpdateTab::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CUpdateTab::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CUpdateTab))
        return static_cast<void*>(const_cast< CUpdateTab*>(this));
    return QWidget::qt_metacast(_clname);
}

int CUpdateTab::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 46)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 46;
    }
    return _id;
}

// SIGNAL 0
void CUpdateTab::ShowRecvText_Signal(unsigned char _t1, unsigned char _t2, const QString _t3, bool _t4, int _t5, int _t6, int _t7)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)), const_cast<void*>(reinterpret_cast<const void*>(&_t6)), const_cast<void*>(reinterpret_cast<const void*>(&_t7)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CUpdateTab::ShowSendText_Signal(unsigned char _t1, unsigned char _t2, const QString _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CUpdateTab::ShowTipText_Signal(const QString _t1, QColor _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void CUpdateTab::FrameNoInc_Signal(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void CUpdateTab::BoardSelectFill_Signal()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void CUpdateTab::CmdFinished_Signal()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void CUpdateTab::DeviceSignalStatus_Signal(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void CUpdateTab::CmdLog_Signal(unsigned char _t1, unsigned char _t2, QString _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void CUpdateTab::CheckSendedOneCmd_Signal(const QString _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
QT_END_MOC_NAMESPACE
