#include "stdafx.h"
#include "imageTab.h"

#include <QtGui>

#include <Windows.h>        // fro Beep()

#include "../../../Common/Function/Function.h"
#include "../../../Common/Define/DefineFile.h"

#include "updateTab.h"
#include "myLabel.h"


// --------------------------------------------------------------------------------------
extern st_PageCheck g_oPageCheck;           // 功能权限

extern bool g_bShowBoardTip;                // 屏蔽“所有的板卡无信号输出提示” 输出

// --------------------------------------------------------------------------------------
char g_chTopSigName[5][64] = 
{
    "内频标10MHz/正弦波",
    "外频标10MHz/正弦波",
    "外频标10.23MHz/正弦波",
    "外频标10.23MHz/LVDS",
    ""
};
char g_chTopSigName_2[5][64] =          // 名称不一样
{
    "内频标",
    "参考输入 1",
    "参考输入 2",
    "参考输入 3",
    ""
};
char g_chBottomSigName[5][64] = 
{
    "1PPS_TTL/LVTTL",
    "1PPS_LVDS",
    "",
    "",
    ""
};

// --------------------------------------------------------------------------------------
// 可视操作 tab
// --------------------------------------------------------------------------------------
CMyImageTab::CMyImageTab(QWidget* parent) : QWidget(parent)
{
    m_pComNetTab = NULL;
    m_pMyLabel = NULL;

    m_enFuncCmd = en_None_Cmd;
    m_bComNetCanUse = false;

    m_pOriFont = new QFont();
    *m_pOriFont = font();

    m_pUseFont = new QFont();

    m_iBeepDiscSec = 0;
    m_iBeepTimes = 0;

    m_iReduceVal_6 = 0;
    m_iReduceVal_5 = 0;
    m_iReduceVal_4 = 0;
    m_iReduceVal_3 = 0;

    m_iReduceVal_26 = 0;
    m_iReduceVal_25 = 0;
    m_iReduceVal_24 = 0;
    m_iReduceVal_23 = 0;

    m_iPriority1_Cur = 0;
    m_iPriority2_Cur = 0;
    m_iPriority3_Cur = 0;
    m_iPriority4_Cur = 0;
    m_iPriority5_Cur = 0;
    m_iPriority6_Cur = 0;
    m_iPriorityType_Cur = 0;

    m_iPriority1_New = 0;
    m_iPriority2_New = 0;
    m_iPriority3_New = 0;
    m_iPriority4_New = 0;
    m_iPriority5_New = 0;
    m_iPriority6_New = 0;
    m_iPriorityType_New = 0;

    m_iPrioSetTimeout_Count = 0;
    m_bQueryPriority = false;
    m_iOutputClk_Count = 0;

    m_pSignalMapper = new QSignalMapper(this);
    connect(m_pSignalMapper, SIGNAL(mapped(int)), this, SLOT(ComNetFunc(int)));

    connect(this, SIGNAL(ShowPrioInfo_Signal(const QString&, bool)), this, SLOT(ShowPrioInfo_Slot(const QString&, bool)));
    connect(this, SIGNAL(HandleFrame_Signal(const QString&, unsigned char, unsigned char)), this, SLOT(HandleFrame_Slot(const QString&, unsigned char, unsigned char)));
    connect(this, SIGNAL(ShowLock_Signal(int,int,int,int, int,int,int,int)), this, SLOT(ShowLock_Slot(int,int,int,int, int,int,int,int)));

    m_pRefStaInput_Buf = new char[SIZE_OF_INPUT_BUF];

    CreateWidget();
    SetFuncButtonEnable(false);

    m_iPrioInfoHide_Count = 0;
    m_iPrioCmdTimeout_Count = 0;
    m_pTimer = new QTimer();
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(TimerReach()));
    m_pTimer->start(1000);
}
CMyImageTab::~CMyImageTab()
{
    delete m_pOriFont;
    delete m_pUseFont;

    delete[] m_pRefStaInput_Buf;
}

void CMyImageTab::SetComNetTab(CUpdateTab* pComNetTab)
{
    m_pComNetTab = pComNetTab;

    if(NULL != m_pMyLabel)
    {
        m_pMyLabel->SetUsePtr(pComNetTab, this);
    }
}

void CMyImageTab::TimerReach()
{
    --m_iPrioInfoHide_Count;
    if(m_iPrioInfoHide_Count <= 0)
    {
        m_pPriorityInfoShow_Label->setText("");
    }

    if(m_iPrioCmdTimeout_Count > 0)
    {
        --m_iPrioCmdTimeout_Count;
        if(m_iPrioCmdTimeout_Count == 0)
        {
            m_bQueryPriority = false;
            ShowPrioInfo_Func(tr("超时，无回复"), true);

            SetPriorityCtrl(true);
        }
    }

    if(m_iPrioSetTimeout_Count > 0)
    {
        --m_iPrioSetTimeout_Count;
        if(0 == m_iPrioSetTimeout_Count)
        {
            if((m_iPriority1_Cur != m_iPriority1_New) || (m_iPriority2_Cur != m_iPriority2_New) ||
               (m_iPriority3_Cur != m_iPriority3_New) || (m_iPriority4_Cur != m_iPriority4_New) ||
               (m_iPriority5_Cur != m_iPriority5_New) || (m_iPriority6_Cur != m_iPriority6_New) || (m_iPriorityType_Cur != m_iPriorityType_New))
            {
                SetPriorityCtrl(true);
                ShowPrioInfo_Func(tr("优先级设置 成功"), false);
            }
            else
            {
                // 如果一样，提示并置使能
                SetPriorityCtrl(true);
                ShowPrioInfo_Func(tr("设置的优先级与设备一样，没改变"), false);
            }
        }
    }
    if(0 == m_iPrioSetTimeout_Count)
    {
        if((m_iPriority1_Cur != m_iPriority1_New) || (m_iPriority2_Cur != m_iPriority2_New) ||
           (m_iPriority3_Cur != m_iPriority3_New) || (m_iPriority4_Cur != m_iPriority4_New) ||
           (m_iPriority5_Cur != m_iPriority5_New) || (m_iPriority6_Cur != m_iPriority6_New) || (m_iPriorityType_Cur != m_iPriorityType_New))
        {
            m_pPriority1_ComboBox->setCurrentIndex(m_iPriority1_New-1);
            m_pPriority2_ComboBox->setCurrentIndex(m_iPriority2_New-1);
            m_pPriority3_ComboBox->setCurrentIndex(m_iPriority3_New-1);
            m_pPriority4_ComboBox->setCurrentIndex(m_iPriority4_New-1);

            m_pPriority5_ComboBox->setCurrentIndex(m_iPriority5_New-1);
            m_pPriority6_ComboBox->setCurrentIndex(m_iPriority6_New-1);

            m_pPriorityType_ComboBox->setCurrentIndex(m_iPriorityType_New-1);

            m_iPriority1_Cur = m_iPriority1_New;
            m_iPriority2_Cur = m_iPriority2_New;
            m_iPriority3_Cur = m_iPriority3_New;
            m_iPriority4_Cur = m_iPriority4_New;
            m_iPriority5_Cur = m_iPriority5_New;
            m_iPriority6_Cur = m_iPriority6_New;
            m_iPriorityType_Cur = m_iPriorityType_New;
        }
    }

    // Beep
    if(m_iBeepDiscSec > 0)
    {
        --m_iBeepDiscSec;
        if(0 == m_iBeepDiscSec)
        {
            if(m_iBeepTimes > 0)
            {
                --m_iBeepTimes;
                if(m_iBeepTimes > 0) m_iBeepDiscSec = 1;
                Beep(2000, 100);
            }
        }
    }

    // 10s 检测板卡是否有输出
    if(true == m_pComNetTab->ComNetIsOpen())
    {
        ++m_iOutputClk_Count;
        if(m_iOutputClk_Count >= 10)
        {
            m_iOutputClk_Count = 0;
            QString strInfo = tr("所有的板卡无信号输出提示\n");
            if(true == g_bShowBoardTip) m_pComNetTab->ShowRecvDataFunc(0x60, 0, strInfo.toLatin1().data(), false, 250, 0, 0);
        }
    }
}

void CMyImageTab::ShowPrioInfo_Func(const QString& strInfo, bool bIsError)
{
    emit ShowPrioInfo_Signal(strInfo, bIsError);
}
void CMyImageTab::ShowPrioInfo_Slot(const QString& strInfo, bool bIsError)
{
    if(true == bIsError)
    {
        m_pPriorityInfoShow_Label->setStyleSheet(tr("color: rgb(150, 0, 0)"));
    }
    else
    {
        m_pPriorityInfoShow_Label->setStyleSheet(tr("color: rgb(0, 50, 150)"));
    }
    m_pPriorityInfoShow_Label->setText(strInfo);
    m_iPrioInfoHide_Count = 10;
}

void CMyImageTab::CreateWidget()
{
    QVBoxLayout* pVLayout = new QVBoxLayout();

    m_pImageGroup = CreateImageGroup();
    QHBoxLayout* pIHLayout = new QHBoxLayout();
    pIHLayout->addWidget(m_pImageGroup, 1);

    QGroupBox* pPriorityGroup = CreatePriorityGroup();
    QGroupBox* pEightGroup = CreateEightGroup();

    m_pPriorityInfoShow_Label = new QLabel(tr(""));
    m_pPriorityInfoShow_Label->setAlignment(Qt::AlignCenter);

    if(false == g_oPageCheck.SignalTab_Priority_Use())
    {
        // Hide
        pPriorityGroup->hide();
        m_pPriorityInfoShow_Label->hide();
    }
    else
    {
        QVBoxLayout* pRightVLayout = new QVBoxLayout();
        pRightVLayout->addWidget(pPriorityGroup);
        pRightVLayout->addStretch();
        pRightVLayout->addWidget(m_pPriorityInfoShow_Label);
        pRightVLayout->addStretch();

        pIHLayout->addLayout(pRightVLayout);
    }
    if(false == g_oPageCheck.SignalTab_InputCtrl_Use())
    {
        pVLayout->addLayout(pIHLayout, 1);
        pVLayout->addLayout(CreateShowGroup());

        // Hide
        CreateEightGroup()->hide();
        QGroupBox* pHideGroup = new QGroupBox();
        pHideGroup->setLayout(CreateChangeGroup());
        pHideGroup->hide();
    }
    else
    {
        QVBoxLayout* pVLayoutBt = new QVBoxLayout();
        pVLayoutBt->addLayout(CreateChangeGroup());
        pVLayoutBt->addLayout(CreateShowGroup());

        QHBoxLayout* pHLayoutBt = new QHBoxLayout();
        pHLayoutBt->addLayout(pVLayoutBt, 1);
        pHLayoutBt->addWidget(CreateEightGroup());

        pVLayout->addLayout(pIHLayout, 1);
        pVLayout->addLayout(pHLayoutBt);
    }

    setLayout(pVLayout);
}

