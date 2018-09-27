#include "stdafx.h"
#include "adjustTab.h"

#include <QtGui>


#include "../../../Common/Function/Function.h"
#include "../../../Common/Define/DefineFile.h"

#include "updateTab.h"

// --------------------------------------------------------------------------------------
extern st_PageCheck g_oPageCheck;   // 功能权限

// --------------------------------------------------------------------------------------
// 设备频率校准 调整等 tab
// --------------------------------------------------------------------------------------
CMyAdjustTab::CMyAdjustTab(QWidget* parent) : QWidget(parent)
{
    m_pComNetTab = NULL;
    m_bPswIsOK = false;
    m_bButtonEnabled = false;

    m_pRefStaInput_Buf = new char[SIZE_OF_INPUT_BUF];

    m_pSignalMapper = new QSignalMapper(this);
    connect(m_pSignalMapper, SIGNAL(mapped(int)), this, SLOT(AdjustMapFunc(int)));

    connect(this, SIGNAL(ShowInfo_Signal(const QString&, QColor)), this, SLOT(ShowInfo_Slot(const QString&, QColor)));
    connect(this, SIGNAL(HandleFrame_Signal(const QString&, unsigned char, unsigned char)), this, SLOT(HandleFrame_Slot(const QString&, unsigned char, unsigned char)));

    CreateWidget();

    SetLockFunc(g_oPageCheck.IsAdmin());

}
CMyAdjustTab::~CMyAdjustTab()
{
    delete[] m_pRefStaInput_Buf;
}


void CMyAdjustTab::SetComNetTab(CUpdateTab* pComNetTab)
{
    m_pComNetTab = pComNetTab;
}

