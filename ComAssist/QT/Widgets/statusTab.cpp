#include "stdafx.h"
#include "statusTab.h"

#include <QtGui>

#include "../../../Common/Function/Function.h"
#include "../../../Common/Define/DefineFile.h"
#include "../../../Libase/Libase.h"

// --------------------------------------------------------------------------------------
// 状态/监控 tab
// --------------------------------------------------------------------------------------
CStatusTab::CStatusTab(QWidget* parent) : QWidget(parent)
{
    m_pMyComBasic = NULL;

    connect(this, SIGNAL(HandleFrame_Signal(const QString&, unsigned char, unsigned char)), this, SLOT(HandleFrame_Slot(const QString&, unsigned char, unsigned char)));

    m_pRefStaInput_Buf = new char[SIZE_OF_INPUT_BUF];

    CreateWidgets();
}
CStatusTab::~CStatusTab()
{
    delete[] m_pRefStaInput_Buf;
}

void CStatusTab::paintEvent(QPaintEvent*)
{
    // Style Sheet
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CStatusTab::SetComBasic(CMyComBasic* pComBasic)
{
    m_pMyComBasic = pComBasic;
}

void CStatusTab::CreateWidgets()
{
    QVBoxLayout* pVLayoutPLL = new QVBoxLayout();
    pVLayoutPLL->addWidget(CreatePLL10MGroup());
    pVLayoutPLL->addWidget(CreatePLL1023Group());

    QHBoxLayout* pTopHLayout = new QHBoxLayout();
    pTopHLayout->addWidget(CreateMainCtrlGroup());
    pTopHLayout->addLayout(pVLayoutPLL);

    QVBoxLayout* pLeftVLayout = new QVBoxLayout();
    pLeftVLayout->addWidget(CreateAddr06Group());
    pLeftVLayout->addWidget(CreateAddr05Group());
    pLeftVLayout->addWidget(CreateAddr04Group());
    pLeftVLayout->addWidget(CreateAddr03Group());
    pLeftVLayout->addWidget(CreateAddr02Group());
    pLeftVLayout->addWidget(CreateAddr01Group());

    QVBoxLayout* pRightVLayout = new QVBoxLayout();
    pRightVLayout->addWidget(CreateAddr26Group());
    pRightVLayout->addWidget(CreateAddr25Group());
    pRightVLayout->addWidget(CreateAddr24Group());
    pRightVLayout->addWidget(CreateAddr23Group());
    pRightVLayout->addWidget(CreateAddr22Group());
    pRightVLayout->addWidget(CreateAddr21Group());

    QHBoxLayout* pBottomHLayout = new QHBoxLayout();
    pBottomHLayout->addLayout(pLeftVLayout);
    pBottomHLayout->addLayout(pRightVLayout);

    QVBoxLayout* pMainVLayout = new QVBoxLayout();
    pMainVLayout->addLayout(pTopHLayout);
    pMainVLayout->addLayout(pBottomHLayout);
    setLayout(pMainVLayout);
}

QGroupBox* CStatusTab::CreateMainCtrlGroup()
{
    QGroupBox* pGroup = new QGroupBox(tr("Main Control"));

    QLabel* pLabelV1 = new QLabel(tr("主电源电压"));
    pLabelV1->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pMainCtrl_MainPowerVol = new QLineEdit();
    m_pMainCtrl_MainPowerVol->setReadOnly(true);
    QLabel* pLabelV2 = new QLabel(tr("备电源电压"));
    pLabelV2->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pMainCtrl_BackupPowerVol = new QLineEdit();
    m_pMainCtrl_BackupPowerVol->setReadOnly(true);
    QLabel* pLabelV3 = new QLabel(tr("内参考时钟电压"));
    pLabelV3->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pMainCtrl_RefClockVol = new QLineEdit();
    m_pMainCtrl_RefClockVol->setReadOnly(true);
    QLabel* pLabelV4 = new QLabel(tr("主板电压"));
    pLabelV4->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pMainCtrl_MainBoardVol = new QLineEdit();
    m_pMainCtrl_MainBoardVol->setReadOnly(true);

    QLabel* pLabelS1 = new QLabel(tr("PLL1电流"));
    pLabelS1->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pMainCtrl_PLL1An = new QLineEdit();
    m_pMainCtrl_PLL1An->setReadOnly(true);
    QLabel* pLabelS2 = new QLabel(tr("PLL2电流"));
    pLabelS2->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pMainCtrl_PLL2An = new QLineEdit();
    m_pMainCtrl_PLL2An->setReadOnly(true);
    QLabel* pLabelS3 = new QLabel(tr("内参考时钟电流"));
    pLabelS3->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pMainCtrl_RefClockAn = new QLineEdit();
    m_pMainCtrl_RefClockAn->setReadOnly(true);
    QLabel* pLabelS4 = new QLabel(tr("主板电流"));
    pLabelS4->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pMainCtrl_MainBoardAn = new QLineEdit();
    m_pMainCtrl_MainBoardAn->setReadOnly(true);

    QGridLayout* pGridLayout = new QGridLayout();
    pGridLayout->addWidget(pLabelV1, 0, 0);
    pGridLayout->addWidget(m_pMainCtrl_MainPowerVol, 0, 1);
    pGridLayout->addWidget(pLabelV4, 0, 2);
    pGridLayout->addWidget(m_pMainCtrl_MainBoardVol, 0, 3);

    pGridLayout->addWidget(pLabelV2, 1, 0);
    pGridLayout->addWidget(m_pMainCtrl_BackupPowerVol, 1, 1);
    pGridLayout->addWidget(pLabelS4, 1, 2);
    pGridLayout->addWidget(m_pMainCtrl_MainBoardAn, 1, 3);

    pGridLayout->addWidget(pLabelS1, 2, 0);
    pGridLayout->addWidget(m_pMainCtrl_PLL1An, 2, 1);
    pGridLayout->addWidget(pLabelV3, 2, 2);
    pGridLayout->addWidget(m_pMainCtrl_RefClockVol, 2, 3);

    pGridLayout->addWidget(pLabelS2, 3, 0);
    pGridLayout->addWidget(m_pMainCtrl_PLL2An, 3, 1);
    pGridLayout->addWidget(pLabelS3, 3, 2);
    pGridLayout->addWidget(m_pMainCtrl_RefClockAn, 3, 3);

    pGroup->setLayout(pGridLayout);
    return pGroup;
}
QGroupBox* CStatusTab::CreatePLL10MGroup()
{
    QGroupBox* pGroup = new QGroupBox(tr("PLL 10.00M"));

    QLabel* pLabel1 = new QLabel(tr("板卡温度"));
    pLabel1->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pPLL10M_BoardTemp = new QLineEdit();
    m_pPLL10M_BoardTemp->setReadOnly(true);

    QLabel* pLabel2 = new QLabel(tr("工作电压"));
    pLabel2->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pPLL10M_WorkVol = new QLineEdit();
    m_pPLL10M_WorkVol->setReadOnly(true);

    QLabel* pLabel3 = new QLabel(tr("频率锁定"));
    pLabel3->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pPLL10M_FreqLock = new QLineEdit();
    m_pPLL10M_FreqLock->setReadOnly(true);

    QLabel* pLabel4 = new QLabel(tr("恒温槽温度"));
    pLabel4->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pPLL10M_ConstCTemp = new QLineEdit();
    m_pPLL10M_ConstCTemp->setReadOnly(true);

    QLabel* pLabel5 = new QLabel(tr("电源温度"));
    pLabel5->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pPLL10M_PowerTemp = new QLineEdit();
    m_pPLL10M_PowerTemp->setReadOnly(true);

    QGridLayout* pGridLayout = new QGridLayout();
    pGridLayout->addWidget(pLabel1, 0, 0);
    pGridLayout->addWidget(m_pPLL10M_BoardTemp, 0, 1);
    pGridLayout->addWidget(pLabel2, 0, 2);
    pGridLayout->addWidget(m_pPLL10M_WorkVol, 0, 3);

    pGridLayout->addWidget(pLabel5, 1, 0);
    pGridLayout->addWidget(m_pPLL10M_PowerTemp, 1, 1);
    pGridLayout->addWidget(pLabel3, 1, 2);
    pGridLayout->addWidget(m_pPLL10M_FreqLock, 1, 3);

    pGridLayout->addWidget(pLabel4, 2, 0);
    pGridLayout->addWidget(m_pPLL10M_ConstCTemp, 2, 1);

    pGroup->setLayout(pGridLayout);
    return pGroup;
}
QGroupBox* CStatusTab::CreatePLL1023Group()
{
    QGroupBox* pGroup = new QGroupBox(tr("PLL 10.23M"));

    QLabel* pLabel1 = new QLabel(tr("板卡温度"));
    pLabel1->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pPLL1023M_BoardTemp = new QLineEdit();
    m_pPLL1023M_BoardTemp->setReadOnly(true);

    QLabel* pLabel2 = new QLabel(tr("工作电压"));
    pLabel2->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pPLL1023M_WorkVol = new QLineEdit();
    m_pPLL1023M_WorkVol->setReadOnly(true);

    QLabel* pLabel3 = new QLabel(tr("频率锁定"));
    pLabel3->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pPLL1023M_FreqLock = new QLineEdit();
    m_pPLL1023M_FreqLock->setReadOnly(true);

    QLabel* pLabel4 = new QLabel(tr("恒温槽温度"));
    pLabel4->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pPLL1023M_ConstCTemp = new QLineEdit();
    m_pPLL1023M_ConstCTemp->setReadOnly(true);

    QLabel* pLabel5 = new QLabel(tr("电源温度"));
    pLabel5->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pPLL1023M_PowerTemp = new QLineEdit();
    m_pPLL1023M_PowerTemp->setReadOnly(true);

    QGridLayout* pGridLayout = new QGridLayout();
    pGridLayout->addWidget(pLabel1, 0, 0);
    pGridLayout->addWidget(m_pPLL1023M_BoardTemp, 0, 1);
    pGridLayout->addWidget(pLabel2, 0, 2);
    pGridLayout->addWidget(m_pPLL1023M_WorkVol, 0, 3);

    pGridLayout->addWidget(pLabel5, 1, 0);
    pGridLayout->addWidget(m_pPLL1023M_PowerTemp, 1, 1);
    pGridLayout->addWidget(pLabel3, 1, 2);
    pGridLayout->addWidget(m_pPLL1023M_FreqLock, 1, 3);

    pGridLayout->addWidget(pLabel4, 2, 0);
    pGridLayout->addWidget(m_pPLL1023M_ConstCTemp, 2, 1);

    pGroup->setLayout(pGridLayout);

    return pGroup;
}

QGroupBox* CStatusTab::CreateAddr06Group()
{
    QGroupBox* pGroup = new QGroupBox(tr("板卡[6, 0]"));

    QLabel* pLabel1 = new QLabel(tr("工作电压"));
    m_pAddr06_WorkVol = new QLineEdit();
    m_pAddr06_WorkVol->setReadOnly(true);

    QLabel* pLabel2 = new QLabel(tr("工作电流"));
    m_pAddr06_WorkAn = new QLineEdit();
    m_pAddr06_WorkAn->setReadOnly(true);

    QLabel* pLabel3 = new QLabel(tr("板卡温度"));
    m_pAddr06_BoardTemp = new QLineEdit();
    m_pAddr06_BoardTemp->setReadOnly(true);

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(pLabel1);
    pHLayout->addWidget(m_pAddr06_WorkVol);
    pHLayout->addWidget(pLabel2);
    pHLayout->addWidget(m_pAddr06_WorkAn);
    pHLayout->addWidget(pLabel3);
    pHLayout->addWidget(m_pAddr06_BoardTemp);

    pGroup->setLayout(pHLayout);
    return pGroup;
}
QGroupBox* CStatusTab::CreateAddr05Group()
{
    QGroupBox* pGroup = new QGroupBox(tr("板卡[5, 0]"));

    QLabel* pLabel1 = new QLabel(tr("工作电压"));
    m_pAddr05_WorkVol = new QLineEdit();
    m_pAddr05_WorkVol->setReadOnly(true);

    QLabel* pLabel2 = new QLabel(tr("工作电流"));
    m_pAddr05_WorkAn = new QLineEdit();
    m_pAddr05_WorkAn->setReadOnly(true);

    QLabel* pLabel3 = new QLabel(tr("板卡温度"));
    m_pAddr05_BoardTemp = new QLineEdit();
    m_pAddr05_BoardTemp->setReadOnly(true);

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(pLabel1);
    pHLayout->addWidget(m_pAddr05_WorkVol);
    pHLayout->addWidget(pLabel2);
    pHLayout->addWidget(m_pAddr05_WorkAn);
    pHLayout->addWidget(pLabel3);
    pHLayout->addWidget(m_pAddr05_BoardTemp);

    pGroup->setLayout(pHLayout);
    return pGroup;
}
QGroupBox* CStatusTab::CreateAddr04Group()
{
    QGroupBox* pGroup = new QGroupBox(tr("板卡[4, 0]"));

    QLabel* pLabel1 = new QLabel(tr("工作电压"));
    m_pAddr04_WorkVol = new QLineEdit();
    m_pAddr04_WorkVol->setReadOnly(true);

    QLabel* pLabel2 = new QLabel(tr("工作电流"));
    m_pAddr04_WorkAn = new QLineEdit();
    m_pAddr04_WorkAn->setReadOnly(true);

    QLabel* pLabel3 = new QLabel(tr("板卡温度"));
    m_pAddr04_BoardTemp = new QLineEdit();
    m_pAddr04_BoardTemp->setReadOnly(true);

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(pLabel1);
    pHLayout->addWidget(m_pAddr04_WorkVol);
    pHLayout->addWidget(pLabel2);
    pHLayout->addWidget(m_pAddr04_WorkAn);
    pHLayout->addWidget(pLabel3);
    pHLayout->addWidget(m_pAddr04_BoardTemp);

    pGroup->setLayout(pHLayout);
    return pGroup;
}
QGroupBox* CStatusTab::CreateAddr03Group()
{
    QGroupBox* pGroup = new QGroupBox(tr("板卡[3, 0]"));

    QLabel* pLabel1 = new QLabel(tr("工作电压"));
    m_pAddr03_WorkVol = new QLineEdit();
    m_pAddr03_WorkVol->setReadOnly(true);

    QLabel* pLabel2 = new QLabel(tr("工作电流"));
    m_pAddr03_WorkAn = new QLineEdit();
    m_pAddr03_WorkAn->setReadOnly(true);

    QLabel* pLabel3 = new QLabel(tr("板卡温度"));
    m_pAddr03_BoardTemp = new QLineEdit();
    m_pAddr03_BoardTemp->setReadOnly(true);

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(pLabel1);
    pHLayout->addWidget(m_pAddr03_WorkVol);
    pHLayout->addWidget(pLabel2);
    pHLayout->addWidget(m_pAddr03_WorkAn);
    pHLayout->addWidget(pLabel3);
    pHLayout->addWidget(m_pAddr03_BoardTemp);

    pGroup->setLayout(pHLayout);
    return pGroup;
}
QGroupBox* CStatusTab::CreateAddr02Group()
{
    QGroupBox* pGroup = new QGroupBox(tr("板卡[2, 0]"));

    QLabel* pLabel1 = new QLabel(tr("工作电压"));
    m_pAddr02_WorkVol = new QLineEdit();
    m_pAddr02_WorkVol->setReadOnly(true);

    QLabel* pLabel2 = new QLabel(tr("工作电流"));
    m_pAddr02_WorkAn = new QLineEdit();
    m_pAddr02_WorkAn->setReadOnly(true);

    QLabel* pLabel3 = new QLabel(tr("板卡温度"));
    m_pAddr02_BoardTemp = new QLineEdit();
    m_pAddr02_BoardTemp->setReadOnly(true);

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(pLabel1);
    pHLayout->addWidget(m_pAddr02_WorkVol);
    pHLayout->addWidget(pLabel2);
    pHLayout->addWidget(m_pAddr02_WorkAn);
    pHLayout->addWidget(pLabel3);
    pHLayout->addWidget(m_pAddr02_BoardTemp);

    pGroup->setLayout(pHLayout);
    return pGroup;
}
QGroupBox* CStatusTab::CreateAddr01Group()
{
    QGroupBox* pGroup = new QGroupBox(tr("板卡[1, 0]"));

    QLabel* pLabel1 = new QLabel(tr("工作电压"));
    m_pAddr01_WorkVol = new QLineEdit();
    m_pAddr01_WorkVol->setReadOnly(true);

    QLabel* pLabel2 = new QLabel(tr("工作电流"));
    m_pAddr01_WorkAn = new QLineEdit();
    m_pAddr01_WorkAn->setReadOnly(true);

    QLabel* pLabel3 = new QLabel(tr("板卡温度"));
    m_pAddr01_BoardTemp = new QLineEdit();
    m_pAddr01_BoardTemp->setReadOnly(true);

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(pLabel1);
    pHLayout->addWidget(m_pAddr01_WorkVol);
    pHLayout->addWidget(pLabel2);
    pHLayout->addWidget(m_pAddr01_WorkAn);
    pHLayout->addWidget(pLabel3);
    pHLayout->addWidget(m_pAddr01_BoardTemp);

    pGroup->setLayout(pHLayout);
    return pGroup;
}

QGroupBox* CStatusTab::CreateAddr26Group()
{
    QGroupBox* pGroup = new QGroupBox(tr("板卡[26, 0]"));

    QLabel* pLabel1 = new QLabel(tr("工作电压"));
    m_pAddr26_WorkVol = new QLineEdit();
    m_pAddr26_WorkVol->setReadOnly(true);

    QLabel* pLabel2 = new QLabel(tr("工作电流"));
    m_pAddr26_WorkAn = new QLineEdit();
    m_pAddr26_WorkAn->setReadOnly(true);

    QLabel* pLabel3 = new QLabel(tr("板卡温度"));
    m_pAddr26_BoardTemp = new QLineEdit();
    m_pAddr26_BoardTemp->setReadOnly(true);

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(pLabel1);
    pHLayout->addWidget(m_pAddr26_WorkVol);
    pHLayout->addWidget(pLabel2);
    pHLayout->addWidget(m_pAddr26_WorkAn);
    pHLayout->addWidget(pLabel3);
    pHLayout->addWidget(m_pAddr26_BoardTemp);

    pGroup->setLayout(pHLayout);
    return pGroup;
}
QGroupBox* CStatusTab::CreateAddr25Group()
{
    QGroupBox* pGroup = new QGroupBox(tr("板卡[25, 0]"));

    QLabel* pLabel1 = new QLabel(tr("工作电压"));
    m_pAddr25_WorkVol = new QLineEdit();
    m_pAddr25_WorkVol->setReadOnly(true);

    QLabel* pLabel2 = new QLabel(tr("工作电流"));
    m_pAddr25_WorkAn = new QLineEdit();
    m_pAddr25_WorkAn->setReadOnly(true);

    QLabel* pLabel3 = new QLabel(tr("板卡温度"));
    m_pAddr25_BoardTemp = new QLineEdit();
    m_pAddr25_BoardTemp->setReadOnly(true);

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(pLabel1);
    pHLayout->addWidget(m_pAddr25_WorkVol);
    pHLayout->addWidget(pLabel2);
    pHLayout->addWidget(m_pAddr25_WorkAn);
    pHLayout->addWidget(pLabel3);
    pHLayout->addWidget(m_pAddr25_BoardTemp);

    pGroup->setLayout(pHLayout);
    return pGroup;
}
QGroupBox* CStatusTab::CreateAddr24Group()
{
    QGroupBox* pGroup = new QGroupBox(tr("板卡[24, 0]"));

    QLabel* pLabel1 = new QLabel(tr("工作电压"));
    m_pAddr24_WorkVol = new QLineEdit();
    m_pAddr24_WorkVol->setReadOnly(true);

    QLabel* pLabel2 = new QLabel(tr("工作电流"));
    m_pAddr24_WorkAn = new QLineEdit();
    m_pAddr24_WorkAn->setReadOnly(true);

    QLabel* pLabel3 = new QLabel(tr("板卡温度"));
    m_pAddr24_BoardTemp = new QLineEdit();
    m_pAddr24_BoardTemp->setReadOnly(true);

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(pLabel1);
    pHLayout->addWidget(m_pAddr24_WorkVol);
    pHLayout->addWidget(pLabel2);
    pHLayout->addWidget(m_pAddr24_WorkAn);
    pHLayout->addWidget(pLabel3);
    pHLayout->addWidget(m_pAddr24_BoardTemp);

    pGroup->setLayout(pHLayout);
    return pGroup;
}
QGroupBox* CStatusTab::CreateAddr23Group()
{
    QGroupBox* pGroup = new QGroupBox(tr("板卡[23, 0]"));

    QLabel* pLabel1 = new QLabel(tr("工作电压"));
    m_pAddr23_WorkVol = new QLineEdit();
    m_pAddr23_WorkVol->setReadOnly(true);

    QLabel* pLabel2 = new QLabel(tr("工作电流"));
    m_pAddr23_WorkAn = new QLineEdit();
    m_pAddr23_WorkAn->setReadOnly(true);

    QLabel* pLabel3 = new QLabel(tr("板卡温度"));
    m_pAddr23_BoardTemp = new QLineEdit();
    m_pAddr23_BoardTemp->setReadOnly(true);

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(pLabel1);
    pHLayout->addWidget(m_pAddr23_WorkVol);
    pHLayout->addWidget(pLabel2);
    pHLayout->addWidget(m_pAddr23_WorkAn);
    pHLayout->addWidget(pLabel3);
    pHLayout->addWidget(m_pAddr23_BoardTemp);

    pGroup->setLayout(pHLayout);
    return pGroup;
}
QGroupBox* CStatusTab::CreateAddr22Group()
{
    QGroupBox* pGroup = new QGroupBox(tr("板卡[22, 0]"));

    QLabel* pLabel1 = new QLabel(tr("工作电压"));
    m_pAddr22_WorkVol = new QLineEdit();
    m_pAddr22_WorkVol->setReadOnly(true);

    QLabel* pLabel2 = new QLabel(tr("工作电流"));
    m_pAddr22_WorkAn = new QLineEdit();
    m_pAddr22_WorkAn->setReadOnly(true);

    QLabel* pLabel3 = new QLabel(tr("板卡温度"));
    m_pAddr22_BoardTemp = new QLineEdit();
    m_pAddr22_BoardTemp->setReadOnly(true);

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(pLabel1);
    pHLayout->addWidget(m_pAddr22_WorkVol);
    pHLayout->addWidget(pLabel2);
    pHLayout->addWidget(m_pAddr22_WorkAn);
    pHLayout->addWidget(pLabel3);
    pHLayout->addWidget(m_pAddr22_BoardTemp);

    pGroup->setLayout(pHLayout);
    return pGroup;
}
QGroupBox* CStatusTab::CreateAddr21Group()
{
    QGroupBox* pGroup = new QGroupBox(tr("板卡[21, 0]"));

    QLabel* pLabel1 = new QLabel(tr("工作电压"));
    m_pAddr21_WorkVol = new QLineEdit();
    m_pAddr21_WorkVol->setReadOnly(true);

    QLabel* pLabel2 = new QLabel(tr("工作电流"));
    m_pAddr21_WorkAn = new QLineEdit();
    m_pAddr21_WorkAn->setReadOnly(true);

    QLabel* pLabel3 = new QLabel(tr("板卡温度"));
    m_pAddr21_BoardTemp = new QLineEdit();
    m_pAddr21_BoardTemp->setReadOnly(true);

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(pLabel1);
    pHLayout->addWidget(m_pAddr21_WorkVol);
    pHLayout->addWidget(pLabel2);
    pHLayout->addWidget(m_pAddr21_WorkAn);
    pHLayout->addWidget(pLabel3);
    pHLayout->addWidget(m_pAddr21_BoardTemp);

    pGroup->setLayout(pHLayout);
    return pGroup;
}


// 处理数据
void CStatusTab::HandleFrameData(unsigned char chFrameCmd, const char* pFrameDataBuf, int iFrameDataLength,
                                 unsigned char chSourceAddr, unsigned char chSourcePort)
{
    QString strData = pFrameDataBuf;
    emit HandleFrame_Signal(strData, chSourceAddr, chSourcePort);
}
void CStatusTab::HandleFrame_Slot(const QString& strData, unsigned char chSourceAddr, unsigned char chSourcePort)
{
    strcpy_s(m_pRefStaInput_Buf, SIZE_OF_INPUT_BUF, strData.toLatin1().data());

    char chData[128] = {0};

    if(NULL != strstr(m_pRefStaInput_Buf, "Main_Control sta"))              // 主控板
    {
        // PLL1_I=	PLL1电流(毫安)	PRM_V=	主电源电压
        // PLL2_I=	PLL2电流(毫安)	SEC_V=	备电源电压
        // INNER_I=	内参考时钟电流(毫安)	INNER_V=	内参考时钟电压
        // MB_I=	主板电流(毫安)	MB_V=	主板电压
        if(true == ParseInputString(m_pRefStaInput_Buf, "PLL1_I=", chData))
        {
            strcat_s(chData, sizeof(chData), " mA");
            m_pMainCtrl_PLL1An->setText(chData);
        }
        if(true == ParseInputString(m_pRefStaInput_Buf, "PLL2_I=", chData))
        {
            strcat_s(chData, sizeof(chData), " mA");
            m_pMainCtrl_PLL2An->setText(chData);
        }
        if(true == ParseInputString(m_pRefStaInput_Buf, "INNER_I=", chData))
        {
            strcat_s(chData, sizeof(chData), " mA");
            m_pMainCtrl_RefClockAn->setText(chData);
        }
        if(true == ParseInputString(m_pRefStaInput_Buf, "INNER_V=", chData))
        {
            strcat_s(chData, sizeof(chData), " V");
            m_pMainCtrl_RefClockVol->setText(chData);
        }
        if(true == ParseInputString(m_pRefStaInput_Buf, "MB_I=", chData))
        {
            strcat_s(chData, sizeof(chData), " mA");
            m_pMainCtrl_MainBoardAn->setText(chData);
        }
        if(true == ParseInputString(m_pRefStaInput_Buf, "MB_V=", chData))
        {
            strcat_s(chData, sizeof(chData), " V");
            m_pMainCtrl_MainBoardVol->setText(chData);
        }
        if(true == ParseInputString(m_pRefStaInput_Buf, "PRM_V=", chData))
        {
            strcat_s(chData, sizeof(chData), " V");
            m_pMainCtrl_MainPowerVol->setText(chData);
        }
        if(true == ParseInputString(m_pRefStaInput_Buf, "SEC_V=", chData))
        {
            strcat_s(chData, sizeof(chData), " V");
            m_pMainCtrl_BackupPowerVol->setText(chData);
        }
    }
    else if(NULL != strstr(m_pRefStaInput_Buf, "Main_PLL10.00M sta"))       // PLL 10MHz
    {
        // Local_T:37	板卡温度	OSC_T=39.694	恒温槽温度
        // U_V=11.06	工作电压 V	Remote_T:0	电源(芯片)温度
        // clklock=0	0频率没锁定  1频率锁定
        if(true == ParseInputString(m_pRefStaInput_Buf, "Local_T:", chData))
        {
            strcat_s(chData, sizeof(chData), " 'C");
            m_pPLL10M_BoardTemp->setText(chData);
        }
        if(true == ParseInputString(m_pRefStaInput_Buf, "OSC_T=", chData))
        {
            strcat_s(chData, sizeof(chData), " 'C");
            m_pPLL10M_ConstCTemp->setText(chData);
        }
        if(true == ParseInputString(m_pRefStaInput_Buf, "U_V=", chData))
        {
            strcat_s(chData, sizeof(chData), " V");
            m_pPLL10M_WorkVol->setText(chData);
        }
        if(true == ParseInputString(m_pRefStaInput_Buf, "Remote_T:", chData))
        {
            strcat_s(chData, sizeof(chData), " 'C");
            m_pPLL10M_PowerTemp->setText(chData);
        }
        if(true == ParseInputString(m_pRefStaInput_Buf, "clklock=", chData))
        {
            if(0 == atoi(chData))
            {
                m_pPLL10M_FreqLock->setText("没锁");
            }
            else
            {
                m_pPLL10M_FreqLock->setText("锁定");
            }
        }
    }
    else if(NULL != strstr(m_pRefStaInput_Buf, "Main_PLL10.23M sta"))       // PLL 10.23MHz
    {
        // Local_T:37	板卡温度	OSC_T=39.694	恒温槽温度
        // U_V=11.06	工作电压 V	Remote_T:0	电源(芯片)温度
        // clklock=0	0频率没锁定  1频率锁定
        if(true == ParseInputString(m_pRefStaInput_Buf, "Local_T:", chData))
        {
            strcat_s(chData, sizeof(chData), " 'C");
            m_pPLL1023M_BoardTemp->setText(chData);
        }
        if(true == ParseInputString(m_pRefStaInput_Buf, "OSC_T=", chData))
        {
            strcat_s(chData, sizeof(chData), " 'C");
            m_pPLL1023M_ConstCTemp->setText(chData);
        }
        if(true == ParseInputString(m_pRefStaInput_Buf, "U_V=", chData))
        {
            strcat_s(chData, sizeof(chData), " V");
            m_pPLL1023M_WorkVol->setText(chData);
        }
        if(true == ParseInputString(m_pRefStaInput_Buf, "Remote_T:", chData))
        {
            strcat_s(chData, sizeof(chData), " 'C");
            m_pPLL1023M_PowerTemp->setText(chData);
        }
        if(true == ParseInputString(m_pRefStaInput_Buf, "clklock=", chData))
        {
            if(0 == atoi(chData))
            {
                m_pPLL1023M_FreqLock->setText("没锁");
            }
            else
            {
                m_pPLL1023M_FreqLock->setText("锁定");
            }
        }
    }
    else if(NULL != strstr(m_pRefStaInput_Buf, "Remote_Monitoring sta"))    // 监控板
    {
        // Board_addr：2		//槽位地址
        // U_V=12.5			//板卡电压
        // U_I=2.0			//板卡电流(毫安)
        // Local_T:37		//板卡温度
        // Remote_T:47		//电源(芯片)温度
        ParseInputForBoard(m_pRefStaInput_Buf);
    }
    else if(NULL != strstr(m_pRefStaInput_Buf, "LVDS_Distribute sta"))      //LVDS分配板
    {
        ParseInputForBoard(m_pRefStaInput_Buf);
    }
    else if(NULL != strstr(m_pRefStaInput_Buf, "LVTTL_Distribute sta"))     // LVTTL分配板
    {
        ParseInputForBoard(m_pRefStaInput_Buf);
    }
    else if(NULL != strstr(m_pRefStaInput_Buf, "Reference_Input sta"))      // 参考输入板
    {
        ParseInputForBoard(m_pRefStaInput_Buf);
    }
    else if(NULL != strstr(m_pRefStaInput_Buf, "RF_Distribute sta"))        // 正弦分配板
    {
        ParseInputForBoard(m_pRefStaInput_Buf);
    }
}

bool CStatusTab::ParseInputString(const char* ptrInput, const char* ptrKey, char* ptrOutput)
{
    const char* ptr = ptrInput;
    const char* ptrValue = strstr(ptr, ptrKey);
    if(NULL != ptrValue)
    {
        ptrValue += strlen(ptrKey);
        ptr = strstr(ptrValue, ",");
        if(NULL != ptr)
        {
            memcpy(ptrOutput, ptrValue, ptr - ptrValue);
            ptrOutput[ptr-ptrValue] = '\0';
            return true;
        }
        ptr = strstr(ptrValue, " ");
        if(NULL != ptr)
        {
            memcpy(ptrOutput, ptrValue, ptr - ptrValue);
            ptrOutput[ptr-ptrValue] = '\0';
            return true;
        }
    }
    return false;
}

void CStatusTab::ParseInputForBoard(const char* ptrInput)
{
    // Board_addr：2		//槽位地址
    // U_V=12.5			//板卡电压
    // U_I=2.0			//板卡电流(毫安)
    // Local_T:37		//板卡温度
    // Remote_T:47		//电源(芯片)温度
    char chData[128] = {0};
    int iAddr = 0;
    float fUV = 0.0f;
    float fUI = 0.0f;
    float fLT = 0.0f;
    if(true == ParseInputString(ptrInput, "Board_addr:", chData))
    {
        QString strData = chData;
        iAddr = strData.toInt(0, 16);
    }
    if(true == ParseInputString(ptrInput, "U_V=", chData))
    {
        fUV = atof(chData);
    }
    if(true == ParseInputString(ptrInput, "U_I=", chData))
    {
        fUI = atof(chData);
    }
    if(true == ParseInputString(ptrInput, "Local_T:", chData))
    {
        fLT = atof(chData);
    }
    BoardShowOnInput(iAddr, fUV, fUI, fLT);
}
void CStatusTab::BoardShowOnInput(int iAddr, float fUV, float fUI, float fLT)
{
    switch(iAddr)
    {
    case 0x06:
        m_pAddr06_WorkVol->setText(QString::number(fUV)+tr(" V"));
        m_pAddr06_WorkAn->setText(QString::number(fUI)+tr(" mA"));
        m_pAddr06_BoardTemp->setText(QString::number(fLT)+tr(" 'C"));
        break;
    case 0x05:
        m_pAddr05_WorkVol->setText(QString::number(fUV)+tr(" V"));
        m_pAddr05_WorkAn->setText(QString::number(fUI)+tr(" mA"));
        m_pAddr05_BoardTemp->setText(QString::number(fLT)+tr(" 'C"));
        break;
    case 0x04:
        m_pAddr04_WorkVol->setText(QString::number(fUV)+tr(" V"));
        m_pAddr04_WorkAn->setText(QString::number(fUI)+tr(" mA"));
        m_pAddr04_BoardTemp->setText(QString::number(fLT)+tr(" 'C"));
        break;
    case 0x03:
        m_pAddr03_WorkVol->setText(QString::number(fUV)+tr(" V"));
        m_pAddr03_WorkAn->setText(QString::number(fUI)+tr(" mA"));
        m_pAddr03_BoardTemp->setText(QString::number(fLT)+tr(" 'C"));
        break;
    case 0x02:
        m_pAddr02_WorkVol->setText(QString::number(fUV)+tr(" V"));
        m_pAddr02_WorkAn->setText(QString::number(fUI)+tr(" mA"));
        m_pAddr02_BoardTemp->setText(QString::number(fLT)+tr(" 'C"));
        break;
    case 0x01:
        m_pAddr01_WorkVol->setText(QString::number(fUV)+tr(" V"));
        m_pAddr01_WorkAn->setText(QString::number(fUI)+tr(" mA"));
        m_pAddr01_BoardTemp->setText(QString::number(fLT)+tr(" 'C"));
        break;

    case 0x26:
        m_pAddr26_WorkVol->setText(QString::number(fUV)+tr(" V"));
        m_pAddr26_WorkAn->setText(QString::number(fUI)+tr(" mA"));
        m_pAddr26_BoardTemp->setText(QString::number(fLT)+tr(" 'C"));
        break;
    case 0x25:
        m_pAddr25_WorkVol->setText(QString::number(fUV)+tr(" V"));
        m_pAddr25_WorkAn->setText(QString::number(fUI)+tr(" mA"));
        m_pAddr25_BoardTemp->setText(QString::number(fLT)+tr(" 'C"));
        break;
    case 0x24:
        m_pAddr24_WorkVol->setText(QString::number(fUV)+tr(" V"));
        m_pAddr24_WorkAn->setText(QString::number(fUI)+tr(" mA"));
        m_pAddr24_BoardTemp->setText(QString::number(fLT)+tr(" 'C"));
        break;
    case 0x23:
        m_pAddr23_WorkVol->setText(QString::number(fUV)+tr(" V"));
        m_pAddr23_WorkAn->setText(QString::number(fUI)+tr(" mA"));
        m_pAddr23_BoardTemp->setText(QString::number(fLT)+tr(" 'C"));
        break;
    case 0x22:
        m_pAddr22_WorkVol->setText(QString::number(fUV)+tr(" V"));
        m_pAddr22_WorkAn->setText(QString::number(fUI)+tr(" mA"));
        m_pAddr22_BoardTemp->setText(QString::number(fLT)+tr(" 'C"));
        break;
    case 0x21:
        m_pAddr21_WorkVol->setText(QString::number(fUV)+tr(" V"));
        m_pAddr21_WorkAn->setText(QString::number(fUI)+tr(" mA"));
        m_pAddr21_BoardTemp->setText(QString::number(fLT)+tr(" 'C"));
        break;
    default:
        break;
    }
}