QGroupBox* CMyImageTab::CreateImageGroup()
{
    QGroupBox* pGroup = new QGroupBox(tr("连接示意图"));
    pGroup->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    m_pMyLabel = new CMyLabel(this);

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(m_pMyLabel);

    pGroup->setLayout(pHLayout);
    
    return pGroup;
}
QGroupBox* CMyImageTab::CreatePriorityGroup()
{
    QGroupBox* pGroup = new QGroupBox(tr("优先级设置"));
    m_pPriority_Group = pGroup;

    int iMinWidth = 120;
    int iMaxWidthCom = 100;
    int iMinWidthCom = 100;

    // 频率
    QGroupBox* pGroupTop = new QGroupBox();

    QLabel* pLabel1 = new QLabel(tr("内频标10M_正弦波"));
    m_pPriority1_ComboBox = new QComboBox();
    pLabel1->setMinimumWidth(iMinWidth);
    m_pPriority1_ComboBox->setMaximumWidth(iMaxWidthCom);
    m_pPriority1_ComboBox->setMinimumWidth(iMinWidthCom);
    m_pPriority1_ComboBox->addItem(tr("0_最高"));
    m_pPriority1_ComboBox->addItem(tr("1_次高"));
    m_pPriority1_ComboBox->addItem(tr("2_次低"));
    m_pPriority1_ComboBox->addItem(tr("3_最低"));
    m_pPriority1_ComboBox->setCurrentIndex(0);
    QHBoxLayout* pHLayout1 = new QHBoxLayout();
    pHLayout1->addWidget(pLabel1);
    pHLayout1->addWidget(m_pPriority1_ComboBox);

    QLabel* pLabel2 = new QLabel(tr("外频标10M_正弦波"));
    m_pPriority2_ComboBox = new QComboBox();
    pLabel2->setMinimumWidth(iMinWidth);
    m_pPriority2_ComboBox->setMaximumWidth(iMaxWidthCom);
    m_pPriority2_ComboBox->setMinimumWidth(iMinWidthCom);
    m_pPriority2_ComboBox->addItem(tr("0_最高"));
    m_pPriority2_ComboBox->addItem(tr("1_次高"));
    m_pPriority2_ComboBox->addItem(tr("2_次低"));
    m_pPriority2_ComboBox->addItem(tr("3_最低"));
    m_pPriority2_ComboBox->setCurrentIndex(1);
    QHBoxLayout* pHLayout2 = new QHBoxLayout();
    pHLayout2->addWidget(pLabel2);
    pHLayout2->addWidget(m_pPriority2_ComboBox);

    QLabel* pLabel3 = new QLabel(tr("外频10.23M_正弦波"));
    m_pPriority3_ComboBox = new QComboBox();
    pLabel3->setMinimumWidth(iMinWidth);
    m_pPriority3_ComboBox->setMaximumWidth(iMaxWidthCom);
    m_pPriority3_ComboBox->setMinimumWidth(iMinWidthCom);
    m_pPriority3_ComboBox->addItem(tr("0_最高"));
    m_pPriority3_ComboBox->addItem(tr("1_次高"));
    m_pPriority3_ComboBox->addItem(tr("2_次低"));
    m_pPriority3_ComboBox->addItem(tr("3_最低"));
    m_pPriority3_ComboBox->setCurrentIndex(2);
    QHBoxLayout* pHLayout3 = new QHBoxLayout();
    pHLayout3->addWidget(pLabel3);
    pHLayout3->addWidget(m_pPriority3_ComboBox);

    QLabel* pLabel4 = new QLabel(tr("外频标10.23M_LVDS"));
    m_pPriority4_ComboBox = new QComboBox();
    pLabel4->setMinimumWidth(iMinWidth);
    m_pPriority4_ComboBox->setMaximumWidth(iMaxWidthCom);
    m_pPriority4_ComboBox->setMinimumWidth(iMinWidthCom);
    m_pPriority4_ComboBox->addItem(tr("0_最高"));
    m_pPriority4_ComboBox->addItem(tr("1_次高"));
    m_pPriority4_ComboBox->addItem(tr("2_次低"));
    m_pPriority4_ComboBox->addItem(tr("3_最低"));
    m_pPriority4_ComboBox->setCurrentIndex(3);
    
    QHBoxLayout* pHLayout4 = new QHBoxLayout();
    pHLayout4->addWidget(pLabel4);
    pHLayout4->addWidget(m_pPriority4_ComboBox);

    QVBoxLayout* pVLayoutTop = new QVBoxLayout();
    pVLayoutTop->addLayout(pHLayout1);
    pVLayoutTop->addLayout(pHLayout2);
    pVLayoutTop->addLayout(pHLayout3);
    pVLayoutTop->addLayout(pHLayout4);
    pGroupTop->setLayout(pVLayoutTop);

    // 脉冲
    QGroupBox* pGroupBottom = new QGroupBox();

    QLabel* pLabel5 = new QLabel(tr("1PPS_TTL"));
    m_pPriority5_ComboBox = new QComboBox();
    pLabel5->setMinimumWidth(iMinWidth);
    m_pPriority5_ComboBox->setMaximumWidth(iMaxWidthCom);
    m_pPriority5_ComboBox->setMinimumWidth(iMinWidthCom);
    m_pPriority5_ComboBox->addItem(tr("0_最高"));
    m_pPriority5_ComboBox->addItem(tr("1_最低"));
    m_pPriority5_ComboBox->setCurrentIndex(0);
    QHBoxLayout* pHLayout5 = new QHBoxLayout();
    pHLayout5->addWidget(pLabel5);
    pHLayout5->addWidget(m_pPriority5_ComboBox);

    QLabel* pLabel6 = new QLabel(tr("1PPS_LVDS"));
    m_pPriority6_ComboBox = new QComboBox();
    pLabel6->setMinimumWidth(iMinWidth);
    m_pPriority6_ComboBox->setMaximumWidth(iMaxWidthCom);
    m_pPriority6_ComboBox->setMinimumWidth(iMinWidthCom);
    m_pPriority6_ComboBox->addItem(tr("0_最高"));
    m_pPriority6_ComboBox->addItem(tr("1_最低"));
    m_pPriority6_ComboBox->setCurrentIndex(1);
    QHBoxLayout* pHLayout6 = new QHBoxLayout();
    pHLayout6->addWidget(pLabel6);
    pHLayout6->addWidget(m_pPriority6_ComboBox);

    QVBoxLayout* pVLayoutBottom = new QVBoxLayout();
    pVLayoutBottom->addLayout(pHLayout5);
    pVLayoutBottom->addLayout(pHLayout6);
    pGroupBottom->setLayout(pVLayoutBottom);

    // 切换方式
    QGroupBox* pGroupType = new QGroupBox();
    QLabel* pLabelType = new QLabel(tr("切换方式"));
    m_pPriorityType_ComboBox = new QComboBox();
    pLabelType->setMinimumWidth(iMinWidth);
    m_pPriorityType_ComboBox->addItem(tr("手动切换"));
    m_pPriorityType_ComboBox->addItem(tr("自动-高优先"));
    m_pPriorityType_ComboBox->addItem(tr("自动-当前优先"));

    QHBoxLayout* pHLayoutType = new QHBoxLayout();
    pHLayoutType->addWidget(pLabelType);
    pHLayoutType->addWidget(m_pPriorityType_ComboBox);
    pGroupType->setLayout(pHLayoutType);

    // Save
    m_iPriority1_Cur = m_pPriority1_ComboBox->currentIndex() + 1;
    m_iPriority2_Cur = m_pPriority2_ComboBox->currentIndex() + 1;
    m_iPriority3_Cur = m_pPriority3_ComboBox->currentIndex() + 1;
    m_iPriority4_Cur = m_pPriority4_ComboBox->currentIndex() + 1;

    m_iPriority5_Cur = m_pPriority5_ComboBox->currentIndex() + 1;
    m_iPriority6_Cur = m_pPriority6_ComboBox->currentIndex() + 1;

    m_iPriorityType_Cur = m_pPriorityType_ComboBox->currentIndex() + 1;

    m_iPriority1_New = m_iPriority1_Cur;
    m_iPriority2_New = m_iPriority2_Cur;
    m_iPriority3_New = m_iPriority3_Cur;
    m_iPriority4_New = m_iPriority4_Cur;
    m_iPriority5_New = m_iPriority5_Cur;
    m_iPriority6_New = m_iPriority6_Cur;
    m_iPriorityType_New = m_iPriorityType_Cur;

    // 设置
    QGroupBox* pGroupConfig = new QGroupBox();

    m_pPriorityQuery_Button = new QPushButton(tr("查询"));
    m_pPrioritySet_Button = new QPushButton(tr("设置"));
    m_pPriorityQuery_Button->setEnabled(false);
    m_pPrioritySet_Button->setEnabled(false);

    connect(m_pPriorityQuery_Button, SIGNAL(clicked()), this, SLOT(PriorityQuery_Slot()));
    connect(m_pPrioritySet_Button, SIGNAL(clicked()), this, SLOT(PrioritySet_Slot()));

    QHBoxLayout* pBtHLayout2 = new QHBoxLayout();
    pBtHLayout2->addWidget(m_pPriorityQuery_Button);
    pBtHLayout2->addWidget(m_pPrioritySet_Button);
    pGroupConfig->setLayout(pBtHLayout2);

    // Group
    QVBoxLayout* pVLayout = new QVBoxLayout();
    pVLayout->addWidget(pGroupTop);
    pVLayout->addWidget(pGroupBottom);
    pVLayout->addWidget(pGroupType);
    pVLayout->addWidget(pGroupConfig);
    pGroup->setLayout(pVLayout);
    return pGroup;
}
QGroupBox* CMyImageTab::CreateEightGroup()
{
    QGroupBox* pGroup = new QGroupBox(tr("正弦波时钟分配板输出幅度(-5~10dbm)"));
    m_pEight_Group = pGroup;

    int iMaxWidth = 60;
    QLabel* pLabel1 = new QLabel(tr("板卡[6, 0]"));
    m_pReduceShow_6_LineEdit = new QLineEdit();
    m_pReduceShow_6_LineEdit->setReadOnly(true);
    m_pReduceShow_6_LineEdit->setMaximumWidth(iMaxWidth);
    m_pReduceAdd_6_Button = new QPushButton(tr("增"));
    m_pReduceSub_6_Button = new QPushButton(tr("减"));
    QLabel* pLabel2 = new QLabel(tr("板卡[5, 0]"));
    m_pReduceShow_5_LineEdit = new QLineEdit();
    m_pReduceShow_5_LineEdit->setReadOnly(true);
    m_pReduceShow_5_LineEdit->setMaximumWidth(iMaxWidth);
    m_pReduceAdd_5_Button = new QPushButton(tr("增"));
    m_pReduceSub_5_Button = new QPushButton(tr("减"));
    QLabel* pLabel3 = new QLabel(tr("板卡[4, 0]"));
    m_pReduceShow_4_LineEdit = new QLineEdit();
    m_pReduceShow_4_LineEdit->setReadOnly(true);
    m_pReduceShow_4_LineEdit->setMaximumWidth(iMaxWidth);
    m_pReduceAdd_4_Button = new QPushButton(tr("增"));
    m_pReduceSub_4_Button = new QPushButton(tr("减"));
    QLabel* pLabel4 = new QLabel(tr("板卡[3, 0]"));
    m_pReduceShow_3_LineEdit = new QLineEdit();
    m_pReduceShow_3_LineEdit->setReadOnly(true);
    m_pReduceShow_3_LineEdit->setMaximumWidth(iMaxWidth);
    m_pReduceAdd_3_Button = new QPushButton(tr("增"));
    m_pReduceSub_3_Button = new QPushButton(tr("减"));

    QLabel* pLabel5 = new QLabel(tr("板卡[26,0]"));
    m_pReduceShow_26_LineEdit = new QLineEdit();
    m_pReduceShow_26_LineEdit->setReadOnly(true);
    m_pReduceShow_26_LineEdit->setMaximumWidth(iMaxWidth);
    m_pReduceAdd_26_Button = new QPushButton(tr("增"));
    m_pReduceSub_26_Button = new QPushButton(tr("减"));
    QLabel* pLabel6 = new QLabel(tr("板卡[25,0]"));
    m_pReduceShow_25_LineEdit = new QLineEdit();
    m_pReduceShow_25_LineEdit->setReadOnly(true);
    m_pReduceShow_25_LineEdit->setMaximumWidth(iMaxWidth);
    m_pReduceAdd_25_Button = new QPushButton(tr("增"));
    m_pReduceSub_25_Button = new QPushButton(tr("减"));
    QLabel* pLabel7 = new QLabel(tr("板卡[24,0]"));
    m_pReduceShow_24_LineEdit = new QLineEdit();
    m_pReduceShow_24_LineEdit->setReadOnly(true);
    m_pReduceShow_24_LineEdit->setMaximumWidth(iMaxWidth);
    m_pReduceAdd_24_Button = new QPushButton(tr("增"));
    m_pReduceSub_24_Button = new QPushButton(tr("减"));
    QLabel* pLabel8 = new QLabel(tr("板卡[23,0]"));
    m_pReduceShow_23_LineEdit = new QLineEdit();
    m_pReduceShow_23_LineEdit->setReadOnly(true);
    m_pReduceShow_23_LineEdit->setMaximumWidth(iMaxWidth);
    m_pReduceAdd_23_Button = new QPushButton(tr("增"));
    m_pReduceSub_23_Button = new QPushButton(tr("减"));

    // 对齐补偿
    QLabel* pAlignDataLabel = new QLabel(tr("对齐补偿  "));
    pAlignDataLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pAlignCompse_LineEdit = new QLineEdit();
    m_pAlignCompse_LineEdit->setReadOnly(true);
    m_pAlignCompse_LineEdit->setMaximumWidth(iMaxWidth);
    m_pAlignCompseAdd_Button = new QPushButton(tr("增"));
    m_pAlignCompseSub_Button = new QPushButton(tr("减"));

    // Map
    connect(m_pReduceAdd_6_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pReduceAdd_6_Button, ID_OF_REDUCE_ADD_6);
    connect(m_pReduceSub_6_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pReduceSub_6_Button, ID_OF_REDUCE_SUB_6);
    connect(m_pReduceAdd_5_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pReduceAdd_5_Button, ID_OF_REDUCE_ADD_5);
    connect(m_pReduceSub_5_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pReduceSub_5_Button, ID_OF_REDUCE_SUB_5);
    connect(m_pReduceAdd_4_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pReduceAdd_4_Button, ID_OF_REDUCE_ADD_4);
    connect(m_pReduceSub_4_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pReduceSub_4_Button, ID_OF_REDUCE_SUB_4);
    connect(m_pReduceAdd_3_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pReduceAdd_3_Button, ID_OF_REDUCE_ADD_3);
    connect(m_pReduceSub_3_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pReduceSub_3_Button, ID_OF_REDUCE_SUB_3);

    connect(m_pReduceAdd_26_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pReduceAdd_26_Button, ID_OF_REDUCE_ADD_26);
    connect(m_pReduceSub_26_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pReduceSub_26_Button, ID_OF_REDUCE_SUB_26);
    connect(m_pReduceAdd_25_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pReduceAdd_25_Button, ID_OF_REDUCE_ADD_25);
    connect(m_pReduceSub_25_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pReduceSub_25_Button, ID_OF_REDUCE_SUB_25);
    connect(m_pReduceAdd_24_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pReduceAdd_24_Button, ID_OF_REDUCE_ADD_24);
    connect(m_pReduceSub_24_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pReduceSub_24_Button, ID_OF_REDUCE_SUB_24);
    connect(m_pReduceAdd_23_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pReduceAdd_23_Button, ID_OF_REDUCE_ADD_23);
    connect(m_pReduceSub_23_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pReduceSub_23_Button, ID_OF_REDUCE_SUB_23);

    connect(m_pAlignCompseAdd_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pAlignCompseAdd_Button, ID_OF_ALIGNCOMPSE_ADD);
    connect(m_pAlignCompseSub_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pAlignCompseSub_Button, ID_OF_ALIGNCOMPSE_SUB);

    QHBoxLayout* pHLayout1 = new QHBoxLayout();
    pHLayout1->addWidget(pLabel1);
    pHLayout1->addWidget(m_pReduceShow_6_LineEdit);
    pHLayout1->addWidget(m_pReduceAdd_6_Button);
    pHLayout1->addWidget(m_pReduceSub_6_Button);
    QHBoxLayout* pHLayout2 = new QHBoxLayout();
    pHLayout2->addWidget(pLabel2);
    pHLayout2->addWidget(m_pReduceShow_5_LineEdit);
    pHLayout2->addWidget(m_pReduceAdd_5_Button);
    pHLayout2->addWidget(m_pReduceSub_5_Button);
    QHBoxLayout* pHLayout3 = new QHBoxLayout();
    pHLayout3->addWidget(pLabel3);
    pHLayout3->addWidget(m_pReduceShow_4_LineEdit);
    pHLayout3->addWidget(m_pReduceAdd_4_Button);
    pHLayout3->addWidget(m_pReduceSub_4_Button);
    QHBoxLayout* pHLayout4 = new QHBoxLayout();
    pHLayout4->addWidget(pLabel4);
    pHLayout4->addWidget(m_pReduceShow_3_LineEdit);
    pHLayout4->addWidget(m_pReduceAdd_3_Button);
    pHLayout4->addWidget(m_pReduceSub_3_Button);


    QHBoxLayout* pHLayout5 = new QHBoxLayout();
    pHLayout5->addWidget(pLabel5);
    pHLayout5->addWidget(m_pReduceShow_26_LineEdit);
    pHLayout5->addWidget(m_pReduceAdd_26_Button);
    pHLayout5->addWidget(m_pReduceSub_26_Button);
    QHBoxLayout* pHLayout6 = new QHBoxLayout();
    pHLayout6->addWidget(pLabel6);
    pHLayout6->addWidget(m_pReduceShow_25_LineEdit);
    pHLayout6->addWidget(m_pReduceAdd_25_Button);
    pHLayout6->addWidget(m_pReduceSub_25_Button);
    QHBoxLayout* pHLayout7 = new QHBoxLayout();
    pHLayout7->addWidget(pLabel7);
    pHLayout7->addWidget(m_pReduceShow_24_LineEdit);
    pHLayout7->addWidget(m_pReduceAdd_24_Button);
    pHLayout7->addWidget(m_pReduceSub_24_Button);
    QHBoxLayout* pHLayout8 = new QHBoxLayout();
    pHLayout8->addWidget(pLabel8);
    pHLayout8->addWidget(m_pReduceShow_23_LineEdit);
    pHLayout8->addWidget(m_pReduceAdd_23_Button);
    pHLayout8->addWidget(m_pReduceSub_23_Button);

    QHBoxLayout* pHLayout9 = new QHBoxLayout();
    pHLayout9->addWidget(pAlignDataLabel);
    pHLayout9->addWidget(m_pAlignCompse_LineEdit);
    pHLayout9->addWidget(m_pAlignCompseAdd_Button);
    pHLayout9->addWidget(m_pAlignCompseSub_Button);

    QVBoxLayout* pVLayout = new QVBoxLayout();
    pVLayout->addLayout(pHLayout1);
    pVLayout->addLayout(pHLayout2);
    pVLayout->addLayout(pHLayout3);
    pVLayout->addLayout(pHLayout4);
    pVLayout->addLayout(pHLayout5);
    pVLayout->addLayout(pHLayout6);
    pVLayout->addLayout(pHLayout7);
    pVLayout->addLayout(pHLayout8);
    pVLayout->addLayout(pHLayout9);

    pGroup->setLayout(pVLayout);
    return pGroup;
}
QHBoxLayout* CMyImageTab::CreateChangeGroup()
{
    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addLayout(Create10MFreqGroup());
    pHLayout->addLayout(Create10MPhasGroup());
    
    return pHLayout;
}