void CMyAdjustTab::paintEvent(QPaintEvent*)
{
    // Style Sheet
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CMyAdjustTab::CreateWidget()
{
    QVBoxLayout* pVLayout1 = new QVBoxLayout();

    //pVLayout1->addWidget(CreatePswLockGroup());
    pVLayout1->addWidget(CreateFreqAdjust());
    pVLayout1->addStretch();
    pVLayout1->addWidget(CreateClearOp());

    // Info Show
    m_pInfoShow_TextEdit = new QTextEdit();
    m_pInfoShow_TextEdit->setReadOnly(true);

    QVBoxLayout* pVLayout2 = new QVBoxLayout();
    pVLayout2->addWidget(m_pInfoShow_TextEdit);


    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addLayout(pVLayout1);
    pHLayout->addLayout(pVLayout2);

    setLayout(pHLayout);
}

QGroupBox* CMyAdjustTab::CreatePswLockGroup()
{
    QGroupBox* pGroup = new QGroupBox(tr("密码保护"));

    QLabel* pLabel = new QLabel(tr("操作密码"));
    m_pPswLock_LineEdit = new QLineEdit();
    m_pPswLock_LineEdit->setEchoMode(QLineEdit::Password);
    m_pPswLock_Button = new QPushButton(tr("确认"));

    connect(m_pPswLock_Button, SIGNAL(clicked()), this, SLOT(PswLockFunc()));

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(pLabel);
    pHLayout->addWidget(m_pPswLock_LineEdit);
    pHLayout->addStretch();
    pHLayout->addWidget(m_pPswLock_Button);

    pGroup->setLayout(pHLayout);
    pGroup->hide();
    return pGroup;
}

QGroupBox* CMyAdjustTab::CreateFreqAdjust()
{
    QGroupBox* pGroup = new QGroupBox("本地频率校准");

    QLabel* pLabel21 = new QLabel(tr("校准总量(uHz)"));
    QLabel* pLabel22 = new QLabel(tr("增减校准量(uHz)"));

    m_pLocalFreqAdjTotal_LineEdit = new QLineEdit();
    m_pLocalFreqAdjTotal_LineEdit->setReadOnly(true);
    m_pLocalFreqAdjDelta_LineEdit = new QLineEdit();
    m_pLocalFreqAdjDelta_LineEdit->setText("0");

    m_pLocalFreqAdjAdd_Button = new QPushButton(tr("增"));
    m_pLocalFreqAdjSub_Button = new QPushButton(tr("减"));
    m_pLocalFreqAdjClear_Button = new QPushButton(tr("清零"));
    m_pLocalFreqAdjSave_Button = new QPushButton(tr("保存"));

    m_pLocalFreqAdjAdd_Button->setEnabled(false);
    m_pLocalFreqAdjSub_Button->setEnabled(false);
    m_pLocalFreqAdjClear_Button->setEnabled(false);
    m_pLocalFreqAdjSave_Button->setEnabled(false);

    // Map
    connect(m_pLocalFreqAdjAdd_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pLocalFreqAdjAdd_Button, ID_OF_LOCALADJ_FREQ_ADD);
    connect(m_pLocalFreqAdjSub_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pLocalFreqAdjSub_Button, ID_OF_LOCALADJ_FREQ_SUB);
    connect(m_pLocalFreqAdjClear_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pLocalFreqAdjClear_Button, ID_OF_LOCALADJ_FREQ_CLEAR);
    connect(m_pLocalFreqAdjSave_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pLocalFreqAdjSave_Button, ID_OF_LOCALADJ_FREQ_SAVE);

    // Reg
    QRegExp oReg1("^[0-9]{1,7}$");
    QRegExpValidator* pValid1 = new QRegExpValidator(oReg1, this);
    m_pLocalFreqAdjDelta_LineEdit->setValidator(pValid1);

    QGridLayout* pGrid = new QGridLayout();
    pGrid->addWidget(pLabel21, 0, 0);
    pGrid->addWidget(pLabel22, 0, 1);
    pGrid->addWidget(m_pLocalFreqAdjTotal_LineEdit, 1, 0);
    pGrid->addWidget(m_pLocalFreqAdjDelta_LineEdit, 1, 1);
    pGrid->addWidget(m_pLocalFreqAdjAdd_Button, 1, 2);
    pGrid->addWidget(m_pLocalFreqAdjSub_Button, 1, 3);
    pGrid->addWidget(m_pLocalFreqAdjClear_Button, 1, 4);
    pGrid->addWidget(m_pLocalFreqAdjSave_Button, 1, 5);

    pGroup->setLayout(pGrid);
    return pGroup;
}

QGroupBox* CMyAdjustTab::CreateClearOp()
{
    QGroupBox* pGroup = new QGroupBox("");

    m_pClearInfo_Button = new QPushButton(tr("清 屏"));
    m_pClearInfo_Button->setMinimumSize(QSize(150, 50));

    connect(m_pClearInfo_Button, SIGNAL(clicked()), this, SLOT(ClearInfoFunc()));

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(m_pClearInfo_Button);
    pHLayout->addStretch();

    pGroup->setLayout(pHLayout);
    return pGroup;
}

// 设置按钮状态(针对端口是否打开)
void CMyAdjustTab::SetFuncButtonEnable(bool bEnable)
{
    m_bButtonEnabled = bEnable;

    if(true == bEnable)
    {
        if(false == m_bPswIsOK) bEnable = false;
    }
    m_pLocalFreqAdjAdd_Button->setEnabled(bEnable);
    m_pLocalFreqAdjSub_Button->setEnabled(bEnable);
    m_pLocalFreqAdjClear_Button->setEnabled(bEnable);
    m_pLocalFreqAdjSave_Button->setEnabled(bEnable);
}

// 处理数据
void CMyAdjustTab::HandleFrameData(unsigned char chFrameCmd, const char* pFrameDataBuf, int iFrameDataLength,
                                 unsigned char chSourceAddr, unsigned char chSourcePort)
{
    QString strData = pFrameDataBuf;
    emit HandleFrame_Signal(strData, chSourceAddr, chSourcePort);
}
void CMyAdjustTab::HandleFrame_Slot(const QString& strData, unsigned char chSourceAddr, unsigned char chSourcePort)
{
    strcpy_s(m_pRefStaInput_Buf, SIZE_OF_INPUT_BUF, strData.toLatin1().data());

    char* ptr = NULL;
    char* ptrValue = NULL;

    if(NULL != strstr(m_pRefStaInput_Buf, "freq_add"))
    {
        ShowInfo_Func(m_pRefStaInput_Buf);

        // freq_add 100Hz, total 26Hz(有可能是负数)
        ptrValue = strstr(m_pRefStaInput_Buf, "freq_add");
        if(NULL != ptrValue)
        {
            while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
            ptr = ptrValue;
            while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
            *ptr = '\0';
            ++ptr;

            int iHereIndex = atoi(ptrValue);
            int iHereData = 0;
            ptrValue = strstr(ptr, "total");
            if(NULL != ptrValue)
            {
                while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
                ptr = ptrValue;
                while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
                *ptr = '\0';
                if('-' == *(ptrValue-1)) --ptrValue;
                iHereData = atoi(ptrValue);
            }

            m_pLocalFreqAdjTotal_LineEdit->setText(QString::number(iHereData));
        }
    }

    
}


// 通信映射功能
void CMyAdjustTab::AdjustMapFunc(int iID)
{
    char chData[1024] = {0};
    QString strData;
    switch(iID)
    {
    case ID_OF_LOCALADJ_FREQ_ADD:
        strData = m_pLocalFreqAdjDelta_LineEdit->text();
        if(!strData.isEmpty())
        {
            sprintf_s(chData, sizeof(chData), "freq_add %d", strData.toInt());
            m_pComNetTab->SendChangeData(0x80, 0x00, chData);
        }
        break;
    case ID_OF_LOCALADJ_FREQ_SUB:
        strData = m_pLocalFreqAdjDelta_LineEdit->text();
        if(!strData.isEmpty())
        {
            sprintf_s(chData, sizeof(chData), "freq_add -%d", strData.toInt());
            m_pComNetTab->SendChangeData(0x80, 0x00, chData);
        }
        break;
    case ID_OF_LOCALADJ_FREQ_CLEAR:
        strData = m_pLocalFreqAdjTotal_LineEdit->text();
        if(!strData.isEmpty())
        {
            int iTotal = strData.toInt();
            sprintf_s(chData, sizeof(chData), "freq_add %d", -iTotal);
            m_pComNetTab->SendChangeData(0x80, 0x00, chData);
            m_pLocalFreqAdjDelta_LineEdit->setText("0");
        }
        break;
    case ID_OF_LOCALADJ_FREQ_SAVE:
        strData = m_pLocalFreqAdjTotal_LineEdit->text();
        if(!strData.isEmpty())
        {
            sprintf_s(chData, sizeof(chData), "save_add %d", strData.toInt());
            m_pComNetTab->SendChangeData(0x80, 0x00, chData);
        }
        break;
    default:
        break;
    }

    // 保存日志
    if((strlen(chData) > 0) && (NULL != m_pComNetTab))
    {
        QString strLog = chData;
        m_pComNetTab->CmdLog_Func(0x80, 0x00, strLog);
    }
}

void CMyAdjustTab::ShowInfo_Func(const QString& strInfo, QColor rgb)
{
    emit ShowInfo_Signal(strInfo, rgb);
}
void CMyAdjustTab::ShowInfo_Slot(const QString& strInfo, QColor rgb)
{
    static bool stbColor = false;

    int iR = 0;
    int iG = 0;
    int iB = 0;
    rgb.getRgb(&iR, &iG, &iB);

    if((0 == iR) && (0 == iG) && (0 == iB))
    {
        stbColor = !stbColor;
        if(true == stbColor)
        {
            m_pInfoShow_TextEdit->setTextColor(QColor(0, 0, 200));
        }
        else
        {
            m_pInfoShow_TextEdit->setTextColor(QColor(0, 0, 0));
        }
    }
    else
    {
        m_pInfoShow_TextEdit->setTextColor(rgb);
    }

    m_pInfoShow_TextEdit->append(strInfo);

    QTextCursor oCursor = m_pInfoShow_TextEdit->textCursor();
    oCursor.movePosition(QTextCursor::End);
    m_pInfoShow_TextEdit->setTextCursor(oCursor);
}
void CMyAdjustTab::ClearInfoFunc()
{
    m_pInfoShow_TextEdit->clear();
}

void CMyAdjustTab::PswLockFunc()
{
    QString strPsw = m_pPswLock_LineEdit->text();
    if(!strPsw.isEmpty())
    {
        if("navtfadjust" == strPsw)
        {
            m_bPswIsOK = true;
            ShowInfo_Func(tr("操作密码输入正确"));
        }
        else
        {
            m_bPswIsOK = false;
            ShowInfo_Func(tr("操作密码输入错误，本页功能不能使用"));
        }
        SetFuncButtonEnable(m_bButtonEnabled);
        m_pPswLock_LineEdit->setText("");
    }
}

// 是否锁定
void CMyAdjustTab::SetLockFunc(bool bEnabled)
{
    m_bPswIsOK = bEnabled;
    SetFuncButtonEnable(m_bButtonEnabled);
}

void CMyAdjustTab::SetAdjustAll(char* pszAll)
{
    m_pLocalFreqAdjTotal_LineEdit->setText(pszAll);
}