QHBoxLayout* CMyImageTab::CreateShowGroup()
{
    QGroupBox* pGroup = new QGroupBox();

    // 内频标
    QLabel* pInLabel = new QLabel(tr("内频标"));
    pInLabel->setAlignment(Qt::AlignCenter);
    m_pInFreqVal_Label = new QLabel("");
    m_pInFreqVal_Label->setAlignment(Qt::AlignCenter);
    m_pInFreqPic_Label = new QLabel();
    m_pInFreqPic_Label->setPixmap(QPixmap(":/images/unlock.png"));
    m_pInFreqPic_Label->setAlignment(Qt::AlignCenter);
    QHBoxLayout* pHLayoutIn = new QHBoxLayout();
    pHLayoutIn->addWidget(pInLabel);
    pHLayoutIn->addWidget(m_pInFreqVal_Label);
    pHLayoutIn->addWidget(m_pInFreqPic_Label);

    // 参考频率
    QLabel* p1PPSLabel = new QLabel(tr("参考频率"));
    p1PPSLabel->setAlignment(Qt::AlignCenter);
    m_pRefFreqVal_Label = new QLabel("");
    m_pRefFreqVal_Label->setAlignment(Qt::AlignCenter);
    m_pRefFreqPic_Label = new QLabel();
    m_pRefFreqPic_Label->setPixmap(QPixmap(":/images/unlock.png"));
    m_pRefFreqPic_Label->setAlignment(Qt::AlignCenter);
    QHBoxLayout* pHLayout1PPS = new QHBoxLayout();
    pHLayout1PPS->addWidget(p1PPSLabel);
    pHLayout1PPS->addWidget(m_pRefFreqVal_Label);
    pHLayout1PPS->addWidget(m_pRefFreqPic_Label);

    // 10M
    QLabel* p10MLabel = new QLabel(tr("10M PLL"));
    p10MLabel->setAlignment(Qt::AlignCenter);
    m_p10MFreqVal_Label = new QLabel("");
    m_p10MFreqVal_Label->setAlignment(Qt::AlignCenter);
    m_p10MFreqPic_Label = new QLabel();
    m_p10MFreqPic_Label->setPixmap(QPixmap(":/images/unlock.png"));
    m_p10MFreqPic_Label->setAlignment(Qt::AlignCenter);
    QHBoxLayout* pHLayout10M = new QHBoxLayout();
    pHLayout10M->addWidget(p10MLabel);
    pHLayout10M->addWidget(m_p10MFreqVal_Label);
    pHLayout10M->addWidget(m_p10MFreqPic_Label);

    // 10.23M
    QLabel* p1023MLabel = new QLabel(tr("10.23M PLL"));
    p1023MLabel->setAlignment(Qt::AlignCenter);
    m_p1023MFreqVal_Label = new QLabel("");
    m_p1023MFreqVal_Label->setAlignment(Qt::AlignCenter);
    m_p1023MFreqPic_Label = new QLabel();
    m_p1023MFreqPic_Label->setPixmap(QPixmap(":/images/unlock.png"));
    m_p1023MFreqPic_Label->setAlignment(Qt::AlignCenter);
    QHBoxLayout* pHLayout1023M = new QHBoxLayout();
    pHLayout1023M->addWidget(p1023MLabel);
    pHLayout1023M->addWidget(m_p1023MFreqVal_Label);
    pHLayout1023M->addWidget(m_p1023MFreqPic_Label);


    // New
    QHBoxLayout* pHLayoutNew = new QHBoxLayout();
    QVBoxLayout* pVLayoutFreq = new QVBoxLayout();
    pVLayoutFreq->addLayout(pHLayoutIn);
    pVLayoutFreq->addLayout(pHLayout1PPS);
    pVLayoutFreq->addLayout(pHLayout10M);
    pVLayoutFreq->addLayout(pHLayout1023M);
    pGroup->setLayout(pVLayoutFreq);
    pHLayoutNew->addWidget(pGroup);

    // 权限
    if(true == g_oPageCheck.SignalTab_InputCtrl_Use())
    {
        QHBoxLayout* pHLayout = new QHBoxLayout();
        pHLayout->addLayout(pHLayoutNew, 1);
        pHLayout->addLayout(CreatePhaseDelayGroup(), 1);

        return pHLayout;
    }
    else
    {
        // Hide
        QGroupBox* pHideGroup = new QGroupBox();
        pHideGroup->setLayout(CreatePhaseDelayGroup());
        pHideGroup->hide();

        return pHLayoutNew;
    }
}

QVBoxLayout* CMyImageTab::Create10MFreqGroup()
{
    QGroupBox* pFreqGroup = new QGroupBox(tr("10M DDS频率微调(调节范围-2000000~2000000uHz，步进10uHz，默认值0)"));
    QGroupBox* pPhasGroup = new QGroupBox(tr("10M DDS相位偏移(调节范围-100~100ns, 步进12.5ns，默认值0)"));

    // 1
    QLabel* pLabel11 = new QLabel(tr("调节总量uHz"));
    QLabel* pLabel12 = new QLabel(tr("增减调节量uHz"));

    m_p10MFreqTotao1_LineEdit = new QLineEdit();
    m_p10MFreqTotao1_LineEdit->setReadOnly(true);
    m_p10MFreqDelta1_LineEdit = new QLineEdit();

    m_p10MFreqAdd1_Button = new QPushButton(tr("增"));
    m_p10MFreqSub1_Button = new QPushButton(tr("减"));
    m_p10MFreqClear1_Button = new QPushButton(tr("清零"));
    m_p10MFreqSave1_Button = new QPushButton(tr("保存"));

    connect(m_p10MFreqAdd1_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_p10MFreqAdd1_Button, ID_OF_10M_FREQ_ADD);
    connect(m_p10MFreqSub1_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_p10MFreqSub1_Button, ID_OF_10M_FREQ_SUB);
    connect(m_p10MFreqSave1_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_p10MFreqSave1_Button, ID_OF_10M_FREQ_SAVE);
    connect(m_p10MFreqClear1_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_p10MFreqClear1_Button, ID_OF_10M_FREQ_CLEAR);

    QGridLayout* pGrid1 = new QGridLayout();
    pGrid1->addWidget(pLabel11, 0, 0);
    pGrid1->addWidget(pLabel12, 0, 1);
    pGrid1->addWidget(m_p10MFreqTotao1_LineEdit, 1, 0);
    pGrid1->addWidget(m_p10MFreqDelta1_LineEdit, 1, 1);
    pGrid1->addWidget(m_p10MFreqAdd1_Button, 1, 2);
    pGrid1->addWidget(m_p10MFreqSub1_Button, 1, 3);
    pGrid1->addWidget(m_p10MFreqClear1_Button, 1, 4);
    pGrid1->addWidget(m_p10MFreqSave1_Button, 1, 5);
    pFreqGroup->setLayout(pGrid1);

    // 2
    QLabel* pLabel21 = new QLabel(tr("调节总量ns"));
    QLabel* pLabel22 = new QLabel(tr("增减调节量ns"));

    m_p10MFreqTotao2_LineEdit = new QLineEdit();
    m_p10MFreqTotao2_LineEdit->setReadOnly(true);
    m_p10MFreqDelta2_LineEdit = new QLineEdit();

    m_p10MFreqAdd2_Button = new QPushButton(tr("增"));
    m_p10MFreqSub2_Button = new QPushButton(tr("减"));
    m_p10MFreqClear2_Button = new QPushButton(tr("清零"));
    m_p10MFreqSave2_Button = new QPushButton(tr("保存"));

    connect(m_p10MFreqAdd2_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_p10MFreqAdd2_Button, ID_OF_10M_PHAS_ADD);
    connect(m_p10MFreqSub2_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_p10MFreqSub2_Button, ID_OF_10M_PHAS_SUB);
    connect(m_p10MFreqSave2_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_p10MFreqSave2_Button, ID_OF_10M_PHAS_SAVE);
    connect(m_p10MFreqClear2_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_p10MFreqClear2_Button, ID_OF_10M_PHAS_CLEAR);

    // Reg
    QRegExp oReg1("^[0-9]{1,7}$");
    QRegExpValidator* pValid1 = new QRegExpValidator(oReg1, this);
    m_p10MFreqDelta1_LineEdit->setValidator(pValid1);

    QRegExp oReg2("^[0-9]{1,3}$");
    QRegExpValidator* pValid2 = new QRegExpValidator(oReg2, this);
    m_p10MFreqDelta2_LineEdit->setValidator(pValid2);

    QGridLayout* pGrid2 = new QGridLayout();
    pGrid2->addWidget(pLabel21, 2, 0);
    pGrid2->addWidget(pLabel22, 2, 1);
    pGrid2->addWidget(m_p10MFreqTotao2_LineEdit, 3, 0);
    pGrid2->addWidget(m_p10MFreqDelta2_LineEdit, 3, 1);
    pGrid2->addWidget(m_p10MFreqAdd2_Button, 3, 2);
    pGrid2->addWidget(m_p10MFreqSub2_Button, 3, 3);
    pGrid2->addWidget(m_p10MFreqClear2_Button, 3, 4);
    pGrid2->addWidget(m_p10MFreqSave2_Button, 3, 5);
    pPhasGroup->setLayout(pGrid2);

    QVBoxLayout* pVLayout = new QVBoxLayout();
    pVLayout->addWidget(pFreqGroup);
    pVLayout->addWidget(pPhasGroup);

    return pVLayout;
}
QVBoxLayout* CMyImageTab::Create10MPhasGroup()
{
    QGroupBox* pGroup1 = new QGroupBox(tr("10.23M DDS 频率微调(调节范围-2000000~2000000uHZ, 步进10uHz，默认值0)"));
    QGroupBox* pGroup2 = new QGroupBox(tr("10.23M DDS 相位偏移(调节范围-97~97ns, 步进12.5ns，默认值0)"));
    m_p10MDDS_Group = pGroup1;

    // 1
    QLabel* pLabel11 = new QLabel(tr("调节总量uHz"));
    QLabel* pLabel12 = new QLabel(tr("增减调节量uHz"));

    m_p1023MPhasTotao1_LineEdit = new QLineEdit();
    m_p1023MPhasTotao1_LineEdit->setReadOnly(true);
    m_p1023MPhasDelta1_LineEdit = new QLineEdit();

    m_p1023MPhasAdd1_Button = new QPushButton(tr("增"));
    m_p1023MPhasSub1_Button = new QPushButton(tr("减"));
    m_p1023MPhasClear1_Button = new QPushButton(tr("清零"));
    m_p1023MPhasSave1_Button = new QPushButton(tr("保存"));

    connect(m_p1023MPhasAdd1_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_p1023MPhasAdd1_Button, ID_OF_1023M_FREQ_ADD);
    connect(m_p1023MPhasSub1_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_p1023MPhasSub1_Button, ID_OF_1023M_FREQ_SUB);
    connect(m_p1023MPhasSave1_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_p1023MPhasSave1_Button, ID_OF_1023M_FREQ_SAVE);
    connect(m_p1023MPhasClear1_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_p1023MPhasClear1_Button, ID_OF_1023M_FREQ_CLEAR);

    QGridLayout* pGrid1 = new QGridLayout();
    pGrid1->addWidget(pLabel11, 0, 0);
    pGrid1->addWidget(pLabel12, 0, 1);
    pGrid1->addWidget(m_p1023MPhasTotao1_LineEdit, 1, 0);
    pGrid1->addWidget(m_p1023MPhasDelta1_LineEdit, 1, 1);
    pGrid1->addWidget(m_p1023MPhasAdd1_Button, 1, 2);
    pGrid1->addWidget(m_p1023MPhasSub1_Button, 1, 3);
    pGrid1->addWidget(m_p1023MPhasClear1_Button, 1, 4);
    pGrid1->addWidget(m_p1023MPhasSave1_Button, 1, 5);
    pGroup1->setLayout(pGrid1);

    // 2
    QLabel* pLabel21 = new QLabel(tr("调节总量ns"));
    QLabel* pLabel22 = new QLabel(tr("增减调节量ns"));

    m_p1023MPhasTotao2_LineEdit = new QLineEdit();
    m_p1023MPhasTotao2_LineEdit->setReadOnly(true);
    m_p1023MPhasDelta2_LineEdit = new QLineEdit();

    m_p1023MPhasAdd2_Button = new QPushButton(tr("增"));
    m_p1023MPhasSub2_Button = new QPushButton(tr("减"));
    m_p1023MPhasClear2_Button = new QPushButton(tr("清零"));
    m_p1023MPhasSave2_Button = new QPushButton(tr("保存"));

    connect(m_p1023MPhasAdd2_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_p1023MPhasAdd2_Button, ID_OF_1023M_PHAS_ADD);
    connect(m_p1023MPhasSub2_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_p1023MPhasSub2_Button, ID_OF_1023M_PHAS_SUB);
    connect(m_p1023MPhasSave2_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_p1023MPhasSave2_Button, ID_OF_1023M_PHAS_SAVE);
    connect(m_p1023MPhasClear2_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_p1023MPhasClear2_Button, ID_OF_1023M_PHAS_CLEAR);

    // Reg
    QRegExp oReg1("^[0-9]{1,7}$");
    QRegExpValidator* pValid1 = new QRegExpValidator(oReg1, this);
    m_p1023MPhasDelta1_LineEdit->setValidator(pValid1);

    QRegExp oReg2("^[0-9]{1,3}$");
    QRegExpValidator* pValid2 = new QRegExpValidator(oReg2, this);
    m_p1023MPhasDelta2_LineEdit->setValidator(pValid2);

    QGridLayout* pGrid2 = new QGridLayout();
    pGrid2->addWidget(pLabel21, 2, 0);
    pGrid2->addWidget(pLabel22, 2, 1);
    pGrid2->addWidget(m_p1023MPhasTotao2_LineEdit, 3, 0);
    pGrid2->addWidget(m_p1023MPhasDelta2_LineEdit, 3, 1);
    pGrid2->addWidget(m_p1023MPhasAdd2_Button, 3, 2);
    pGrid2->addWidget(m_p1023MPhasSub2_Button, 3, 3);
    pGrid2->addWidget(m_p1023MPhasClear2_Button, 3, 4);
    pGrid2->addWidget(m_p1023MPhasSave2_Button, 3, 5);
    pGroup2->setLayout(pGrid2);

    QVBoxLayout* pVLayout = new QVBoxLayout();
    pVLayout->addWidget(pGroup1);
    pVLayout->addWidget(pGroup2);

    return pVLayout;
}

QVBoxLayout* CMyImageTab::CreatePhaseDelayGroup()
{
    // 1
    QLabel* pLabel11 = new QLabel(tr("调节总量ns"));
    QLabel* pLabel12 = new QLabel(tr("增减调节量ns"));
    m_pPlusTip_Label = new QLabel(tr("调节总量超出范围，输入无效"));
    QPalette pe;
    pe.setColor(QPalette::WindowText,Qt::darkRed);
    m_pPlusTip_Label->setPalette(pe);
    m_pPlusTip_Label->hide();

    m_pPlusTotal_LineEdit = new QLineEdit();
    m_pPlusTotal_LineEdit->setReadOnly(true);
    m_pPlusDelta_LineEdit = new QLineEdit();

    m_pPlusAdd_Button = new QPushButton(tr("增"));
    m_pPlusSub_Button = new QPushButton(tr("减"));
    m_pPlusClear_Button = new QPushButton(tr("默认"));

    connect(m_pPlusAdd_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pPlusAdd_Button, ID_OF_PLUS_ADD);
    connect(m_pPlusSub_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pPlusSub_Button, ID_OF_PLUS_SUB);
    connect(m_pPlusClear_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pPlusClear_Button, ID_OF_PLUS_CLEAR);

    QPushButton* pEmptyButton = new QPushButton(tr("  "));
    pEmptyButton->setEnabled(false);

    QGridLayout* pGrid1 = new QGridLayout();
    pGrid1->addWidget(pLabel11, 0, 0);
    pGrid1->addWidget(pLabel12, 0, 1);
    pGrid1->addWidget(m_pPlusTip_Label, 0, 2, 1, 2);
    pGrid1->addWidget(m_pPlusTotal_LineEdit, 1, 0);
    pGrid1->addWidget(m_pPlusDelta_LineEdit, 1, 1);
    pGrid1->addWidget(m_pPlusAdd_Button, 1, 2);
    pGrid1->addWidget(m_pPlusSub_Button, 1, 3);
    pGrid1->addWidget(m_pPlusClear_Button, 1, 4);
    pGrid1->addWidget(pEmptyButton, 1, 5);
    
    QGroupBox* pPhaseGroup = new QGroupBox(tr("PPS脉宽调节(调节范围100 ~ 2000000ns, 步进12.5ns，默认值20us)"));
    pPhaseGroup->setLayout(pGrid1);
    m_pPhaseWidth_Group = pPhaseGroup;

    // 2
    QLabel* pLabel21 = new QLabel(tr("调节总量ns"));
    QLabel* pLabel22 = new QLabel(tr("增减调节量ns"));

    m_pDelayTotal_LineEdit = new QLineEdit();
    m_pDelayTotal_LineEdit->setReadOnly(true);
    m_pDelayDelta_LineEdit = new QLineEdit();

    m_pDelayAdd_Button = new QPushButton(tr("增"));
    m_pDelaySub_Button = new QPushButton(tr("减"));
    m_pDelayClear_Button = new QPushButton(tr("清零"));

    connect(m_pDelayAdd_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pDelayAdd_Button, ID_OF_DELAY_ADD);
    connect(m_pDelaySub_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pDelaySub_Button, ID_OF_DELAY_SUB);
    connect(m_pDelayClear_Button, SIGNAL(clicked()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pDelayClear_Button, ID_OF_DELAY_CLEAR);

    m_pAlign_Button = new QPushButton(tr("对 齐"));

    connect(m_pAlign_Button, SIGNAL(clicked()), this, SLOT(AlignFunc()));

    QGridLayout* pGrid2 = new QGridLayout();
    pGrid2->addWidget(pLabel21, 0, 0);
    pGrid2->addWidget(pLabel22, 0, 1);
    pGrid2->addWidget(m_pDelayTotal_LineEdit, 1, 0);
    pGrid2->addWidget(m_pDelayDelta_LineEdit, 1, 1);
    pGrid2->addWidget(m_pDelayAdd_Button, 1, 2);
    pGrid2->addWidget(m_pDelaySub_Button, 1, 3);
    pGrid2->addWidget(m_pDelayClear_Button, 1, 4);
    pGrid2->addWidget(m_pAlign_Button, 1, 5);

    QGroupBox* pDelayGroup = new QGroupBox(tr("PPS相位偏移(调节范围-999999999~999999999ns, 步进12.5ns，默认值0)"));
    pDelayGroup->setLayout(pGrid2);
    m_pPhaseDelay_Group = pDelayGroup;

    // Reg
    QRegExp oReg1("^[0-9]{1,9}$");
    QRegExpValidator* pValid1 = new QRegExpValidator(oReg1, this);
    m_pPlusDelta_LineEdit->setValidator(pValid1);

    QRegExp oReg2("^[0-9]{1,9}$");
    QRegExpValidator* pValid2 = new QRegExpValidator(oReg2, this);
    m_pDelayDelta_LineEdit->setValidator(pValid2);

    // Layout
    QVBoxLayout* pVLayout = new QVBoxLayout();
    pVLayout->addWidget(pPhaseGroup);
    pVLayout->addWidget(pDelayGroup);

    return pVLayout;
}

void CMyImageTab::resizeEvent(QResizeEvent* resizeEvent)
{
    const QSize size = resizeEvent->size();

    // 右边控件对齐
    QRect rectEight = m_pEight_Group->geometry();
    QRect rectDDS = m_p10MDDS_Group->geometry();
    if((true == g_oPageCheck.SignalTab_Priority_Use()) && (true == g_oPageCheck.SignalTab_InputCtrl_Use()))
    {
        m_pPriority_Group->setMinimumWidth(rectEight.width());
    }
    if(true == g_oPageCheck.SignalTab_InputCtrl_Use())
    {
        m_pPhaseWidth_Group->setMaximumWidth(rectDDS.width());
        m_pPhaseDelay_Group->setMaximumWidth(rectDDS.width());
    }

    // 视图窗口尺寸
    m_iWinWidth = 400;
    m_iWinHeight = m_iWinWidth * size.height()/size.width();

    // Image 使用尺寸(window)
    QRect rectImage = m_pImageGroup->geometry();
    int x = rectImage.x()*m_iWinWidth/width();
    int y = rectImage.y()*m_iWinHeight/height();
    m_iImageWidth = rectImage.width() * m_iWinWidth / width();
    m_iImageHeight = rectImage.height() * m_iWinHeight / height();

    if(NULL != m_pMyLabel)
    {
        m_pMyLabel->resize(rectImage.width(), rectImage.height());
        m_pMyLabel->move(0, 0);
    }
}

void CMyImageTab::paintEvent(QPaintEvent* pe)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Style Sheet
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
    
    // window
    int w,h,x,y;
    GetViewportXYWH(x, y, w, h);
    painter.setWindow(0, 0, m_iWinWidth, m_iWinHeight);
    painter.setViewport(x, y, w, h);
   
}

void CMyImageTab::GetViewportXYWH(int& x, int& y, int& w, int& h)
{
    x = 0;
    y = 0;
    w = width();
    h = height();
}

bool CMyImageTab::ComNetCanUse()
{
    return m_bComNetCanUse;
}

// 只有在 “手动切换” 模式才能切换频率
bool CMyImageTab::CanSwitchSignal()
{
    if(1 != m_iPriorityType_Cur)
    {
        ShowPrioInfo_Func(tr("请先选择“手工切换”模式"), true);
        return false;
    }
    return true;
}

void CMyImageTab::mousePressEvent(QMouseEvent* mouseEvent)
{
    int iWidth = width();
    int iHeight = height();

    QPoint herePos = mouseEvent->pos();
    int x = herePos.x();
    int y = herePos.y();

    float fWidthDiv = (float)width() / (float)m_iWinWidth;
    float fHeightDiv = (float)height() / (float)m_iWinHeight;

    if(NULL != m_pMyLabel)
    {
        m_pMyLabel->mousePressEventFunc(mouseEvent);
    }
}


// 刷新命令：连续获取设备状态和清命令
void CMyImageTab::CmdGetDeviceStatus()
{
    m_enFuncCmd = en_Refresh_Cmd;
}
void CMyImageTab::CmdClear()
{
    m_enFuncCmd = en_None_Cmd;
}


// 设置按钮状态(针对端口是否打开)
void CMyImageTab::SetFuncButtonEnable(bool bEnable)
{
    m_bComNetCanUse = bEnable;

    if(true == g_oPageCheck.SignalTab_InputCtrl_Use())
    {
        m_p10MFreqAdd1_Button->setEnabled(bEnable);
        m_p10MFreqSub1_Button->setEnabled(bEnable);
        m_p10MFreqClear1_Button->setEnabled(bEnable);
        m_p10MFreqSave1_Button->setEnabled(bEnable);

        m_p10MFreqAdd2_Button->setEnabled(bEnable);
        m_p10MFreqSub2_Button->setEnabled(bEnable);
        m_p10MFreqClear2_Button->setEnabled(bEnable);
        m_p10MFreqSave2_Button->setEnabled(bEnable);

        m_p1023MPhasAdd1_Button->setEnabled(bEnable);
        m_p1023MPhasSub1_Button->setEnabled(bEnable);
        m_p1023MPhasClear1_Button->setEnabled(bEnable);
        m_p1023MPhasSave1_Button->setEnabled(bEnable);

        m_p1023MPhasAdd2_Button->setEnabled(bEnable);
        m_p1023MPhasSub2_Button->setEnabled(bEnable);
        m_p1023MPhasClear2_Button->setEnabled(bEnable);
        m_p1023MPhasSave2_Button->setEnabled(bEnable);

        m_pPlusAdd_Button->setEnabled(bEnable);
        m_pPlusSub_Button->setEnabled(bEnable);
        m_pPlusClear_Button->setEnabled(bEnable);

        m_pDelayAdd_Button->setEnabled(bEnable);
        m_pDelaySub_Button->setEnabled(bEnable);
        m_pDelayClear_Button->setEnabled(bEnable);

        m_pAlign_Button->setEnabled(bEnable);

        //if(false == bEnable)
        {
            m_pReduceAdd_6_Button->setEnabled(false);
            m_pReduceSub_6_Button->setEnabled(false);
            m_pReduceAdd_5_Button->setEnabled(false);
            m_pReduceSub_5_Button->setEnabled(false);
            m_pReduceAdd_4_Button->setEnabled(false);
            m_pReduceSub_4_Button->setEnabled(false);
            m_pReduceAdd_3_Button->setEnabled(false);
            m_pReduceSub_3_Button->setEnabled(false);
            m_pReduceAdd_26_Button->setEnabled(false);
            m_pReduceSub_26_Button->setEnabled(false);
            m_pReduceAdd_25_Button->setEnabled(false);
            m_pReduceSub_25_Button->setEnabled(false);
            m_pReduceAdd_24_Button->setEnabled(false);
            m_pReduceSub_24_Button->setEnabled(false);
            m_pReduceAdd_23_Button->setEnabled(false);
            m_pReduceSub_23_Button->setEnabled(false);
        }
        //else
        {
            if((NULL != m_pComNetTab) && (m_pComNetTab->BoardIsValid(6,0))) m_pReduceAdd_6_Button->setEnabled(bEnable);
            if((NULL != m_pComNetTab) && (m_pComNetTab->BoardIsValid(6,0))) m_pReduceSub_6_Button->setEnabled(bEnable);
            if((NULL != m_pComNetTab) && (m_pComNetTab->BoardIsValid(5,0))) m_pReduceAdd_5_Button->setEnabled(bEnable);
            if((NULL != m_pComNetTab) && (m_pComNetTab->BoardIsValid(5,0))) m_pReduceSub_5_Button->setEnabled(bEnable);
            if((NULL != m_pComNetTab) && (m_pComNetTab->BoardIsValid(4,0))) m_pReduceAdd_4_Button->setEnabled(bEnable);
            if((NULL != m_pComNetTab) && (m_pComNetTab->BoardIsValid(4,0))) m_pReduceSub_4_Button->setEnabled(bEnable);
            if((NULL != m_pComNetTab) && (m_pComNetTab->BoardIsValid(3,0))) m_pReduceAdd_3_Button->setEnabled(bEnable);
            if((NULL != m_pComNetTab) && (m_pComNetTab->BoardIsValid(3,0))) m_pReduceSub_3_Button->setEnabled(bEnable);

            if((NULL != m_pComNetTab) && (m_pComNetTab->BoardIsValid(0x26,0))) m_pReduceAdd_26_Button->setEnabled(bEnable);
            if((NULL != m_pComNetTab) && (m_pComNetTab->BoardIsValid(0x26,0))) m_pReduceSub_26_Button->setEnabled(bEnable);
            if((NULL != m_pComNetTab) && (m_pComNetTab->BoardIsValid(0x25,0))) m_pReduceAdd_25_Button->setEnabled(bEnable);
            if((NULL != m_pComNetTab) && (m_pComNetTab->BoardIsValid(0x25,0))) m_pReduceSub_25_Button->setEnabled(bEnable);
            if((NULL != m_pComNetTab) && (m_pComNetTab->BoardIsValid(0x24,0))) m_pReduceAdd_24_Button->setEnabled(bEnable);
            if((NULL != m_pComNetTab) && (m_pComNetTab->BoardIsValid(0x24,0))) m_pReduceSub_24_Button->setEnabled(bEnable);
            if((NULL != m_pComNetTab) && (m_pComNetTab->BoardIsValid(0x23,0))) m_pReduceAdd_23_Button->setEnabled(bEnable);
            if((NULL != m_pComNetTab) && (m_pComNetTab->BoardIsValid(0x23,0))) m_pReduceSub_23_Button->setEnabled(bEnable);

        }

        m_pAlignCompseAdd_Button->setEnabled(bEnable);
        m_pAlignCompseSub_Button->setEnabled(bEnable);
    }

    if(true == g_oPageCheck.SignalTab_Priority_Use())
    {
        m_pPriorityQuery_Button->setEnabled(bEnable);
        m_pPrioritySet_Button->setEnabled(bEnable);
    }
}

// 命令结束
void CMyImageTab::CmdFinished_Func()
{
    switch(m_enFuncCmd)
    {
    case en_Refresh_Cmd:
        m_enFuncCmd = en_getsta_Cmd;
        
        // 发送减0命令，获取调节总量等信息
        SendSubZeroForGetInfo();

        //  给 参考输入板 和 主控板 发送 getsta 命令
        m_pComNetTab->Send_getsta_To_RInputAndMain();

        // 获取衰减量
        SendReduceCmdGetInfo();

        // 获取对齐补偿
        CompseFunc(0);

        // 获取 本地频率校准
        GetLocalFreqAdj();

        break;
    case en_getsta_Cmd:
        m_enFuncCmd = en_None_Cmd;
        break;
    default:
        m_enFuncCmd = en_None_Cmd;
        break;
    }
}

// 处理数据
void CMyImageTab::HandleFrameData(unsigned char chFrameCmd, const char* pFrameDataBuf, int iFrameDataLength, unsigned char chSourceAddr, unsigned char chSourcePort)
{
    QString strData = pFrameDataBuf;
    emit HandleFrame_Signal(strData, chSourceAddr, chSourcePort);
}

void CMyImageTab::HandleFrame_Slot(const QString& strData, unsigned char chSourceAddr, unsigned char chSourcePort)
{
    strcpy_s(m_pRefStaInput_Buf, SIZE_OF_INPUT_BUF, strData.toLatin1().data());

    char* ptr = NULL;
    char* ptrValue = NULL;
    char chValue = 0;

    if(NULL != strstr(m_pRefStaInput_Buf, "Main_Control"))
    {
        // Main_Control sta RefClkSelec=0 
        // SEC=0.10V PRM=24.41V MB=13.80V MB_I=361.00 INNER=12.93V INNER_I=417.00 PLL1_I=1107.00 PLL2_I=329.00

        // RefClkSelec=0
        ptrValue = strstr(m_pRefStaInput_Buf, "RefClkSelec");
        if(NULL != ptrValue)
        {
            ptrValue += 12;
            chValue = *ptrValue - '0';

            stSignalSelectData& oSigSelect = m_TopImageData.m_stSigSelectArray[0];
            if(0 == chValue)
            {
                ResetOldImageConnect(0);
                m_TopImageData.m_pCurSigSelect = &oSigSelect;

                oSigSelect.m_enSelectStatus = en_Signal_Connect_sta;
                if(NULL != m_pMyLabel) m_pMyLabel->SetTopConnectStatus(0, en_Signal_Connect_Status);
            }
            else
            {
                oSigSelect.m_enSelectStatus = en_Signal_sta;
                if(NULL != m_pMyLabel) m_pMyLabel->SetTopSelectStatus(0, en_Signal_Status);
            }

            update();
        }

        // priority 123412 1
        ptrValue = strstr(m_pRefStaInput_Buf, "priority");
        if(NULL != ptrValue)
        {
            m_iPrioCmdTimeout_Count = 0;

            int iPriority1 = 0;
            int iPriority2 = 0;
            int iPriority3 = 0;
            int iPriority4 = 0;

            int iPriority5 = 0;
            int iPriority6 = 0;

            int iPriorityType = 0;

            char chValue[2] = {0};
            while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
            chValue[0] = *ptrValue;
            ++ptrValue;
            iPriority1 = atoi(chValue);
            chValue[0] = *ptrValue;
            ++ptrValue;
            iPriority2 = atoi(chValue);
            chValue[0] = *ptrValue;
            ++ptrValue;
            iPriority3 = atoi(chValue);
            chValue[0] = *ptrValue;
            ++ptrValue;
            iPriority4 = atoi(chValue);

            chValue[0] = *ptrValue;
            ++ptrValue;
            iPriority5 = atoi(chValue);
            chValue[0] = *ptrValue;
            ++ptrValue;
            iPriority6 = atoi(chValue);

            while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
            chValue[0] = *ptrValue;
            iPriorityType = atoi(chValue);

            if(iPriority1 < 1 || iPriority1 > 4) iPriority1 = 4;
            if(iPriority2 < 1 || iPriority2 > 4) iPriority2 = 4;
            if(iPriority3 < 1 || iPriority3 > 4) iPriority3 = 4;
            if(iPriority4 < 1 || iPriority4 > 4) iPriority4 = 4;

            if(iPriority5 < 1 || iPriority5 > 2) iPriority5 = 2;
            if(iPriority6 < 1 || iPriority6 > 2) iPriority6 = 2;

            if(iPriorityType < 1 || iPriorityType > 3) iPriorityType = 3;

            if(true == m_bQueryPriority)
            {
                m_bQueryPriority = false;
                SetPriority(iPriority1, iPriority2, iPriority3, iPriority4, iPriority5, iPriority6, iPriorityType);

                SetPriorityCtrl(true);
            }
            else
            {
                // 转换
                int PriorityArray[5] = {0};
                PriorityArray[1] = iPriority1;
                PriorityArray[2] = iPriority2;
                PriorityArray[3] = iPriority3;
                PriorityArray[4] = iPriority4;
                for(int i = 1; i < 5; ++i)
                {
                    if(1 == PriorityArray[i])
                    {
                        iPriority1 = i;
                        continue;
                    }
                    if(2 == PriorityArray[i])
                    {
                        iPriority2 = i;
                        continue;
                    }
                    if(3 == PriorityArray[i])
                    {
                        iPriority3 = i;
                        continue;
                    }
                    if(4 == PriorityArray[i])
                    {
                        iPriority4 = i;
                        continue;
                    }
                }
                if(iPriority5 > iPriority6)
                {
                    iPriority6 = 1;
                    iPriority5 = 2;
                }

                m_iPriority1_New = iPriority1;
                m_iPriority2_New = iPriority2;
                m_iPriority3_New = iPriority3;
                m_iPriority4_New = iPriority4;
                m_iPriority5_New = iPriority5;
                m_iPriority6_New = iPriority6;
                m_iPriorityType_New = iPriorityType;
            }
        }
    }
    else if(NULL != strstr(m_pRefStaInput_Buf, "Reference_Input"))
    {
        // Reference_Input NoSecRef=0 sta U_V=0.00 ImA=0.0 Local_T:31 Remote_T:37 
        // 10M=0 10.23M=0 10.23M_LVDS=0 clkS1 PPS_TTL=0 PPS_LVDS=0 ppsS1

        // NoSecRef
        ptrValue = strstr(m_pRefStaInput_Buf, "NoSecRef");
        if(NULL != ptrValue)
        {
            // 表示选择的参考没有输入 且 本地标为1，要告警
            ptrValue += 9;
            chValue = *ptrValue - '0';
            if((1 == chValue) && (en_Signal_sta == m_TopImageData.m_stSigSelectArray[0].m_enSelectStatus))
            {
                QString strInfo = tr("当前选择的参考信号没有输入\n");
                m_pComNetTab->ShowRecvDataFunc(chSourceAddr, chSourcePort, strInfo.toLatin1().data(), false, 250, 0, 0);
            }
        }

        //10M=0
        ptrValue = strstr(m_pRefStaInput_Buf, "10M");
        if(NULL != ptrValue)
        {
            ptrValue += 4;
            chValue = *ptrValue - '0';

            stSignalSelectData& oSigSelect = m_TopImageData.m_stSigSelectArray[1];
            if(1 == chValue)
            {
                oSigSelect.m_enSelectStatus = en_Signal_sta;
                if(NULL != m_pMyLabel) m_pMyLabel->SetTopSelectStatus(1, en_Signal_Status);
            }
            else
            {
                oSigSelect.m_enSelectStatus = en_Empty_sta;
                if(NULL != m_pMyLabel) m_pMyLabel->SetTopSelectStatus(1, en_Empty_Status);
            }
        }

        //10.23M=0 
        ptrValue = strstr(m_pRefStaInput_Buf, "10.23M");
        if(NULL != ptrValue)
        {
            ptrValue += 7;
            chValue = *ptrValue - '0';

            stSignalSelectData& oSigSelect = m_TopImageData.m_stSigSelectArray[2];
            if(1 == chValue)
            {
                oSigSelect.m_enSelectStatus = en_Signal_sta;
                if(NULL != m_pMyLabel) m_pMyLabel->SetTopSelectStatus(2, en_Signal_Status);
            }
            else
            {
                oSigSelect.m_enSelectStatus = en_Empty_sta;
                if(NULL != m_pMyLabel) m_pMyLabel->SetTopSelectStatus(2, en_Empty_Status);
            }
        }

        //10.23M_LVDS=0 
        ptrValue = strstr(m_pRefStaInput_Buf, "10.23M_LVDS");
        if(NULL != ptrValue)
        {
            ptrValue += 12;
            chValue = *ptrValue - '0';

            stSignalSelectData& oSigSelect = m_TopImageData.m_stSigSelectArray[3];
            if(1 == chValue)
            {
                oSigSelect.m_enSelectStatus = en_Signal_sta;
                if(NULL != m_pMyLabel) m_pMyLabel->SetTopSelectStatus(3, en_Signal_Status);
            }
            else
            {
                oSigSelect.m_enSelectStatus = en_Empty_sta;
                if(NULL != m_pMyLabel) m_pMyLabel->SetTopSelectStatus(3, en_Empty_Status);
            }
        }

        //clkS1 : 1/2/3 分别对应 10M,10.23M, LVDS, 本地标为1才可能连接
        ptrValue = strstr(m_pRefStaInput_Buf, "clkS");
        if(NULL != ptrValue)
        {
            ptrValue += 4;
            chValue = *ptrValue - '0';
            if(chValue < 0) chValue = 0;
            if(chValue > 3) chValue = 3;

            if((chValue > 0) && (en_Signal_sta == m_TopImageData.m_stSigSelectArray[0].m_enSelectStatus))
            {
                stSignalSelectData& oSigSelect = m_TopImageData.m_stSigSelectArray[chValue];

                ResetOldImageConnect(0);
                m_TopImageData.m_pCurSigSelect = &oSigSelect;

                if((en_Empty_sta == oSigSelect.m_enSelectStatus) || (en_Empty_Connect_sta == oSigSelect.m_enSelectStatus))
                {
                    oSigSelect.m_enSelectStatus = en_Empty_sta;
                }
                else
                {
                    oSigSelect.m_enSelectStatus = en_Signal_Connect_sta;
                }
            }

            if((chValue > 0) && (NULL != m_pMyLabel))
            {
                if(en_Signal_Connect_Status != m_pMyLabel->GetTopStatus(0))
                {
                    if(en_Signal_Status == m_pMyLabel->GetTopStatus(chValue))
                    {
                        m_pMyLabel->SetTopConnectStatus(chValue, en_Signal_Connect_Status);
                    }
                }
            }
        }

        //PPS_TTL=0 
        ptrValue = strstr(m_pRefStaInput_Buf, "PPS_TTL");
        if(NULL != ptrValue)
        {
            ptrValue += 8;
            chValue = *ptrValue - '0';

            stSignalSelectData& oSigSelect = m_BottomImageData.m_stSigSelectArray[0];
            if(1 == chValue)
            {
                oSigSelect.m_enSelectStatus = en_Signal_sta;
                if(NULL != m_pMyLabel) m_pMyLabel->SetBottomSelectStatus(0, en_Signal_Status);
            }
            else
            {
                oSigSelect.m_enSelectStatus = en_Empty_sta;
                if(NULL != m_pMyLabel) m_pMyLabel->SetBottomSelectStatus(0, en_Empty_Status);
            }
        }

        //PPS_LVDS=0 
        ptrValue = strstr(m_pRefStaInput_Buf, "PPS_LVDS");
        if(NULL != ptrValue)
        {
            ptrValue += 9;
            chValue = *ptrValue - '0';

            stSignalSelectData& oSigSelect = m_BottomImageData.m_stSigSelectArray[1];
            if(1 == chValue)
            {
                oSigSelect.m_enSelectStatus = en_Signal_sta;
                if(NULL != m_pMyLabel) m_pMyLabel->SetBottomSelectStatus(1, en_Signal_Status);
            }
            else
            {
                oSigSelect.m_enSelectStatus = en_Empty_sta;
                if(NULL != m_pMyLabel) m_pMyLabel->SetBottomSelectStatus(1, en_Empty_Status);
            }

            // UpdateTab
            if(NULL != m_pComNetTab) m_pComNetTab->SetDeviceSignalStatus(chValue);
        }

        //ppsS1
        ptrValue = strstr(m_pRefStaInput_Buf, "ppsS");
        if(NULL != ptrValue)
        {
            ptrValue += 4;
            chValue = *ptrValue - '0';
            if(chValue < 0) chValue = 0;
            if(chValue > 1) chValue = 1;

            stSignalSelectData& oSigSelect = m_BottomImageData.m_stSigSelectArray[chValue];

            ResetOldImageConnect(1);
            m_BottomImageData.m_pCurSigSelect = &oSigSelect;

            if((en_Empty_sta == oSigSelect.m_enSelectStatus) || (en_Empty_Connect_sta == oSigSelect.m_enSelectStatus))
            {
                oSigSelect.m_enSelectStatus = en_Empty_sta;
            }
            else
            {
                oSigSelect.m_enSelectStatus = en_Signal_Connect_sta;
            }

            if(NULL != m_pMyLabel)
            {
                if(en_Signal_Status == m_pMyLabel->GetBottomStatus(chValue))
                {
                    m_pMyLabel->SetBottomConnectStatus(chValue, en_Signal_Connect_Status);
                }
            }
        }

        update();
    }
    else if(NULL != (ptrValue = strstr(m_pRefStaInput_Buf, "setfreq")))
    {
        // setfreq 1 value 10uHz, total 26uHz(有可能是负数)
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

        if(1 == iHereIndex)
        {
            if((0x80 == chSourceAddr) && (0x00 == chSourcePort)) m_p10MFreqTotao1_LineEdit->setText(QString::number(iHereData));
        }
        else
        {
            if((0x80 == chSourceAddr) && (0x00 == chSourcePort)) m_p1023MPhasTotao1_LineEdit->setText(QString::number(iHereData));
        }
    }
    else if(NULL != (ptrValue = strstr(m_pRefStaInput_Buf, "setphas")))
    {
        // setphas 2 value 10uHz, total 26uHz(有可能是负数)
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

        if(1 == iHereIndex)
        {
            if((0x80 == chSourceAddr) && (0x00 == chSourcePort)) m_p10MFreqTotao2_LineEdit->setText(QString::number(iHereData));
        }
        else
        {
            if((0x80 == chSourceAddr) && (0x00 == chSourcePort)) m_p1023MPhasTotao2_LineEdit->setText(QString::number(iHereData));
        }
    }
    else if(NULL != strstr(m_pRefStaInput_Buf, "pw pulse value"))
    {
        if((NULL != strstr(m_pRefStaInput_Buf, "parameter is too small")) || (NULL != strstr(m_pRefStaInput_Buf, "parameter is too large")))
        {
            m_pPlusTip_Label->show();
        }
        else
        {
            m_pPlusTip_Label->hide();
        }

        // pw pulse value 100, total value 500000200ns(有可能是负数)
        ptrValue = strstr(m_pRefStaInput_Buf, "total value");
        if(NULL != ptrValue)
        {
            while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
            ptr = ptrValue;
            while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
            *ptr = '\0';
            if('-' == *(ptrValue-1)) --ptrValue;
            if((0x80 == chSourceAddr) && (0x23 == chSourcePort)) m_pPlusTotal_LineEdit->setText(QString::number(atoi(ptrValue)));
        }
    }
    else if(NULL != strstr(m_pRefStaInput_Buf, "ud delay value"))
    {
        // ud delay value 10, total value 20ns; (有可能是负数)
        ptrValue = strstr(m_pRefStaInput_Buf, "total value");
        if(NULL != ptrValue)
        {
            while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
            ptr = ptrValue;
            while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
            *ptr = '\0';
            if('-' == *(ptrValue-1)) --ptrValue;
            if((0x80 == chSourceAddr) && (0x23 == chSourcePort)) m_pDelayTotal_LineEdit->setText(QString::number(atoi(ptrValue)));
        }
    }
    else if(NULL != (ptrValue = strstr(m_pRefStaInput_Buf, "parameter is")))    // (有可能是负数)
    {
        while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
        ptr = ptrValue;
        while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
        *ptr = '\0';
        if('-' == *(ptrValue-1)) --ptrValue;

        SetReduceShowValue(atoi(ptrValue), chSourceAddr, chSourcePort);
    }
    else if(NULL != (ptrValue = strstr(m_pRefStaInput_Buf, "sy compensatory value =")))
    {
        while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
        ptr = ptrValue;
        while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
        *ptr = '\0';
        m_pAlignCompse_LineEdit->setText(QString::number(atoi(ptrValue)));
    }
    else if(NULL != (ptrValue = strstr(m_pRefStaInput_Buf, "priority")))
    {
        m_iPrioCmdTimeout_Count = 0;
        SetPriorityCtrl(true);
    }
    else if(NULL != (ptrValue = strstr(m_pRefStaInput_Buf, "Input_alarm")))
    {
        // Input_alarm 3 10M value:10230635

        // 3
        while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
        ptr = ptrValue;
        while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
        *ptr = '\0';
        int iIndex = atoi(ptrValue);

        // 10M
        ptrValue = ptr + 1;
        while(0x20 == *ptrValue) ++ptrValue;
        ptr = ptrValue;
        while(0x20 != *ptr) ++ptr;
        *ptr = '\0';
        char* pName = ptrValue;

        // 10230635
        ptrValue = ptr + 1;
        while(':' != *ptrValue) ++ptrValue;
        ptr = ++ptrValue;
        while(('\0' != *ptr) && (('.' == *ptr) || ((*ptr >= '0') && (*ptr <= '9')))) ++ptr;
        *ptr = '\0';
        char* pValue = ptrValue;

        // 提示
        QString strDan = "毫秒";
        if(iIndex > 2) strDan = "Hz";
        QString strInfo = tr("\n%1 输入信号出现异常，当前值：%2%3").arg(pName).arg(pValue).arg(strDan);
        //ShowInfo_Func(strInfo, QColor(250, 0, 0));
        m_pComNetTab->ShowRecvDataFunc(chSourceAddr, chSourcePort, strInfo.toLatin1().data(), false, 250, 0, 0);
    }
    /*else if(NULL != (ptrValue = strstr(m_pRefStaInput_Buf, "output")))
    {
        // 暂时屏蔽：因为这个错误频繁出现: 正弦波时钟分配板[26] 无输出
        // 21
        while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
        ptr = ptrValue;
        while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
        *ptr = '\0';
        int iAddr = (int)strtol(ptrValue, NULL, 16);

        // 1
        ptrValue = ptr + 1;
        while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
        ptr = ptrValue;
        while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
        *ptr = '\0';
        int iFlag = atoi(ptrValue);
        if(iFlag <= 0)
        {
            QString strBoradName = m_pComNetTab->GetBoardNameOnAddr(iAddr, 0);
            char chShow[128];
            sprintf_s(chShow, sizeof(chShow), "%s[%02X] 无输出", strBoradName.toLatin1().data(), iAddr);
            m_pComNetTab->ShowRecvDataFunc(chSourceAddr, chSourcePort, chShow, false, 250, 0, 0);
        }
    }*/
    else if(NULL != (ptrValue = strstr(m_pRefStaInput_Buf, "frequency local")))
    {
        // New: 2016-01-07：跳变检测：Delta = (Tnext - Tcur) - (Tcur - Tpre)，即，当前秒与上一秒的变化，与下一秒与当前秒的变化，两者之间的差值
        // frequency local=4999999 1 reference=4999999 1 10M=10000000 1 10.23M=10230000 1 
        // phase=914 PLL1=102388447 PLL2=108447318 ex_ref=0 lo_ref=56919247
        static int iT0PLL1 = 0;
        static int iT1PLL1 = 0;
        static int iT2PLL1 = 0;

        static int iT0PLL2 = 0;
        static int iT1PLL2 = 0;
        static int iT2PLL2 = 0;

        static int iT0ExRef = 0;
        static int iT1ExRef = 0;
        static int iT2ExRef = 0;

        static int iT0LoRef = 0;
        static int iT1LoRef = 0;
        static int iT2LoRef = 0;

        static int iT0Phase = 0;
        static int iT1Phase = 0;
        static int iT2Phase = 0;

        // Local
        // 4999999
        while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
        ptr = ptrValue;
        while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
        *ptr = '\0';
        int iFreq = atoi(ptrValue);

        // 0
        ptrValue = ++ptr;
        while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
        ptr = ptrValue;
        while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
        *ptr = '\0';
        int iLocalLock = atoi(ptrValue);

        // reference
        // 10230000
        ptrValue = ++ptr;
        while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
        ptr = ptrValue;
        while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
        *ptr = '\0';
        int iRef = atoi(ptrValue);

        // 0
        ptrValue = ++ptr;
        while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
        ptr = ptrValue;
        while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
        *ptr = '\0';
        int iRefLock = atoi(ptrValue);

        // 10M
        // 10000000
        ptrValue = ++ptr;
        while(('=' != *ptrValue)) ++ptrValue;
        ptr = ptrValue;

        while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
        ptr = ptrValue;
        while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
        *ptr = '\0';
        int i10M = atoi(ptrValue);

        // 0
        ptrValue = ++ptr;
        while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
        ptr = ptrValue;
        while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
        *ptr = '\0';
        int i10MLock = atoi(ptrValue);

        // 10.23M
        // 10230000
        ptrValue = ++ptr;
        while(('=' != *ptrValue)) ++ptrValue;
        ptr = ptrValue;

        while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
        ptr = ptrValue;
        while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
        *ptr = '\0';
        int i1023M = atoi(ptrValue);

        // 0
        ptrValue = ++ptr;
        while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
        ptr = ptrValue;
        while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
        *ptr = '\0';
        int i1023MLock = atoi(ptrValue);

        // phase=914 
        ptrValue = ++ptr;
        while(('=' != *ptrValue)) ++ptrValue;
        ptr = ptrValue;

        while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
        ptr = ptrValue;
        while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
        *ptr = '\0';
        int iPhase = atoi(ptrValue);

        // PLL1=102388447 
        ptrValue = ++ptr;
        while(('=' != *ptrValue)) ++ptrValue;
        ptr = ptrValue;

        while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
        ptr = ptrValue;
        while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
        *ptr = '\0';
        int iPLL1 = atoi(ptrValue);

        // PLL2=108447318 
        ptrValue = ++ptr;
        while(('=' != *ptrValue)) ++ptrValue;
        ptr = ptrValue;

        while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
        ptr = ptrValue;
        while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
        *ptr = '\0';
        int iPLL2 = atoi(ptrValue);

        // ex_ref=0 
        ptrValue = ++ptr;
        while(('=' != *ptrValue)) ++ptrValue;
        ptr = ptrValue;

        while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
        ptr = ptrValue;
        while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
        *ptr = '\0';
        int iEx_ref = atoi(ptrValue);

        // lo_ref=56919247
        ptrValue = ++ptr;
        while(('=' != *ptrValue)) ++ptrValue;
        ptr = ptrValue;

        while((*ptrValue < '0') || (*ptrValue > '9')) ++ptrValue;
        ptr = ptrValue;
        while((*ptr >= '0') && (*ptr <= '9')) ++ptr;
        *ptr = '\0';
        int iLo_ref = atoi(ptrValue);

        // Check
        iT0Phase = iT1Phase;
        iT1Phase = iT2Phase;
        iT2Phase = iPhase;

        // 时间必须前后连续
        if((1 == (iT1Phase-iT0Phase)) && (1 == (iT2Phase-iT1Phase)))
        {
            iT0PLL1 = iT1PLL1;
            iT1PLL1 = iT2PLL1;

            iT0PLL2 = iT1PLL2;
            iT1PLL2 = iT2PLL2;

            iT0ExRef = iT1ExRef;
            iT1ExRef = iT2ExRef;

            iT0LoRef = iT1LoRef;
            iT1LoRef = iT2LoRef;
        }
        else
        {
            iT0PLL1 = 0;
            iT1PLL1 = 0;

            iT0PLL2 = 0;
            iT1PLL2 = 0;

            iT0ExRef = 0;
            iT1ExRef = 0;

            iT0LoRef = 0;
            iT1LoRef = 0;
        }
        
        iT2PLL1 = iPLL1;
        iT2PLL2 = iPLL2;
        iT2ExRef = iEx_ref;
        iT2LoRef = iLo_ref;

        const int iChekc = 3;
        int iDeltaPLL1 = 0;
        int iDeltaPLL2 = 0;
        int iDeltaExRef = 0;
        int iDeltaLoRef = 0;
        int iErrCount = 0;
        QString strErr = "";
        if((0 != iT0PLL1) && (0 != iT1PLL1) && (0 != iT2PLL1))
        {
            iDeltaPLL1 = abs((iT2PLL1-iT1PLL1) - (iT1PLL1-iT0PLL1));
            if(iDeltaPLL1 > iChekc)
            {
                ++iErrCount;
                strErr = tr("10.23M 频率发生跳变\r\n");
            }
        }
        if((0 != iT0PLL2) && (0 != iT1PLL2) && (0 != iT2PLL2))
        {
            iDeltaPLL2 = abs((iT2PLL2-iT1PLL2) - (iT1PLL2-iT0PLL2));
            if(iDeltaPLL2 > iChekc) 
            {
                ++iErrCount;
                strErr = tr("10M 频率发生跳变\r\n");
            }
        }
        if((0 != iT0ExRef) && (0 != iT1ExRef) && (0 != iT2ExRef))
        {
            iDeltaExRef = abs((iT2ExRef-iT1ExRef) - (iT1ExRef-iT0ExRef));
            if(iDeltaExRef > iChekc) 
            {
                ++iErrCount;
                strErr = tr("参考频率发生跳变\r\n");
            }
        }
        if((0 != iT0LoRef) && (0 != iT1LoRef) && (0 != iT2LoRef))
        {
            iDeltaLoRef = abs((iT2LoRef-iT1LoRef) - (iT1LoRef-iT0LoRef));
            if(iDeltaLoRef > iChekc) 
            {
                ++iErrCount;
                strErr = tr("本地参考发生跳变\r\n");
            }
        }

        // 如果单一出现跳变，就报该错误；如果多个出现跳变，就报本地震荡错误
        if(iErrCount > 1)
        {
            strErr = tr("本地振荡器发生跳变\r\n");
        }
        
        if(iErrCount > 0)
        {
            m_pComNetTab->ShowRecvDataFunc(chSourceAddr, chSourcePort, strErr.toLatin1().data(), false, 250, 0, 0);
            m_pComNetTab->CmdLog_Func(chSourceAddr, chSourcePort, strErr);
            StartBeep();
        }

        // Lock
        ShowLock_Func(iLocalLock, iRefLock, i10MLock, i1023MLock, iFreq, iRef, i10M, i1023M);
    }
    else if(NULL != (ptrValue = strstr(m_pRefStaInput_Buf, "output_pps_clk")))
    {
        // output_pps_clk 22 LVTTL_PPS1:Y LVTTL_PPS2:Y
        // output_pps_clk 3 LVDS&RS422_PPS:Y LVDS&RS422_CLK:Y
        bool bHadOutput = true;
        char chType[16];
        char chAddi[16];
        char chName[32];
        char chUseName[1024];
        int iTypeSize = 0;
        char* ptrSave = NULL;
        ptrValue = strstr(ptrValue, ":");
        while(NULL != ptrValue)
        {
            ptr = ptrValue;                 // :
            ptrSave = ptr;
            if('Y' == *(ptrValue+1))        // Y
            {
                bHadOutput = true;
            }
            else if('N' == *(ptrValue+1))   // N
            {
                bHadOutput = false;
            }
            else                            // Error
            {
                break;
            }

            // _
            while((' ' != *ptrValue) && ('_' != *ptrValue)) --ptrValue;
            iTypeSize = ptr-ptrValue-1;
            if(('_' != *ptrValue) || (iTypeSize > 10) || (iTypeSize < 3))       // Error
            {
                break;
            }
            ptr = ptrValue;                 // _

            // PPS CLK
            strncpy_s(chType, sizeof(chType), ptrValue+1, iTypeSize);
            chType[iTypeSize] = '\0';

            // &(可能没有)
            while((' ' != *ptrValue) && ('&' != *ptrValue)) --ptrValue;
            iTypeSize = ptr-ptrValue-1;
            chAddi[0] = '\0';
            if('&' == *ptrValue)
            {
                // RS422
                strncpy_s(chAddi, sizeof(chAddi), ptrValue+1, iTypeSize);
                chAddi[iTypeSize] = '\0';

                // 空格
                ptr = ptrValue;                 // &
                while(' ' != *ptrValue) --ptrValue;
            }

            // 空格(Name)
            iTypeSize = ptr-ptrValue-1;
            strncpy_s(chName, sizeof(chName), ptrValue+1, iTypeSize);
            chName[iTypeSize] = '\0';

            QString strBoradName = m_pComNetTab->GetBoardNameOnAddr(chSourceAddr, chSourcePort);
            sprintf_s(chUseName, sizeof(chUseName), "%s_%s_P%02X%02X", strBoradName.toLatin1().data(), chName, chSourceAddr, chSourcePort);
            
            if(true == bHadOutput)
            {
                //QString strInfo = tr("%1 %2 %3 输出正常\n").arg(chUseName).arg(chAddi).arg(chType);
                //m_pComNetTab->ShowRecvDataFunc(chSourceAddr, chSourcePort, strInfo.toLatin1().data(), false, 0, 0, 250);
            }
            else
            {
                QString strInfo = tr("%1 %2 %3 无输出\n").arg(chUseName).arg(chAddi).arg(chType);
                m_pComNetTab->ShowRecvDataFunc(chSourceAddr, chSourcePort, strInfo.toLatin1().data(), false, 250, 0, 0);
            }
            m_iOutputClk_Count = 0;

            // next
            ptrValue = ptrSave + 1;
            ptrValue = strstr(ptrValue, ":");
        }
    }
}

void CMyImageTab::StartBeep()
{
    m_iBeepDiscSec = 1;
    m_iBeepTimes = 10;
}
void CMyImageTab::StopBeep()
{
    m_iBeepDiscSec = 0;
    m_iBeepTimes = 0;
}

// 新的连接接上，旧的就取消
void CMyImageTab::ResetOldImageConnect(int iSelect)
{
    stImageData* pImageData = &m_TopImageData;
    if(1 == iSelect) pImageData = &m_BottomImageData;

    if(NULL != pImageData->m_pCurSigSelect)
    {
        if(en_Empty_Connect_sta == pImageData->m_pCurSigSelect->m_enSelectStatus)
        {
            pImageData->m_pCurSigSelect->m_enSelectStatus = en_Empty_sta;
        }
        else if(en_Signal_Connect_sta == pImageData->m_pCurSigSelect->m_enSelectStatus)
        {
            pImageData->m_pCurSigSelect->m_enSelectStatus = en_Signal_sta;
        }
    }
}

// 检查是否超出范围
bool CMyImageTab::ValueOutRange(int iIndex, bool bAdd)
{
    bool bIsOut = false;

    int iTotal = 0;
    int iChange = 0;
    QString strText = "";
    switch(iIndex)
    {
    case 0:     // -2000000~2000000uHz
        strText = m_p10MFreqTotao1_LineEdit->text();
        if(!strText.isEmpty()) iTotal = strText.toInt();
        strText = m_p10MFreqDelta1_LineEdit->text();
        if(!strText.isEmpty()) iChange = strText.toInt();
        if(false == bAdd)
        {
            iChange = 0 - iChange;
        }
        iTotal = iTotal + iChange;
        if((iTotal < -2000000) || (iTotal > 2000000)) bIsOut = true;
        break;
    case 1:     // -100~100ns
        strText = m_p10MFreqTotao2_LineEdit->text();
        if(!strText.isEmpty()) iTotal = strText.toInt();
        strText = m_p10MFreqDelta2_LineEdit->text();
        if(!strText.isEmpty()) iChange = strText.toInt();
        if(false == bAdd)
        {
            iChange = 0 - iChange;
        }
        iTotal = iTotal + iChange;
        if((iTotal < -100) || (iTotal > 100)) bIsOut = true;
        break;
    case 2:     // -2000000~2000000uHZ
        strText = m_p1023MPhasTotao1_LineEdit->text();
        if(!strText.isEmpty()) iTotal = strText.toInt();
        strText = m_p1023MPhasDelta1_LineEdit->text();
        if(!strText.isEmpty()) iChange = strText.toInt();
        if(false == bAdd)
        {
            iChange = 0 - iChange;
        }
        iTotal = iTotal + iChange;
        if((iTotal < -2000000) || (iTotal > 2000000)) bIsOut = true;
        break;
    case 3:     // -97~97ns
        strText = m_p1023MPhasTotao2_LineEdit->text();
        if(!strText.isEmpty()) iTotal = strText.toInt();
        strText = m_p1023MPhasDelta2_LineEdit->text();
        if(!strText.isEmpty()) iChange = strText.toInt();
        if(false == bAdd)
        {
            iChange = 0 - iChange;
        }
        iTotal = iTotal + iChange;
        if((iTotal < -97) || (iTotal > 97)) bIsOut = true;
        break;
    case 4:     // 100 ~ 2000000ns
        strText = m_pPlusTotal_LineEdit->text();
        if(!strText.isEmpty()) iTotal = strText.toInt();
        strText = m_pPlusDelta_LineEdit->text();
        if(!strText.isEmpty()) iChange = strText.toInt();
        if(false == bAdd)
        {
            iChange = 0 - iChange;
        }
        iTotal = iTotal + iChange;
        if((iTotal < 100) || (iTotal > 2000000)) bIsOut = true;
        break;
    case 5:     // -999999999~999999999ns
        strText = m_pDelayTotal_LineEdit->text();
        if(!strText.isEmpty()) iTotal = strText.toInt();
        strText = m_pDelayDelta_LineEdit->text();
        if(!strText.isEmpty()) iChange = strText.toInt();
        if(false == bAdd)
        {
            iChange = 0 - iChange;
        }
        iTotal = iTotal + iChange;
        if((iTotal < -999999999) || (iTotal > 999999999)) bIsOut = true;
        break;
    default:
        break;
    }

    return bIsOut;
}

// 通信映射功能
void CMyImageTab::ComNetFunc(int iID)
{
    char chData[1024] = {0};
    unsigned char chPort = 0x00;
    QString strData;
    switch(iID)
    {
    case ID_OF_10M_FREQ_ADD:
        strData = m_p10MFreqDelta1_LineEdit->text();
        if(!strData.isEmpty())
        {
            if(false == ValueOutRange(0, true))
            {
                sprintf_s(chData, sizeof(chData), "setfreq 1 %d", strData.toInt());
                m_pComNetTab->SendChangeData(0x80, 0x00, chData);
            }
            else
            {
                m_pComNetTab->ShowRecvDataFunc(0x60, 0x00, "数值超出范围，调整失败", false, 200, 0, 0);
            }
        }
        break;
    case ID_OF_10M_FREQ_SUB:
        strData = m_p10MFreqDelta1_LineEdit->text();
        if(!strData.isEmpty())
        {
            if(false == ValueOutRange(0, false))
            {
                sprintf_s(chData, sizeof(chData), "setfreq 1 -%d", strData.toInt());
                m_pComNetTab->SendChangeData(0x80, 0x00, chData);
            }
            else
            {
                m_pComNetTab->ShowRecvDataFunc(0x60, 0x00, "数值超出范围，调整失败", false, 200, 0, 0);
            }
        }
        break;
    case ID_OF_10M_FREQ_SAVE:
        strData = m_p10MFreqTotao1_LineEdit->text();
        if(!strData.isEmpty())
        {
            sprintf_s(chData, sizeof(chData), "savfreq 1 %d", strData.toInt());
            m_pComNetTab->SendChangeData(0x80, 0x00, chData);
        }
        break;
    case ID_OF_10M_PHAS_ADD:
        strData = m_p10MFreqDelta2_LineEdit->text();
        if(!strData.isEmpty())
        {
            if(false == ValueOutRange(1, true))
            {
                sprintf_s(chData, sizeof(chData), "setphas 1 %d", strData.toInt());
                m_pComNetTab->SendChangeData(0x80, 0x00, chData);
            }
            else
            {
                m_pComNetTab->ShowRecvDataFunc(0x60, 0x00, "数值超出范围，调整失败", false, 200, 0, 0);
            }
        }
        break;
    case ID_OF_10M_PHAS_SUB:
        strData = m_p10MFreqDelta2_LineEdit->text();
        if(!strData.isEmpty())
        {
            if(false == ValueOutRange(1, false))
            {
                sprintf_s(chData, sizeof(chData), "setphas 1 -%d", strData.toInt());
                m_pComNetTab->SendChangeData(0x80, 0x00, chData);
            }
            else
            {
                m_pComNetTab->ShowRecvDataFunc(0x60, 0x00, "数值超出范围，调整失败", false, 200, 0, 0);
            }
        }
        break;
    case ID_OF_10M_PHAS_SAVE:
        strData = m_p10MFreqTotao2_LineEdit->text();
        if(!strData.isEmpty())
        {
            sprintf_s(chData, sizeof(chData), "savphas 1 %d", strData.toInt());
            m_pComNetTab->SendChangeData(0x80, 0x00, chData);
        }
        break;

    case ID_OF_1023M_FREQ_ADD:
        strData = m_p1023MPhasDelta1_LineEdit->text();
        if(!strData.isEmpty())
        {
            if(false == ValueOutRange(2, true))
            {
                sprintf_s(chData, sizeof(chData), "setfreq 2 %d", strData.toInt());
                m_pComNetTab->SendChangeData(0x80, 0x00, chData);
            }
            else
            {
                m_pComNetTab->ShowRecvDataFunc(0x60, 0x00, "数值超出范围，调整失败", false, 200, 0, 0);
            }
        }
        break;
    case ID_OF_1023M_FREQ_SUB:
        strData = m_p1023MPhasDelta1_LineEdit->text();
        if(!strData.isEmpty())
        {
            if(false == ValueOutRange(2, false))
            {
                sprintf_s(chData, sizeof(chData), "setfreq 2 -%d", strData.toInt());
                m_pComNetTab->SendChangeData(0x80, 0x00, chData);
            }
            else
            {
                m_pComNetTab->ShowRecvDataFunc(0x60, 0x00, "数值超出范围，调整失败", false, 200, 0, 0);
            }
        }
        break;
    case ID_OF_1023M_FREQ_SAVE:
        strData = m_p1023MPhasTotao1_LineEdit->text();
        if(!strData.isEmpty())
        {
            sprintf_s(chData, sizeof(chData), "savfreq 2 %d", strData.toInt());
            m_pComNetTab->SendChangeData(0x80, 0x00, chData);
        }
        break;
    case ID_OF_1023M_PHAS_ADD:
        strData = m_p1023MPhasDelta2_LineEdit->text();
        if(!strData.isEmpty())
        {
            if(false == ValueOutRange(3, true))
            {
                sprintf_s(chData, sizeof(chData), "setphas 2 %d", strData.toInt());
                m_pComNetTab->SendChangeData(0x80, 0x00, chData);
            }
            else
            {
                m_pComNetTab->ShowRecvDataFunc(0x60, 0x00, "数值超出范围，调整失败", false, 200, 0, 0);
            }
        }
        break;
    case ID_OF_1023M_PHAS_SUB:
        strData = m_p1023MPhasDelta2_LineEdit->text();
        if(!strData.isEmpty())
        {
            if(false == ValueOutRange(3, false))
            {
                sprintf_s(chData, sizeof(chData), "setphas 2 -%d", strData.toInt());
                m_pComNetTab->SendChangeData(0x80, 0x00, chData);
            }
            else
            {
                m_pComNetTab->ShowRecvDataFunc(0x60, 0x00, "数值超出范围，调整失败", false, 200, 0, 0);
            }
        }
        break;
    case ID_OF_1023M_PHAS_SAVE:
        strData = m_p1023MPhasTotao2_LineEdit->text();
        if(!strData.isEmpty())
        {
            sprintf_s(chData, sizeof(chData), "savphas 2 %d", strData.toInt());
            m_pComNetTab->SendChangeData(0x80, 0x00, chData);
        }
        break;

    case ID_OF_PLUS_ADD:
        strData = m_pPlusDelta_LineEdit->text();
        if(!strData.isEmpty())
        {
            if(false == ValueOutRange(4, true))
            {
                sprintf_s(chData, sizeof(chData), "pw %d", strData.toInt());
                m_pComNetTab->SendChangeData(0x80, 0x23, chData);
                chPort = 0x23;
            }
            else
            {
                m_pComNetTab->ShowRecvDataFunc(0x60, 0x00, "数值超出范围，调整失败", false, 200, 0, 0);
            }
        }
        break;
    case ID_OF_PLUS_SUB:
        strData = m_pPlusDelta_LineEdit->text();
        if(!strData.isEmpty())
        {
            if(false == ValueOutRange(4, false))
            {
                sprintf_s(chData, sizeof(chData), "pw -%d", strData.toInt());
                m_pComNetTab->SendChangeData(0x80, 0x23, chData);
                chPort = 0x23;
            }
            else
            {
                m_pComNetTab->ShowRecvDataFunc(0x60, 0x00, "数值超出范围，调整失败", false, 200, 0, 0);
            }
        }
        break;

    case ID_OF_DELAY_ADD:
        strData = m_pDelayDelta_LineEdit->text();
        if(!strData.isEmpty())
        {
            if(false == ValueOutRange(5, true))
            {
                sprintf_s(chData, sizeof(chData), "ud %d", strData.toInt());
                m_pComNetTab->SendChangeData(0x80, 0x23, chData);
                chPort = 0x23;
            }
            else
            {
                m_pComNetTab->ShowRecvDataFunc(0x60, 0x00, "数值超出范围，调整失败", false, 200, 0, 0);
            }
        }
        break;
    case ID_OF_DELAY_SUB:
        strData = m_pDelayDelta_LineEdit->text();
        if(!strData.isEmpty())
        {
            if(false == ValueOutRange(5, false))
            {
                sprintf_s(chData, sizeof(chData), "ud -%d", strData.toInt());
                m_pComNetTab->SendChangeData(0x80, 0x23, chData);
                chPort = 0x23;
            }
            else
            {
                m_pComNetTab->ShowRecvDataFunc(0x60, 0x00, "数值超出范围，调整失败", false, 200, 0, 0);
            }
        }
        break;
    case ID_OF_10M_FREQ_CLEAR:                          // 清零 ----------------------------------
        strData = m_p10MFreqTotao1_LineEdit->text();
        if(!strData.isEmpty())
        {
            int iTotal = strData.toInt();
            sprintf_s(chData, sizeof(chData), "setfreq 1 %d", -iTotal);
            m_p10MFreqDelta1_LineEdit->setText("0");
            m_pComNetTab->SendChangeData(0x80, 0x00, chData);
        }
        break;
    case ID_OF_10M_PHAS_CLEAR:
        strData = m_p10MFreqTotao2_LineEdit->text();
        if(!strData.isEmpty())
        {
            int iTotal = strData.toInt();
            sprintf_s(chData, sizeof(chData), "setphas 1 %d", -iTotal);
            m_p10MFreqDelta2_LineEdit->setText("0");
            m_pComNetTab->SendChangeData(0x80, 0x00, chData);
        }
        break;
    case ID_OF_1023M_FREQ_CLEAR:
        strData = m_p1023MPhasTotao1_LineEdit->text();
        if(!strData.isEmpty())
        {
            int iTotal = strData.toInt();
            sprintf_s(chData, sizeof(chData), "setfreq 2 %d", -iTotal);
            m_p1023MPhasDelta1_LineEdit->setText("0");
            m_pComNetTab->SendChangeData(0x80, 0x00, chData);
        }
        break;
    case ID_OF_1023M_PHAS_CLEAR:
        strData = m_p1023MPhasTotao2_LineEdit->text();
        if(!strData.isEmpty())
        {
            int iTotal = strData.toInt();
            sprintf_s(chData, sizeof(chData), "setphas 2 %d", -iTotal);
            m_p1023MPhasDelta2_LineEdit->setText("0");
            m_pComNetTab->SendChangeData(0x80, 0x00, chData);
        }
        break;
    case ID_OF_PLUS_CLEAR:                      // 改为“默认”， 总量为 20000   调节量为 0
        strData = m_pPlusTotal_LineEdit->text();
        if(!strData.isEmpty())
        {
            int iTotal = 20000 - strData.toInt();
            sprintf_s(chData, sizeof(chData), "pw %d", iTotal);
            m_pPlusDelta_LineEdit->setText("0");
            m_pComNetTab->SendChangeData(0x80, 0x23, chData);
            chPort = 0x23;
        }
        break;
    case ID_OF_DELAY_CLEAR:
        strData = m_pDelayTotal_LineEdit->text();
        if(!strData.isEmpty())
        {
            int iTotal = strData.toInt();
            sprintf_s(chData, sizeof(chData), "ud %d", -iTotal);
            m_pDelayDelta_LineEdit->setText("0");
            m_pComNetTab->SendChangeData(0x80, 0x23, chData);
            chPort = 0x23;
        }
        break;
    case ID_OF_REDUCE_ADD_6:                // 衰减 -----------------------------------------------------------
        ReduceClickFunc(true, m_iReduceVal_6, 0x06, 0x00);
        break;
    case ID_OF_REDUCE_SUB_6:
        ReduceClickFunc(false, m_iReduceVal_6, 0x06, 0x00);
        break;
    case ID_OF_REDUCE_ADD_5:
        ReduceClickFunc(true, m_iReduceVal_5, 0x05, 0x00);
        break;
    case ID_OF_REDUCE_SUB_5:
        ReduceClickFunc(false, m_iReduceVal_5, 0x05, 0x00);
        break;
    case ID_OF_REDUCE_ADD_4:
        ReduceClickFunc(true, m_iReduceVal_4, 0x04, 0x00);
        break;
    case ID_OF_REDUCE_SUB_4:
        ReduceClickFunc(false, m_iReduceVal_4, 0x04, 0x00);
        break;
    case ID_OF_REDUCE_ADD_3:
        ReduceClickFunc(true, m_iReduceVal_3, 0x03, 0x00);
        break;
    case ID_OF_REDUCE_SUB_3:
        ReduceClickFunc(false, m_iReduceVal_3, 0x03, 0x00);
        break;
    case ID_OF_REDUCE_ADD_26:
        ReduceClickFunc(true, m_iReduceVal_26, 0x26, 0x00);
        break;
    case ID_OF_REDUCE_SUB_26:
        ReduceClickFunc(false, m_iReduceVal_26, 0x26, 0x00);
        break;
    case ID_OF_REDUCE_ADD_25:
        ReduceClickFunc(true, m_iReduceVal_25, 0x25, 0x00);
        break;
    case ID_OF_REDUCE_SUB_25:
        ReduceClickFunc(false, m_iReduceVal_25, 0x25, 0x00);
        break;
    case ID_OF_REDUCE_ADD_24:
        ReduceClickFunc(true, m_iReduceVal_24, 0x24, 0x00);
        break;
    case ID_OF_REDUCE_SUB_24:
        ReduceClickFunc(false, m_iReduceVal_24, 0x24, 0x00);
        break;
    case ID_OF_REDUCE_ADD_23:
        ReduceClickFunc(true, m_iReduceVal_23, 0x23, 0x00);
        break;
    case ID_OF_REDUCE_SUB_23:
        ReduceClickFunc(false, m_iReduceVal_23, 0x23, 0x00);
        break;
    case ID_OF_ALIGNCOMPSE_ADD:                             // 对齐补偿
        {
            QString strData = m_pAlignCompse_LineEdit->text();
            if(!strData.isEmpty())
            {
                int iShowValue = strData.toInt();
                ++iShowValue;
                if(iShowValue < 0) iShowValue = 0;
                if(iShowValue > 63) iShowValue = 63;
                CompseFunc(iShowValue);
            }
        }
        break;
    case ID_OF_ALIGNCOMPSE_SUB:
        {
            QString strData = m_pAlignCompse_LineEdit->text();
            if(!strData.isEmpty())
            {
                int iShowValue = strData.toInt();
                --iShowValue;
                if(iShowValue < 0) iShowValue = 0;
                if(iShowValue > 63) iShowValue = 63;
                CompseFunc(iShowValue);
            }
        }
        break;
    default:
        break;
    }

    // 保存日志
    if((strlen(chData) > 0) && (NULL != m_pComNetTab))
    {
        QString strLog = chData;
        m_pComNetTab->CmdLog_Func(0x80, chPort, strLog);
    }
}

// 对齐补偿
void CMyImageTab::CompseFunc(int iValue)
{
    char chData[1024] = {0};

    sprintf_s(chData, sizeof(chData), "sy %d",iValue);
    m_pComNetTab->SendChangeData(0x80, 0x23, chData);

    // 保存日志
    if(NULL != m_pComNetTab)
    {
        QString strLog = chData;
        m_pComNetTab->CmdLog_Func(0x80, 0x23, strLog);
    }
}

// 获取 本地频率校准
void CMyImageTab::GetLocalFreqAdj()
{
    char chData[1024] = {0};

    int iValue = 0;

    sprintf_s(chData, sizeof(chData), "freq_add %d",iValue);
    m_pComNetTab->SendChangeData(0x80, 0x00, chData);
}

// 衰减
void CMyImageTab::ReduceClickFunc(bool bIsAdd, int iShowValue_in, int iAddr, int iPort)
{
    char chData[1024] = {0};
    
    if((iShowValue_in <= -5) && (false == bIsAdd))
    {
        QString strInfo = tr("衰减值已经达到最小值 %1，不能继续减小").arg(iShowValue_in);
        m_pComNetTab->ShowRecvDataFunc(iAddr, iPort, strInfo.toLatin1().data(), false, 200, 0, 0);
        return;
    }
    if((iShowValue_in >= 10) && (true == bIsAdd))
    {
        QString strInfo = tr("衰减值已经达到最大值 %1，不能继续增大").arg(iShowValue_in);
        m_pComNetTab->ShowRecvDataFunc(iAddr, iPort, strInfo.toLatin1().data(), false, 200, 0, 0);
        return;
    }

    if(true == bIsAdd)
    {
        ++iShowValue_in;
    }
    else
    {
        --iShowValue_in;
    }
    if(iShowValue_in < -5) iShowValue_in = -5;
    if(iShowValue_in > 10) iShowValue_in = 10;

    sprintf_s(chData, sizeof(chData), "dat %d", iShowValue_in);
    m_pComNetTab->SendChangeData(iAddr, iPort, chData);

    // 保存日志
    if(NULL != m_pComNetTab)
    {
        QString strLog = chData;
        m_pComNetTab->CmdLog_Func((unsigned char)iAddr, (unsigned char)iPort, strLog);
    }
}

void CMyImageTab::ClearAllReduceShow()
{
    m_pReduceShow_6_LineEdit->setText("");
    m_pReduceShow_5_LineEdit->setText("");
    m_pReduceShow_4_LineEdit->setText("");
    m_pReduceShow_3_LineEdit->setText("");
    m_pReduceShow_26_LineEdit->setText("");
    m_pReduceShow_25_LineEdit->setText("");
    m_pReduceShow_24_LineEdit->setText("");
    m_pReduceShow_23_LineEdit->setText("");
    m_pAlignCompse_LineEdit->setText("");
}

// 衰减：显示接收到的数据
void CMyImageTab::SetReduceShowValue(int iValue, unsigned char chSourceAddr, unsigned char chSourcePort)
{
    if((iValue < -5) || (iValue > 10))
    {
        // 注意: “PLL_Universal”这块板卡不显示告警，直接退出
        std::string strBoardName = m_pComNetTab->GetBoardOriName(chSourceAddr, chSourcePort);
        if((strBoardName.length() > 0) && ("PLL_Universal" == strBoardName)) return;

        QString strInfo = tr("接收到的衰减值超出范围 %1").arg(iValue);
        m_pComNetTab->ShowRecvDataFunc(chSourceAddr, chSourcePort, strInfo.toLatin1().data(), false, 200, 0, 0);
        return;
    }

    int iShowValue = iValue;
    if((0x06 == chSourceAddr) && (0 == chSourcePort))
    {
        m_iReduceVal_6 = iShowValue;
        m_pReduceShow_6_LineEdit->setText(QString::number(iShowValue));
    }
    else if((0x05 == chSourceAddr) && (0 == chSourcePort))
    {
        m_iReduceVal_5 = iShowValue;
        m_pReduceShow_5_LineEdit->setText(QString::number(iShowValue));
    }
    else if((0x04 == chSourceAddr) && (0 == chSourcePort))
    {
        m_iReduceVal_4 = iShowValue;
        m_pReduceShow_4_LineEdit->setText(QString::number(iShowValue));
    }
    else if((0x03 == chSourceAddr) && (0 == chSourcePort))
    {
        m_iReduceVal_3 = iShowValue;
        m_pReduceShow_3_LineEdit->setText(QString::number(iShowValue));
    }
    else if((0x26 == chSourceAddr) && (0 == chSourcePort))
    {
        m_iReduceVal_26 = iShowValue;
        m_pReduceShow_26_LineEdit->setText(QString::number(iShowValue));
    }
    else if((0x25 == chSourceAddr) && (0 == chSourcePort))
    {
        m_iReduceVal_25 = iShowValue;
        m_pReduceShow_25_LineEdit->setText(QString::number(iShowValue));
    }
    else if((0x24 == chSourceAddr) && (0 == chSourcePort))
    {
        m_iReduceVal_24 = iShowValue;
        m_pReduceShow_24_LineEdit->setText(QString::number(iShowValue));
    }
    else if((0x23 == chSourceAddr) && (0 == chSourcePort))
    {
        m_iReduceVal_23 = iShowValue;
        m_pReduceShow_23_LineEdit->setText(QString::number(iShowValue));
    }
}

// 获取衰减量
void CMyImageTab::SendReduceCmdGetInfo()
{
    int iMS = SEND_CMD_DELAY_MS;
    char chData[1024] = {0};
    strcpy_s(chData, sizeof(chData), "dat");
    int iAddr = 0x06;
    int iPort = 0x00;
    m_pComNetTab->SendChangeData(iAddr, iPort, chData);
    DelayMS(iMS);
    m_pComNetTab->SendChangeData(iAddr, iPort, chData);
    DelayMS(iMS);
    iAddr = 0x05;
    iPort = 0x00;
    m_pComNetTab->SendChangeData(iAddr, iPort, chData);
    DelayMS(iMS);
    m_pComNetTab->SendChangeData(iAddr, iPort, chData);
    DelayMS(iMS);
    iAddr = 0x04;
    iPort = 0x00;
    m_pComNetTab->SendChangeData(iAddr, iPort, chData);
    DelayMS(iMS);
    m_pComNetTab->SendChangeData(iAddr, iPort, chData);
    DelayMS(iMS);
    iAddr = 0x03;
    iPort = 0x00;
    m_pComNetTab->SendChangeData(iAddr, iPort, chData);
    DelayMS(iMS);
    m_pComNetTab->SendChangeData(iAddr, iPort, chData);
    DelayMS(iMS);

    iAddr = 0x26;
    iPort = 0x00;
    m_pComNetTab->SendChangeData(iAddr, iPort, chData);
    DelayMS(iMS);
    m_pComNetTab->SendChangeData(iAddr, iPort, chData);
    DelayMS(iMS);
    iAddr = 0x25;
    iPort = 0x00;
    m_pComNetTab->SendChangeData(iAddr, iPort, chData);
    DelayMS(iMS);
    m_pComNetTab->SendChangeData(iAddr, iPort, chData);
    DelayMS(iMS);
    iAddr = 0x24;
    iPort = 0x00;
    m_pComNetTab->SendChangeData(iAddr, iPort, chData);
    DelayMS(iMS);
    m_pComNetTab->SendChangeData(iAddr, iPort, chData);
    DelayMS(iMS);
    iAddr = 0x23;
    iPort = 0x00;
    m_pComNetTab->SendChangeData(iAddr, iPort, chData);
    DelayMS(iMS);
    m_pComNetTab->SendChangeData(iAddr, iPort, chData);
    DelayMS(iMS);
}

void CMyImageTab::AlignFunc()
{
    char chData[1024] = {0};
    sprintf_s(chData, sizeof(chData), "sy");
    m_pComNetTab->SendChangeData(0x80, 0x00, chData);
}

// 发送减0命令，获取调节总量等信息
void CMyImageTab::SendSubZeroForGetInfo()
{
    int iMS = SEND_CMD_DELAY_MS;

    m_pPlusDelta_LineEdit->setText("0");
    ComNetFunc(ID_OF_PLUS_SUB);
    DelayMS(iMS);
    ComNetFunc(ID_OF_PLUS_SUB);
    DelayMS(iMS);

    m_p10MFreqDelta1_LineEdit->setText("0");
    ComNetFunc(ID_OF_10M_FREQ_SUB);
    DelayMS(iMS);
    ComNetFunc(ID_OF_10M_FREQ_SUB);
    DelayMS(iMS);

    m_p10MFreqDelta2_LineEdit->setText("0");
    ComNetFunc(ID_OF_10M_PHAS_SUB);
    DelayMS(iMS);
    ComNetFunc(ID_OF_10M_PHAS_SUB);
    DelayMS(iMS);

    m_p1023MPhasDelta1_LineEdit->setText("0");
    ComNetFunc(ID_OF_1023M_FREQ_SUB);
    DelayMS(iMS);
    ComNetFunc(ID_OF_1023M_FREQ_SUB);
    DelayMS(iMS);

    m_p1023MPhasDelta2_LineEdit->setText("0");
    ComNetFunc(ID_OF_1023M_PHAS_SUB);
    DelayMS(iMS);
    ComNetFunc(ID_OF_1023M_PHAS_SUB);
    DelayMS(iMS);

    m_pDelayDelta_LineEdit->setText("0");
    ComNetFunc(ID_OF_DELAY_SUB);
    DelayMS(iMS);
    ComNetFunc(ID_OF_DELAY_SUB);
    DelayMS(iMS);
}


void CMyImageTab::DelayMS(int iMS)
{
    QTime t;
    t.start();
    while(t.elapsed() < iMS)
    {
        QCoreApplication::processEvents(); 
    }
}

// 优先级
void CMyImageTab::PriorityQuery_Slot()
{
    if(m_iPrioSetTimeout_Count > 0)
    {
        ShowPrioInfo_Func(tr("请等候 %1 秒").arg(m_iPrioSetTimeout_Count), false);
        return;
    }

    // 获取优先级
    ShowPrioInfo_Func(tr("下发 优先级查询 命令"), false);
    m_iPrioCmdTimeout_Count = 10;

    SetPriorityCtrl(false);

    m_bQueryPriority = true;

    char chData[1024] = {0};
    strcpy_s(chData, sizeof(chData), "sta");
    m_pComNetTab->SendChangeData(0x80, 0x00, chData);
}
void CMyImageTab::PrioritySet_Slot()
{
    if(m_iPrioSetTimeout_Count > 0)
    {
        ShowPrioInfo_Func(tr("请等候 %1 秒").arg(m_iPrioSetTimeout_Count), false);
        return;
    }

    int iPriority1 = m_pPriority1_ComboBox->currentIndex() + 1;
    int iPriority2 = m_pPriority2_ComboBox->currentIndex() + 1;
    int iPriority3 = m_pPriority3_ComboBox->currentIndex() + 1;
    int iPriority4 = m_pPriority4_ComboBox->currentIndex() + 1;

    int iPriority5 = m_pPriority5_ComboBox->currentIndex() + 1;
    int iPriority6 = m_pPriority6_ComboBox->currentIndex() + 1;

    int iPriorityType = m_pPriorityType_ComboBox->currentIndex() + 1;

    int CheckArray[4] = {0};
    CheckArray[iPriority1-1] = 1;
    CheckArray[iPriority2-1] = 1;
    CheckArray[iPriority3-1] = 1;
    CheckArray[iPriority4-1] = 1;

    //if(iPriorityType > 1)     // 改：所有模式都检测
    {
        for(int i = 0; i < 4; ++i)
        {
            if(0 == CheckArray[i])
            {
                ShowPrioInfo_Func(tr("优先级重复，请重新选择"), true);
                return;
            }
        }

        // 优先级的数字不能重复
        if(iPriority5 == iPriority6)
        {
            ShowPrioInfo_Func(tr("优先级重复，请重新选择"), true);
            return;
        }
    }

    SetPriorityCtrl(false);

    // 设置优先级
    ShowPrioInfo_Func(tr("下发 优先级设置 命令"), false);

    // 转换
    int PriorityArray[5] = {0};
    for(int i = 1; i < 5; ++i)
    {
        if(i == iPriority1)
        {
            PriorityArray[i] = 1;
            continue;
        }
        if(i == iPriority2)
        {
            PriorityArray[i] = 2;
            continue;
        }
        if(i == iPriority3)
        {
            PriorityArray[i] = 3;
            continue;
        }
        if(i == iPriority4)
        {
            PriorityArray[i] = 4;
            continue;
        }
    }
    if(iPriority6 < iPriority5)
    {
        iPriority5 = 2;
        iPriority6 = 1;
    }

    m_iPrioSetTimeout_Count = 10;
    m_iPrioCmdTimeout_Count = 10;
    m_bQueryPriority = false;

    char chData[1024] = {0};
    sprintf_s(chData, sizeof(chData), "priority %d%d%d%d%d%d %d", 
                                       PriorityArray[1], PriorityArray[2], PriorityArray[3], PriorityArray[4], iPriority5, iPriority6, iPriorityType);
    m_pComNetTab->SendChangeData(0x80, 0x00, chData);

    // 保存日志
    if(NULL != m_pComNetTab)
    {
        QString strLog = chData;
        m_pComNetTab->CmdLog_Func(0x80, 0x00, strLog);
    }
}

// 设置优先级
void CMyImageTab::SetPriority(int iPriority1, int iPriority2, int iPriority3, int iPriority4, int iPriority5, int iPriority6, int iPriorityType)
{
    // 转换
    int PriorityArray[5] = {0};
    PriorityArray[1] = iPriority1;
    PriorityArray[2] = iPriority2;
    PriorityArray[3] = iPriority3;
    PriorityArray[4] = iPriority4;
    for(int i = 1; i < 5; ++i)
    {
        if(1 == PriorityArray[i])
        {
            iPriority1 = i;
            continue;
        }
        if(2 == PriorityArray[i])
        {
            iPriority2 = i;
            continue;
        }
        if(3 == PriorityArray[i])
        {
            iPriority3 = i;
            continue;
        }
        if(4 == PriorityArray[i])
        {
            iPriority4 = i;
            continue;
        }
    }
    if(iPriority5 > iPriority6)
    {
        iPriority6 = 1;
        iPriority5 = 2;
    }

    m_iPriority1_New = iPriority1;
    m_iPriority2_New = iPriority2;
    m_iPriority3_New = iPriority3;
    m_iPriority4_New = iPriority4;
    m_iPriority5_New = iPriority5;
    m_iPriority6_New = iPriority6;
    m_iPriorityType_New = iPriorityType;

    m_pPriority1_ComboBox->setCurrentIndex(m_iPriority1_New-1);
    m_pPriority2_ComboBox->setCurrentIndex(m_iPriority2_New-1);
    m_pPriority3_ComboBox->setCurrentIndex(m_iPriority3_New-1);
    m_pPriority4_ComboBox->setCurrentIndex(m_iPriority4_New-1);

    m_pPriority5_ComboBox->setCurrentIndex(m_iPriority5_New-1);
    m_pPriority6_ComboBox->setCurrentIndex(m_iPriority6_New-1);

    m_pPriorityType_ComboBox->setCurrentIndex(m_iPriorityType_New-1);

    m_iPriority1_Cur = m_iPriority1_New;
    m_iPriority2_Cur = m_iPriority2_New;
    m_iPriority3_Cur = m_iPriority3_New;
    m_iPriority4_Cur = m_iPriority4_New;
    m_iPriority5_Cur = m_iPriority5_New;
    m_iPriority6_Cur = m_iPriority6_New;
    m_iPriorityType_Cur = m_iPriorityType_New;
}

// 设置频率或者相位等调整的数值
void CMyImageTab::SetFreqAndPhaseEdit(int iIndex, char* pchData)
{
    switch(iIndex)
    {
    case 0:
        m_p10MFreqTotao1_LineEdit->setText(pchData);
        m_p10MFreqDelta1_LineEdit->setText("0");
        break;
    case 1:
        m_p1023MPhasTotao1_LineEdit->setText(pchData);
        m_p1023MPhasDelta1_LineEdit->setText("0");
        break;
    case 2:
        m_p10MFreqTotao2_LineEdit->setText(pchData);
        m_p10MFreqDelta2_LineEdit->setText("0");
        break;
    case 3:
        m_p1023MPhasTotao2_LineEdit->setText(pchData);
        m_p1023MPhasDelta2_LineEdit->setText("0");
        break;
    case 4:
        m_pPlusTotal_LineEdit->setText(pchData);
        m_pPlusDelta_LineEdit->setText("0");
        break;
    case 5:
        m_pDelayTotal_LineEdit->setText(pchData);
        m_pDelayDelta_LineEdit->setText("0");
        break;
    case 6:
        m_pAlignCompse_LineEdit->setText(pchData);
        break;
    default:
        break;
    }
}

// 优先级控件
void CMyImageTab::SetPriorityCtrl(bool bEnable)
{
    m_pPriority1_ComboBox->setEnabled(bEnable);
    m_pPriority2_ComboBox->setEnabled(bEnable);
    m_pPriority3_ComboBox->setEnabled(bEnable);
    m_pPriority4_ComboBox->setEnabled(bEnable);
    m_pPriority5_ComboBox->setEnabled(bEnable);
    m_pPriority6_ComboBox->setEnabled(bEnable);
    m_pPriorityType_ComboBox->setEnabled(bEnable);
    m_pPrioritySet_Button->setEnabled(bEnable);
    m_pPriorityQuery_Button->setEnabled(bEnable);
}

void CMyImageTab::ShowLock_Func(int iLocalLock, int iRefLock, int i10MLock, int i1023MLock, 
                                int iLocalFreq, int iRefFreq, int i10MFreq, int i1023MFreq)
{
    emit ShowLock_Signal(iLocalLock, iRefLock, i10MLock, i1023MLock, iLocalFreq, iRefFreq, i10MFreq, i1023MFreq);
}
void CMyImageTab::ShowLock_Slot(int iLocalLock, int iRefLock, int i10MLock, int i1023MLock,
                                int iLocalFreq, int iRefFreq, int i10MFreq, int i1023MFreq)
{
    if(iLocalLock <= 0)
    {
        m_pInFreqPic_Label->setPixmap(QPixmap(":/images/unlock.png"));
    }
    else
    {
        m_pInFreqPic_Label->setPixmap(QPixmap(":/images/lock.png"));
    }
    if(iRefLock <= 0)
    {
        m_pRefFreqPic_Label->setPixmap(QPixmap(":/images/unlock.png"));
    }
    else
    {
        m_pRefFreqPic_Label->setPixmap(QPixmap(":/images/lock.png"));
    }
    if(i10MLock <= 0)
    {
        m_p10MFreqPic_Label->setPixmap(QPixmap(":/images/unlock.png"));
    }
    else
    {
        m_p10MFreqPic_Label->setPixmap(QPixmap(":/images/lock.png"));
    }
    if(i1023MLock <= 0)
    {
        m_p1023MFreqPic_Label->setPixmap(QPixmap(":/images/unlock.png"));
    }
    else
    {
        m_p1023MFreqPic_Label->setPixmap(QPixmap(":/images/lock.png"));
    }

    m_pInFreqVal_Label->setText(QString::number(iLocalFreq));
    m_pRefFreqVal_Label->setText(QString::number(iRefFreq));
    m_p10MFreqVal_Label->setText(QString::number(i10MFreq));
    m_p1023MFreqVal_Label->setText(QString::number(i1023MFreq));
}


