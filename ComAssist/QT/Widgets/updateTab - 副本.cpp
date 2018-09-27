#include "stdafx.h"
#include "updateTab.h"

#include <Windows.h>
#include <QtGui>

#include "../../../Common/Function/Function.h"
#include "../../../Libase/Libase.h"

#include "imageTab.h"               // �������
#include "statusTab.h"
#include "adjustTab.h"
#include "gnssTab.h"

#include "../../../Common/Define/DefineFile.h"

// --------------------------------------------------------------------------------------
extern st_PageCheck g_oPageCheck;   // ����Ȩ��

extern QString g_strGNSSAddr;       // GNSS �忨��Ŀ�ĵ�ַ
extern QString g_strGNSSPort;

// �汾����
extern int g_iLargeVer;
extern int g_iSmallVer;
extern int g_iYear;
extern int g_iMonth;
extern int g_iDay;


bool g_bShowBoardTip = true;        // ���Ρ����еİ忨���ź������ʾ�� ���


// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
// ��ʾ��������
void ShowRecvDataFunc(unsigned char chAddr, unsigned char chPort, const char* pszShowData, bool bChanged, int iR, int iG, int iB);
void SetRecvShowFunc(int iShowType);

// ��ʾ��������
void ShowSendDataFunc(unsigned char chAddr, unsigned char chPort, const char* pszShowData, int iLength);

// ����������ʾ����ɫ(������ɫ)
void ShowErrorColorFunc();

// �������س�ʱ����
void DownloadTimeoutFunc();

// ֡���ݴ�����
void FrameHanleFunc(const st_FrameData* pFrameData);

// --------------------------------------------------------------------------------------
CUpdateTab* g_pUpdateTab = NULL;

// ֡�������ݱ���
st_FrameData g_FrameData_1;
char* g_pFrameBuf_1 = NULL;
st_FrameData g_FrameData_2;
char* g_pFrameBuf_2 = NULL;

// �忨��ַ
unsigned char g_BoardAddr[COUNT_OF_AUTODOWN_CMD][2] = {
    0x01,0x00,
    0x02,0x00,
    0x03,0x00,
    0x04,0x00,
    0x05,0x00,
    0x06,0x00,
    0x21,0x00,
    0x22,0x00,
    0x23,0x00,
    0x24,0x00,
    0x25,0x00,
    0x26,0x00,
    0x80,0x23,
    0x80,0xaa,
    0x80,0x00
};

// --------------------------------------------------------------------------------------
// ͨ��/���� tab
// --------------------------------------------------------------------------------------
CUpdateTab::CUpdateTab(QWidget* parent) : QWidget(parent)
{
    m_pMyComBasic = NULL;
    m_bComIsOpened = false;
    m_bNetIsOpened = false;
    m_bUseIsCom = false;
    m_bIsEncrypt = false;
    m_bUseFileName = false;
    m_strFileNameUse = "";

    m_hExportFile = NULL;
    m_hCmdLogFile = NULL;

    m_iTimeOut = 0;
    m_iTimeOut_Save = 0;
    m_iReDownCount_Save = 0;
    m_bStopDownload = false;
    m_strFile = "";

    m_bSingleDebug = false;
    m_iSingleNum = 0;
    m_iRecvShowType = 0;

    m_iMultiFrame = 0;
    m_bCmdIsNavtfIsOne = false;
    m_bCmdIsNavtfFinished = false;
    m_bSomeFuncEnabled = false;

    m_pCmdItem_Head = NULL;
    m_pCmdItem_Cur = NULL;
    m_enOpType = en_Op_Is_NULL;

    m_pMyImageTab = NULL;
    m_pStatusTab = NULL;
    m_pMyAdjustTab = NULL;
    m_pGNSSTab  = NULL;

    m_pSignalMapper = new QSignalMapper(this);
    connect(m_pSignalMapper, SIGNAL(mapped(int)), this, SLOT(SaveInput(int)));

    // ��ʾ
    connect(this, SIGNAL(ShowRecvText_Signal(unsigned char, unsigned char, const QString, bool, int, int, int)), this, SLOT(ShowRecvText_Slot(unsigned char, unsigned char, const QString, bool, int, int, int)));
    connect(this, SIGNAL(ShowSendText_Signal(unsigned char, unsigned char, const QString)), this, SLOT(ShowSendText_Slot(unsigned char, unsigned char, const QString)));
    connect(this, SIGNAL(ShowTipText_Signal(const QString, QColor)), this, SLOT(ShowTipText_Slot(const QString, QColor)));

    // �ؼ�
    connect(this, SIGNAL(FrameNoInc_Signal(int)), this, SLOT(FrameNoInc_Slot(int)));
    connect(this, SIGNAL(BoardSelectFill_Signal()), this, SLOT(BoardSelectFill_Slot()));

    // �������
    connect(this, SIGNAL(CmdFinished_Signal()), this, SLOT(CmdFinished_Slot()));

    // �豸�ź�״̬
    connect(this, SIGNAL(DeviceSignalStatus_Signal(int)), this, SLOT(DeviceSignalStatus_Slot(int)));

    connect(this, SIGNAL(CmdLog_Signal(unsigned char, unsigned char, QString)), this, SLOT(CmdLog_Slot(unsigned char, unsigned char, QString)));
    connect(this, SIGNAL(CheckSendedOneCmd_Signal(const QString, int)), this, SLOT(CheckSendedOneCmd_Slot(const QString, int)));

    // �Զ�����Ŀ¼
    char chAutoPath[1024] = {0};
    CFuncTool::GetMoudlePath(chAutoPath, sizeof(chAutoPath));
    m_strAutoPath = chAutoPath;
    m_strAutoPath += NAME_OF_AUTO_UPDATE_DIR;
    if(false == CFuncTool::FolderExist(m_strAutoPath.toLatin1()))
    {
        CFuncTool::CreateFolder(m_strAutoPath.toLatin1());
    }
    m_strAutoPath += "/";

    // ������ʽ
    // 1~2λ16��������
    QRegExp rx1("^0|([1-9a-fA-F][0-9a-fA-F]{0,1}$)");
    m_p0x16_RegExpVal = new QRegExpValidator(rx1, this);

    // 1~6λ����
    QRegExp rx2("^[1-9][0-9]{0,5}$");
    m_pNum_RegExpVal = new QRegExpValidator(rx2, this);

    // IP ��ַ
    QRegExp rx3("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    m_pIP_RegExpVal = new QRegExpValidator(rx3, this);


    CreateWidgets();

    // ������ǹ���Ա�汾����Ҫ���볬�������������ĳЩ����
    SetSomeFuncEnabled(g_oPageCheck.IsAdmin());
    
    // ��ʼ����������
    InitComInfo();

    // INI
    m_pComName_ComboBox->setCurrentIndex(CFuncTool::GetComIndex());
    m_pComBaud_ComboBox->setCurrentIndex(CFuncTool::GetComBaud());

    // ע�⣺���ڲ�������ʱ�̶�Ϊ57600
    m_pComBaud_ComboBox->setCurrentIndex(5);

    // INI
    char chFilePath[1024] = {0};
    CFuncTool::GetDownloadFilePath(chFilePath, sizeof(chFilePath));
    m_strFile = chFilePath;
    m_pFileShow_LineEdit->setText(GetShortFileOnPath(m_strFile.toLatin1()));

    g_pUpdateTab = this;

    g_pFrameBuf_1 = new char[1024*2];
    g_pFrameBuf_2 = new char[1024*2];
    g_FrameData_1.m_pFrameDataBuf = g_pFrameBuf_1;
    g_FrameData_2.m_pFrameDataBuf = g_pFrameBuf_2;

    // Timer
    m_pTimer = new QTimer(this);
    m_pTimer->stop();
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(ReSendTimeReach()));

    SetFuncButtonEnable(false);

    // ��ʼ���忨�����ƶ�Ӧ��ϵ
    {
        m_BoardName_map["Reference_Input"] = "�ο������"; 
        m_BoardName_map["Remote_Monitoring"] = "Զ�̼�ذ�";
        m_BoardName_map["Main_PLL10.23M"] = "���໷10.23M";
        m_BoardName_map["Main_PLL10.00M"] = "���໷10.00M";
        m_BoardName_map["Main_Control"] = "���ذ�";
        m_BoardName_map["RF_Distribute"] = "���Ҳ�ʱ�ӷ����";
        m_BoardName_map["LVTTL_Distribute"] = "LVTTL��������";
        m_BoardName_map["LVDS_Distribute"] = "LVDS��������";
        m_BoardName_map["Reference_GNSS"] = "GNSS�����";
        m_BoardName_map["RF_Distri125M"] = "���Ҳ�ʱ�ӷ����";
        m_BoardName_map["PLL_Universal"] = "ͨ�����໷��";
    }

    // �����Զ����ر����ڴ�
    char* ptr = NULL;
    for(int i = 0; i < NUM_OF_AUTODOWN_REPORT; ++i)
    {
        ptr = new char[SIZE_OF_AUTODOWN_REPORT];
        memset(ptr, 0, SIZE_OF_AUTODOWN_REPORT);
        m_pAutoDownloadReport[i] = ptr;
    }
    for(int i = 0; i < COUNT_OF_AUTODOWN_CMD; ++i)
    {
        ptr = new char[SIZE_OF_AUTODOWN_REPORT];
        memset(ptr, 0, SIZE_OF_AUTODOWN_REPORT);
        m_pAutoUpdateSimpleReport[i] = ptr;
    }

}
CUpdateTab::~CUpdateTab()
{
    delete[] g_pFrameBuf_1;
    delete[] g_pFrameBuf_2;

    char* ptr = NULL;
    for(int i = 0; i < NUM_OF_AUTODOWN_REPORT; ++i)
    {
        ptr = m_pAutoDownloadReport[i];
        delete[] ptr;
    }
    for(int i = 0; i < COUNT_OF_AUTODOWN_CMD; ++i)
    {
        ptr = m_pAutoUpdateSimpleReport[i];
        delete[] ptr;
    }

    CloseExportFile();
    if(NULL != m_hCmdLogFile) CloseHandle((HANDLE)m_hCmdLogFile);
    m_hCmdLogFile = NULL;
}

// ������ǹ���Ա�汾����Ҫ���볬�������������ĳЩ����
void CUpdateTab::SetSomeFuncEnabled(bool bEnable)
{
    m_pAutoReSend_CheckBox->setEnabled(bEnable);
    m_pTimeReSend_LineEdit->setEnabled(bEnable);
    m_pAutoMakeFrame_CheckBox->setEnabled(bEnable);
    m_pSend16Data_CheckBox->setEnabled(bEnable);
    m_pRecv16Data_CheckBox->setEnabled(bEnable);
    m_pEnter_CheckBox->setEnabled(bEnable);
    
    m_pAutoReSend_CheckBox->setChecked(false);
    m_pAutoMakeFrame_CheckBox->setChecked(true);
    m_pSend16Data_CheckBox->setChecked(false);
    m_pRecv16Data_CheckBox->setChecked(false);
    m_pEnter_CheckBox->setChecked(false);

    m_pSelectAlone_Radio->setEnabled(bEnable);
    BoardTypeChanged(UPDATE_SELECT_IS_SYSTEM);

    m_bSomeFuncEnabled = bEnable;

    if(NULL != m_pMyAdjustTab)
    {
        m_pMyAdjustTab->SetLockFunc(bEnable);
    }
}

void CUpdateTab::SetComBasic(CMyComBasic* pComBasic)
{
    m_pMyComBasic = pComBasic;

    m_pMyComBasic->SetShowRecvDataFunc(::ShowRecvDataFunc);
    m_pMyComBasic->SetRecvShowFunc(::SetRecvShowFunc);
    m_pMyComBasic->SetShowSendDataFunc(::ShowSendDataFunc);
    m_pMyComBasic->SetDownloadTimeoutFunc(::DownloadTimeoutFunc);
    m_pMyComBasic->SetFrameHandleFunc(::FrameHanleFunc);

    // INI
    int iSelect = CFuncTool::GetComNetSelect();
    if(COMNET_SELECT_IS_COM == iSelect)
    {
        ComNetChanged(COMNET_SELECT_IS_COM);
    }
    else if(COMNET_SELECT_IS_NET == iSelect)
    {
        ComNetChanged(COMNET_SELECT_IS_NET);
    }

    // ��Ϊ��������˳���Ⱥ�֮������⣬��������������һ��ѡ��
    // ��������͡���ť���������������Ƿ���16����; ��������ఴť��ǿ������Ϊ16����(�ָ�����)
    SetFrameDataStyle(true, true, true, true);

    // ��ʾʹ�õ�DLL�汾
    int iLargeVer = 0;
    int iSmallVer = 0;
    int iYear = 0;
    int iMonth = 0;
    int iDay = 0;
    QString strInfo = "";

    // ͨ��
    m_pMyComBasic->GetDllLibVerAndDate(iLargeVer, iSmallVer, iYear, iMonth, iDay);
    strInfo = tr("ͨ�ſ� �汾: V%1.%2, %3��%4��%5��").arg(iLargeVer).arg(iSmallVer).arg(iYear).arg(iMonth).arg(iDay);
    ShowTipInfoFunc(strInfo.toLatin1().data());

    // UI
    strInfo = tr("����� �汾: V%1.%2, %3��%4��%5��").arg(g_iLargeVer).arg(g_iSmallVer).arg(g_iYear).arg(g_iMonth).arg(g_iDay);
    ShowTipInfoFunc(strInfo.toLatin1().data());

}

void CUpdateTab::Set_MyImageTab(CMyImageTab* pMyImageTab)
{
    m_pMyImageTab = pMyImageTab;
}

void CUpdateTab::Set_StatusTab(CStatusTab* pStatusTab)
{
    m_pStatusTab = pStatusTab;
}

void CUpdateTab::Set_MyAdjustTab(CMyAdjustTab* pMyAdjustTab)
{
    m_pMyAdjustTab = pMyAdjustTab;
}

void CUpdateTab::Set_GNSSTab(CGNSSTab* pGNSSTab)
{
    m_pGNSSTab = pGNSSTab;
}

void CUpdateTab::CreateWidgets()
{
    // ���ز��ֿؼ�
    // ��ʱ����ʾ��Դ��ַ���͡������֡�
    CreateSourceAddr()->hide();
    CreateFrameCmd()->hide();

    // ��
    QVBoxLayout* pLeftVLayout = new QVBoxLayout();
    pLeftVLayout->addLayout(CreateCommSet());
    pLeftVLayout->addStretch();


    QVBoxLayout* pRefStatusLayout = CreateRefStatus();
    if(true == g_oPageCheck.UpdateTab_RefStatus_Use())
    {
        pLeftVLayout->addLayout(pRefStatusLayout);          // �ο����� ״̬��ʾ
    }
    else
    {
        m_pRefInput_Group->hide();
    }

    pLeftVLayout->addWidget(CreateClear());

    // ��
    QVBoxLayout* pMidVLayout = new QVBoxLayout();
    pMidVLayout->addLayout(CreateShow());
    pMidVLayout->addWidget(CreateOption());

    // ��
    QVBoxLayout* pRightLayout = new QVBoxLayout();
    pRightLayout->addLayout(CreateUpdate());
    pRightLayout->addWidget(CreateSend());

    QHBoxLayout* pTopHLayout = new QHBoxLayout();
    pTopHLayout->addLayout(pLeftVLayout);
    pTopHLayout->addLayout(pMidVLayout, 1);
    pTopHLayout->addLayout(pRightLayout);

    setLayout(pTopHLayout);

    // INI
    BoardTypeChanged(CFuncTool::GetUpdateSelect());
}

// ͨ������
QVBoxLayout* CUpdateTab::CreateCommSet()
{
    QVBoxLayout* pVLayout = new QVBoxLayout();
    pVLayout->addWidget(CreateComInfo());
    pVLayout->addWidget(CreateNetInfo());

    return pVLayout;
}
QGroupBox* CUpdateTab::CreateComInfo()
{
    QGroupBox* pGroup = new QGroupBox(tr("��������"));
    m_pSelectCom_GroupBox = pGroup;
    m_pSelectCom_GroupBox->setCheckable(true);
    connect(m_pSelectCom_GroupBox, SIGNAL(clicked()), this, SLOT(SelectCom_Slot()));

    // INI
    int iSelect = CFuncTool::GetComNetSelect();
    if(COMNET_SELECT_IS_COM == iSelect)
    {
        m_bUseIsCom = true;
        m_pSelectCom_GroupBox->setChecked(true);
    }
    else
    {
        m_bUseIsCom = false;
        m_pSelectCom_GroupBox->setChecked(false);
    }

    QLabel* pComNameLabel = new QLabel(tr("���ں�"));
    pComNameLabel->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    pComNameLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pComName_ComboBox = new QComboBox();
    m_pComName_ComboBox->setMaximumWidth(SIZE_WIDTH_OF_EDIT);

    QLabel* pComBaudLabel = new QLabel(tr("������"));
    pComBaudLabel->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    pComBaudLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pComBaud_ComboBox = new QComboBox();
    m_pComBaud_ComboBox->setMaximumWidth(SIZE_WIDTH_OF_EDIT);

    QLabel* pComDataBitsLabel = new QLabel(tr("����λ"));
    pComDataBitsLabel->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    pComDataBitsLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pComDataBit_ComboBox = new QComboBox();
    m_pComDataBit_ComboBox->setMaximumWidth(SIZE_WIDTH_OF_EDIT);

    QLabel* pComStopBitsLabel = new QLabel(tr("ֹͣλ"));
    pComStopBitsLabel->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    pComStopBitsLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pComStopBit_ComboBox = new QComboBox();
    m_pComStopBit_ComboBox->setMaximumWidth(SIZE_WIDTH_OF_EDIT);

    QLabel* pComParityLabel = new QLabel(tr("У��λ"));
    pComParityLabel->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    pComParityLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pComCheck_ComboBox = new QComboBox();
    m_pComCheck_ComboBox->setMaximumWidth(SIZE_WIDTH_OF_EDIT);

    m_pComStatus_Label = new QLabel(tr(""));
    m_pComStatus_Label->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    m_pComStatus_Label->setPixmap(QPixmap(":/images/red.png"));
    m_pComStatus_Label->setAlignment(Qt::AlignCenter);

    m_pComOpen_Button = new QPushButton(tr("�򿪴���"));
    m_pComOpen_Button->setMaximumWidth(SIZE_WIDTH_OF_EDIT);
    m_pComOpen_Button->setEnabled(false);

    connect(m_pComOpen_Button, SIGNAL(clicked()), this, SLOT(OpenCloseCom()));
    connect(m_pComName_ComboBox, SIGNAL(currentIndexChanged (const QString&)), this, SLOT(ComNameChanged(const QString&)));

    QGridLayout* pGLayout = new QGridLayout();
    pGLayout->addWidget(pComNameLabel, 0, 0);
    pGLayout->addWidget(m_pComName_ComboBox, 0, 1);
    pGLayout->addWidget(pComBaudLabel, 1, 0);
    pGLayout->addWidget(m_pComBaud_ComboBox, 1, 1);
    pGLayout->addWidget(m_pComStatus_Label, 2, 0);
    pGLayout->addWidget(m_pComOpen_Button, 2, 1);

    pGroup->setLayout(pGLayout);

    return pGroup;
}
QGroupBox* CUpdateTab::CreateNetInfo()
{
    QGroupBox* pGroup = new QGroupBox(tr("��������"));
    m_pSelectNet_GroupBox = pGroup;
    m_pSelectNet_GroupBox->setCheckable(true);
    connect(m_pSelectNet_GroupBox, SIGNAL(clicked()), this, SLOT(SelectNet_Slot()));

    // INI
    int iSelect = CFuncTool::GetComNetSelect();
    if(COMNET_SELECT_IS_COM == iSelect)
    {
        m_bUseIsCom = true;
        m_pSelectNet_GroupBox->setChecked(false);
    }
    else
    {
        m_bUseIsCom = false;
        m_pSelectNet_GroupBox->setChecked(true);
    }

    QLabel* pNetIPLable = new QLabel(tr("IP��ַ"));
    pNetIPLable->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    pNetIPLable->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pNetIPAddr_LineEdit = new QLineEdit();
    m_pNetIPAddr_LineEdit->setMaximumWidth(SIZE_WIDTH_OF_EDIT);
    m_pNetIPAddr_LineEdit->setValidator(m_pIP_RegExpVal);

    QLabel* pNetIPPort = new QLabel(tr("IP�˿�"));
    pNetIPPort->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    pNetIPPort->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pNetIPPort_LineEdit = new QLineEdit();
    m_pNetIPPort_LineEdit->setReadOnly(true);
    m_pNetIPPort_LineEdit->setMaximumWidth(SIZE_WIDTH_OF_EDIT);
    m_pNetIPPort_LineEdit->setValidator(m_pNum_RegExpVal);

    m_pNetStatus_Label = new QLabel(tr(""));
    m_pNetStatus_Label->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    m_pNetStatus_Label->setPixmap(QPixmap(":/images/red.png"));
    m_pNetStatus_Label->setAlignment(Qt::AlignCenter);

    m_pNetOpen_Button = new QPushButton(tr("������"));
    m_pNetOpen_Button->setMaximumWidth(SIZE_WIDTH_OF_EDIT);
    m_pNetOpen_Button->setEnabled(false);
    connect(m_pNetOpen_Button, SIGNAL(clicked()), this, SLOT(OpenCloseNet()));

    connect(m_pNetIPAddr_LineEdit, SIGNAL(editingFinished()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pNetIPAddr_LineEdit, ID_OF_IP_ADDRESS);
    connect(m_pNetIPPort_LineEdit, SIGNAL(editingFinished()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pNetIPPort_LineEdit, ID_OF_IP_PORT);

    // ���ڲ���
    QLabel* pLabel = new QLabel(tr("  "));
    pLabel->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    pLabel->setAlignment(Qt::AlignCenter);
    m_pNetConnectTest_Button = new QPushButton(tr("��������"));
    m_pNetConnectTest_Button->setMaximumWidth(SIZE_WIDTH_OF_EDIT);
    m_pNetConnectTest_Button->setEnabled(false);
    connect(m_pNetConnectTest_Button, SIGNAL(clicked()), this, SLOT(NetConnectTestFunc()));

    char chAddr[1024] = {0};
    CFuncTool::GetUDPAddr(chAddr, sizeof(chAddr));
    m_pNetIPAddr_LineEdit->setText(chAddr);
    m_pNetIPPort_LineEdit->setText(QString::number(CFuncTool::GetUDPPort()));

    // ע�⣺���ڶ˿���ʱ�̶�Ϊ 20212
    m_pNetIPPort_LineEdit->setText("20212");
    m_pNetIPPort_LineEdit->setEnabled(false);

    QGridLayout* pGLayout = new QGridLayout();
    pGLayout->addWidget(pNetIPLable, 0, 0);
    pGLayout->addWidget(m_pNetIPAddr_LineEdit, 0, 1);
    pGLayout->addWidget(pNetIPPort, 1, 0);
    pGLayout->addWidget(m_pNetIPPort_LineEdit, 1, 1);
    pGLayout->addWidget(pLabel, 2, 0);
    pGLayout->addWidget(m_pNetConnectTest_Button, 2, 1);
    pGLayout->addWidget(m_pNetStatus_Label, 3, 0);
    pGLayout->addWidget(m_pNetOpen_Button, 3, 1);

    pGroup->setLayout(pGLayout);

    return pGroup;
}

// �����ļ�
QGroupBox* CUpdateTab::CreateExportGroup()
{
    QGroupBox* pGroup = new QGroupBox(tr("�ļ�����"));

    m_pExportPath_Button = new QPushButton(tr("·��..."));
    m_pExportFileShow_LineEdit = new QLineEdit();
    m_pExportFileShow_LineEdit->setReadOnly(true);
    m_pAllowExport_CheckBox = new QCheckBox(tr("����"));
    m_pExportUseTime_CheckBox = new QCheckBox(tr("ʱ��"));

    connect(m_pExportPath_Button, SIGNAL(clicked()), this, SLOT(SelectExportFilePath()));

    // INI
    char chPath[256] = {0};
    CFuncTool::GetExportFilePath(chPath, sizeof(chPath));
    int iAllow = CFuncTool::GetAllowExportFile();
    if(strlen(chPath) > 3)
    {
        m_pExportFileShow_LineEdit->setText(chPath);
    }
    if(iAllow > 0)
    {
        m_pAllowExport_CheckBox->setChecked(true);
    }
    m_pExportUseTime_CheckBox->setChecked(CFuncTool::GetExportUseTime()>0?true:false);

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(m_pExportPath_Button);
    pHLayout->addWidget(m_pExportFileShow_LineEdit);
    pHLayout->addWidget(m_pAllowExport_CheckBox);
    pHLayout->addWidget(m_pExportUseTime_CheckBox);

    pGroup->setLayout(pHLayout);
    return pGroup;
}

QGroupBox* CUpdateTab::CreateSourceAddr()
{
    QGroupBox* pGroup = new QGroupBox(tr("Դ��ַ"));

    QLabel* pAddrLabel = new QLabel(tr("Դ��ַ"));
    pAddrLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pSourceAddr_LineEdit = new QLineEdit();
    m_pSourceAddr_LineEdit->setReadOnly(true);
    m_pSourceAddr_LineEdit->setMaximumWidth(SIZE_WIDTH_OF_EDIT);
    m_pSourceAddr_LineEdit->setValidator(m_p0x16_RegExpVal);

    QLabel* pPortLabel = new QLabel(tr("Դ�˿�"));
    pPortLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pSourcePort_LineEdit = new QLineEdit();
    m_pSourcePort_LineEdit->setReadOnly(true);
    m_pSourcePort_LineEdit->setMaximumWidth(SIZE_WIDTH_OF_EDIT);
    m_pSourcePort_LineEdit->setValidator(m_p0x16_RegExpVal);

    QLabel* pRsvLabel = new QLabel(tr("����һ"));
    pRsvLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pSourceResv_LineEdit = new QLineEdit();
    m_pSourceResv_LineEdit->setMaximumWidth(SIZE_WIDTH_OF_EDIT);
    m_pSourceResv_LineEdit->setValidator(m_p0x16_RegExpVal);

    connect(m_pSourceAddr_LineEdit, SIGNAL(editingFinished()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pSourceAddr_LineEdit, ID_OF_SOURCE_ADDR);
    connect(m_pSourcePort_LineEdit, SIGNAL(editingFinished()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pSourcePort_LineEdit, ID_OF_SOURCE_PORT);
    connect(m_pSourceResv_LineEdit, SIGNAL(editingFinished()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pSourceResv_LineEdit, ID_OF_SOURCE_USE);

    int iValue = CFuncTool::GetComSvrAddr();

    // ��ʼ��0x60
    if(0 == iValue) iValue = 0x60;

    m_pSourceAddr_LineEdit->setText(QString::number(iValue, 16).toUpper());
    iValue = CFuncTool::GetComSvrPort();
    m_pSourcePort_LineEdit->setText(QString::number(iValue, 16).toUpper());
    m_pSourceResv_LineEdit->setText(QString::number(CFuncTool::GetComBackup1(), 16).toUpper());

    // ע�⣺Դ��ַ��ʱ�̶�Ϊ[0x60, 00]
    iValue = 0x60;
    m_pSourceAddr_LineEdit->setText(QString::number(iValue, 16).toUpper());
    m_pSourceAddr_LineEdit->setEnabled(false);
    m_pSourcePort_LineEdit->setText("0");
    m_pSourcePort_LineEdit->setEnabled(false);

    QGridLayout* pGLayout = new QGridLayout();
    pGLayout->addWidget(pAddrLabel, 0, 0);
    pGLayout->addWidget(m_pSourceAddr_LineEdit, 0, 1);
    pGLayout->addWidget(pPortLabel, 1, 0);
    pGLayout->addWidget(m_pSourcePort_LineEdit, 1, 1);

    pGroup->setLayout(pGLayout);

    return pGroup;
}

QGroupBox* CUpdateTab::CreateFrameCmd()
{
    QGroupBox* pGroup = new QGroupBox(tr("������"));

    QLabel* pAddrLabel = new QLabel(tr("֡ ��"));
    pAddrLabel->setAlignment(Qt::AlignRight);
    m_pFrameNo_LineEdit = new QLineEdit();
    m_pFrameNo_LineEdit->setMaximumWidth(SIZE_WIDTH_OF_EDIT);
    m_pFrameNo_LineEdit->setValidator(m_p0x16_RegExpVal);

    QLabel* pPortLabel = new QLabel(tr("������"));
    pPortLabel->setAlignment(Qt::AlignRight);
    m_pFrameCmd_LineEdit = new QLineEdit();
    m_pFrameCmd_LineEdit->setMaximumWidth(SIZE_WIDTH_OF_EDIT);
    m_pFrameCmd_LineEdit->setValidator(m_p0x16_RegExpVal);

    connect(m_pFrameNo_LineEdit, SIGNAL(editingFinished()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pFrameNo_LineEdit, ID_OF_FRAME_NUM);
    connect(m_pFrameCmd_LineEdit, SIGNAL(editingFinished()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pFrameCmd_LineEdit, ID_OF_FRAME_COMMAND);

    m_pFrameNo_LineEdit->setText(QString::number(CFuncTool::GetComNumber(), 16).toUpper());
    m_pFrameCmd_LineEdit->setText(QString::number(CFuncTool::GetComCommand(), 16).toUpper());

    QGridLayout* pGLayout = new QGridLayout();
    pGLayout->addWidget(pAddrLabel, 0, 0);
    pGLayout->addWidget(m_pFrameNo_LineEdit, 0, 1);
    pGLayout->addWidget(pPortLabel, 1, 0);
    pGLayout->addWidget(m_pFrameCmd_LineEdit, 1, 1);

    pGroup->setLayout(pGLayout);

    return pGroup;
}

// �ο�����
QVBoxLayout* CUpdateTab::CreateRefStatus()
{
    QGroupBox* pGroup = new QGroupBox(tr("�ο�����"));
    m_pRefInput_Group = pGroup;

    // ��Ƶ��
    QLabel* pRefStatus_Label = new QLabel(tr(""));
    pRefStatus_Label->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    pRefStatus_Label->setPixmap(QPixmap(":/images/green.png"));
    pRefStatus_Label->setAlignment(Qt::AlignCenter);

    QLabel* pRefSignalName_Label = new QLabel(tr("��Ƶ��10M"));
    pRefSignalName_Label->setMaximumWidth(SIZE_WIDTH_OF_EDIT);

    // LVDS
    m_p1PPSLVDS_Label = new QLabel(tr(""));
    m_p1PPSLVDS_Label->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    m_p1PPSLVDS_Label->setPixmap(QPixmap(":/images/red.png"));
    m_p1PPSLVDS_Label->setAlignment(Qt::AlignCenter);

    QLabel* pRefSignalName_Label2 = new QLabel(tr("1PPS_LVDS"));
    pRefSignalName_Label2->setMaximumWidth(SIZE_WIDTH_OF_EDIT);

    QGridLayout* pGLayout = new QGridLayout();
    pGLayout->addWidget(pRefStatus_Label, 0, 0);
    pGLayout->addWidget(pRefSignalName_Label, 0, 1);
    pGLayout->addWidget(m_p1PPSLVDS_Label, 1, 0);
    pGLayout->addWidget(pRefSignalName_Label2, 1, 1);
    pGroup->setLayout(pGLayout);

    // VLayout
    QVBoxLayout* pVLayout = new QVBoxLayout();
    pVLayout->addWidget(pGroup);
    return pVLayout;
}


QGroupBox* CUpdateTab::CreateClear()
{
    QGroupBox* pGroup = new QGroupBox(tr(""));

    m_pRecvAll_CheckBox = new QCheckBox(tr("��������"));
    m_pRecvAll_CheckBox->setEnabled(false);

    m_pDebug_CheckBox = new QCheckBox(tr("����"));
    m_pDebug_CheckBox->setEnabled(false);

    m_pClearShow_Button = new QPushButton(tr("�� ��"));
    m_pClearShow_Button->setMinimumHeight(50);
    m_pClearShow_Button->setMinimumWidth(50);

    connect(m_pClearShow_Button, SIGNAL(clicked()), this, SLOT(ClearShowInfo()));
    connect(m_pRecvAll_CheckBox, SIGNAL(clicked()), this, SLOT(RecvAllClicked()));
    connect(m_pDebug_CheckBox, SIGNAL(clicked()), this, SLOT(CheckDebugClicked()));

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(m_pClearShow_Button);

    pGroup->setLayout(pHLayout);

    return pGroup;
}


QGroupBox* CUpdateTab::CreateOption()
{
    QHBoxLayout* pMainHLayout = new QHBoxLayout();

    m_pAutoReSend_CheckBox = new QCheckBox(tr("�Զ��ط�"));
    connect(m_pAutoReSend_CheckBox, SIGNAL(clicked()), this, SLOT(AutoReSendClicked()));

    QLabel* pTimeLabel = new QLabel(tr("����"));
    m_pTimeReSend_LineEdit = new QLineEdit();
    m_pTimeReSend_LineEdit->setMaximumWidth(100);
    m_pTimeReSend_LineEdit->setMinimumWidth(100);
    m_pTimeReSend_LineEdit->setValidator(m_pNum_RegExpVal);
    QLabel* pMSLabel = new QLabel(tr("ms"));

    connect(m_pTimeReSend_LineEdit, SIGNAL(editingFinished()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pTimeReSend_LineEdit, ID_OF_RESEND_CIRCLE);
    m_pTimeReSend_LineEdit->setText(QString::number(CFuncTool::GetComCircle()));

    QHBoxLayout* pTopHLayout = new QHBoxLayout();
    pTopHLayout->addWidget(m_pAutoReSend_CheckBox);
    pTopHLayout->addWidget(pTimeLabel);
    pTopHLayout->addWidget(m_pTimeReSend_LineEdit);
    pTopHLayout->addWidget(pMSLabel);
    pTopHLayout->addStretch();


    m_pAutoMakeFrame_CheckBox = new QCheckBox(tr("�Զ���֡"));
    connect(m_pAutoMakeFrame_CheckBox, SIGNAL(clicked()), this, SLOT(AutoMakeFrameClicked()));

    m_pSend16Data_CheckBox = new QCheckBox(tr("��16����"));
    connect(m_pSend16Data_CheckBox, SIGNAL(clicked()), this, SLOT(CheckSend16DataClicked()));

    m_pRecv16Data_CheckBox = new QCheckBox(tr("��16����"));
    connect(m_pRecv16Data_CheckBox, SIGNAL(clicked()), this, SLOT(CheckRecv16DataClicked()));

    m_pEnter_CheckBox = new QCheckBox(tr("�� ��"));
    connect(m_pEnter_CheckBox, SIGNAL(clicked()), this, SLOT(EnterClicked()));

    // INI
    int iEnter = CFuncTool::GetEnterForSend();
    if(0 != iEnter) m_pEnter_CheckBox->setChecked(true);

    QHBoxLayout* pBottomHLayout = new QHBoxLayout();
    pBottomHLayout->addWidget(m_pAutoMakeFrame_CheckBox);
    pBottomHLayout->addWidget(m_pSend16Data_CheckBox);
    pBottomHLayout->addWidget(m_pRecv16Data_CheckBox);

    if(true == g_oPageCheck.UpdateTab_EnterCheck_Use())
    {
        pBottomHLayout->addWidget(m_pEnter_CheckBox);
    }
    else
    {
        m_pEnter_CheckBox->hide();
        m_pEnter_CheckBox->setChecked(false);
    }
    pBottomHLayout->addStretch();

    QVBoxLayout* pVLayout = new QVBoxLayout();
    pVLayout->addLayout(pTopHLayout);
    pVLayout->addLayout(pBottomHLayout);

    m_pSendText_LineEdit = new QLineEdit();
    m_pSendText_LineEdit->setMinimumHeight(50);
    m_pSendText_LineEdit->setObjectName("UpdateSend_LineEdit");

    // QSS
    m_pSendText_LineEdit->setStyleSheet("font-size: 25px;");

    // INI
    char chSendData[1024] = {0};
    CFuncTool::GetComSendData(chSendData, sizeof(chSendData));
    m_pSendText_LineEdit->setText(chSendData);

    int iCheck = CFuncTool::GetAutoMakeFrame();
    if(iCheck > 0)
    {
        m_pAutoMakeFrame_CheckBox->setChecked(true);
    }
    else
    {
        m_pAutoMakeFrame_CheckBox->setChecked(false);
    }
    iCheck = CFuncTool::GetSend16Hex();
    if(iCheck > 0)
    {
        m_pSend16Data_CheckBox->setChecked(true);
    }
    else
    {
        m_pSend16Data_CheckBox->setChecked(false);
    }
    iCheck = CFuncTool::GetRecv16Hex();
    if(iCheck > 0)
    {
        m_pRecv16Data_CheckBox->setChecked(true);
    }
    else
    {
        m_pRecv16Data_CheckBox->setChecked(false);
    }

    pMainHLayout->addLayout(pVLayout);
    pMainHLayout->addWidget(m_pSendText_LineEdit, 1);

    QGroupBox* pGroup = new QGroupBox(tr(""));
    pGroup->setLayout(pMainHLayout);

    return pGroup;
}
QGroupBox* CUpdateTab::CreateSend()
{
    QGroupBox* pGroup = new QGroupBox(tr(""));

    QHBoxLayout* pHLayout = new QHBoxLayout();

    m_pSendData_Button = new QPushButton(tr("��������"));
    m_pSendData_Button->setMinimumHeight(50);
    m_pSendData_Button->setMinimumWidth(50);
    m_pSendData_Button->setEnabled(false);

    connect(m_pSendData_Button, SIGNAL(clicked()), this, SLOT(SendData()));

    pHLayout->addWidget(m_pSendData_Button);

    pGroup->setLayout(pHLayout);

    return pGroup;
}

// �������
QVBoxLayout* CUpdateTab::CreateUpdate()
{
    QVBoxLayout* pVLayout = new QVBoxLayout();
    pVLayout->addWidget(CreateBoardSelectGroup());
    pVLayout->addWidget(CreateManuUpdate());
    pVLayout->addStretch();
    if(true == g_oPageCheck.UpdateTab_ExportFile_Use())
    {
        pVLayout->addWidget(CreateExportGroup());
    }

    return pVLayout;
}
// �忨ѡ��
QGroupBox* CUpdateTab::CreateBoardSelectGroup()
{
    QGroupBox* pGroup = new QGroupBox(tr("�忨ѡ��"));

    // �����忨
    QButtonGroup* pButtonGroup = new QButtonGroup(this);
    m_pSelectAlone_Radio = new QRadioButton(tr("�����忨"));
    m_pSelectSystem_Radio = new QRadioButton(tr("�豸�忨"));
    pButtonGroup->addButton(m_pSelectAlone_Radio, UPDATE_SELECT_IS_ALONE);
    pButtonGroup->addButton(m_pSelectSystem_Radio, UPDATE_SELECT_IS_SYSTEM);
    connect(pButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(BoardTypeChanged(int)));

    QLabel* pLabelAddr = new QLabel(tr("��ַ"));
    m_pTargetAddr_LineEdit = new QLineEdit();
    m_pTargetAddr_LineEdit->setMaximumWidth(100);
    m_pTargetAddr_LineEdit->setValidator(m_p0x16_RegExpVal);
    QLabel* pLabelPort = new QLabel(tr("�˿�"));
    m_pTargetPort_LineEdit = new QLineEdit();
    m_pTargetPort_LineEdit->setMaximumWidth(100);
    m_pTargetPort_LineEdit->setValidator(m_p0x16_RegExpVal);

    m_pTargetResv_LineEdit = new QLineEdit();
    m_pTargetResv_LineEdit->setValidator(m_p0x16_RegExpVal);
    m_pTargetResv_LineEdit->hide();

    connect(m_pTargetAddr_LineEdit, SIGNAL(editingFinished()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pTargetAddr_LineEdit, ID_OF_TARGET_ADDR);
    connect(m_pTargetPort_LineEdit, SIGNAL(editingFinished()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pTargetPort_LineEdit, ID_OF_TARGET_PORT);
    connect(m_pTargetResv_LineEdit, SIGNAL(editingFinished()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pTargetResv_LineEdit, ID_OF_TARGET_USE);

    m_pTargetAddr_LineEdit->setText(QString::number(CFuncTool::GetComDestAddr(), 16).toUpper());
    m_pTargetPort_LineEdit->setText(QString::number(CFuncTool::GetComDestPort(), 16).toUpper());
    m_pTargetResv_LineEdit->setText(QString::number(CFuncTool::GetComBackup2(), 16).toUpper());

    // ȫ���忨
    m_pBoardSelect_ComboBox = new QComboBox();
    m_pBoardSelect_ComboBox->setMinimumWidth(280);
    m_pBoardSelect_ComboBox->addItem(NAME_OF_VERSION_ALL);
    connect(m_pBoardSelect_ComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(BoardSelect_Slot(const QString&)));

    QHBoxLayout* pHLayout1 = new QHBoxLayout();
    pHLayout1->addWidget(m_pSelectAlone_Radio);
    pHLayout1->addStretch();
    pHLayout1->addWidget(pLabelAddr);
    pHLayout1->addWidget(m_pTargetAddr_LineEdit);
    pHLayout1->addWidget(pLabelPort);
    pHLayout1->addWidget(m_pTargetPort_LineEdit);

    QHBoxLayout* pHLayout2 = new QHBoxLayout();
    pHLayout2->addWidget(m_pSelectSystem_Radio);
    pHLayout2->addWidget(m_pBoardSelect_ComboBox);

    QVBoxLayout* pVLayout = new QVBoxLayout();
    pVLayout->addLayout(pHLayout1);
    pVLayout->addLayout(pHLayout2);
    pGroup->setLayout(pVLayout);

    return pGroup;
}
QGroupBox* CUpdateTab::CreateManuUpdate()
{
    QGroupBox* pGroup = new QGroupBox(tr("���ܲ���"));              // ԭ��Ϊ���ֶ�����

    m_pVerQuary_Button = new QPushButton(tr("�忨��ѯ"));           // ԭ��Ϊ���汾��ѯ
    m_pVerQuary_Button->setMinimumWidth(80);
    m_pVerQuary_Button->setMinimumHeight(SIZE_HEIGHT_OF_BUTTON_RIGHT);
    m_pOpenFile_Button = new QPushButton(tr("���ļ�..."));
    m_pOpenFile_Button->setMinimumWidth(80);
    m_pOpenFile_Button->setMinimumHeight(SIZE_HEIGHT_OF_BUTTON_RIGHT);
    m_pFileShow_LineEdit = new QLineEdit();
    m_pFileShow_LineEdit->setReadOnly(true);
    m_pFileShow_LineEdit->setMinimumWidth(80);
    m_pFileShow_LineEdit->setMinimumWidth(SIZE_WIDTH_OF_BUTTON_RIGHT);

    QLabel* pTimeOutLabel = new QLabel(tr("��ʱ����"));
    pTimeOutLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    m_pTimeoutMs_LineEdit = new QLineEdit();
    m_pTimeoutMs_LineEdit->setMinimumWidth(SIZE_WIDTH_OF_BUTTON_RIGHT);
    m_pTimeoutMs_LineEdit->setValidator(m_pNum_RegExpVal);
    QLabel* pTimeOutCountLabel = new QLabel(tr("��ʱ����"));
    pTimeOutCountLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    m_pTimeoutCount_LineEdit = new QLineEdit();
    m_pTimeoutCount_LineEdit->setMinimumWidth(SIZE_WIDTH_OF_BUTTON_RIGHT);
    m_pTimeoutCount_LineEdit->setValidator(m_pNum_RegExpVal);

    connect(m_pTimeoutMs_LineEdit, SIGNAL(editingFinished()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pTimeoutMs_LineEdit, ID_OF_TIMEOUT_MS);
    connect(m_pTimeoutCount_LineEdit, SIGNAL(editingFinished()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pTimeoutCount_LineEdit, ID_OF_TIMEOUT_COUNT);

    m_iTimeOut_Save = CFuncTool::GetDownloadTimeoutMS();
    m_iReDownCount_Save = 5;//CFuncTool::GetDownloadTimeoutCount(); // ǿ��Ϊ5

    m_pTimeoutMs_LineEdit->setText(QString::number(m_iTimeOut_Save));
    m_pTimeoutCount_LineEdit->setText(QString::number(m_iReDownCount_Save));

    m_iTimeOut_Save = m_iTimeOut_Save / 100;                    // ת��: 2000ms / 100ms = 20

    QHBoxLayout* pHLayoutT0 = new QHBoxLayout();
    pHLayoutT0->addWidget(pTimeOutLabel);
    pHLayoutT0->addWidget(m_pTimeoutMs_LineEdit);
    pHLayoutT0->addStretch();
    QHBoxLayout* pHLayoutT1 = new QHBoxLayout();
    pHLayoutT1->addWidget(pTimeOutCountLabel);
    pHLayoutT1->addWidget(m_pTimeoutCount_LineEdit);
    pHLayoutT1->addStretch();

    m_pStartUpdate_Button = new QPushButton(tr("��ʼ����"));
    m_pStartUpdate_Button->setMinimumWidth(80);
    m_pStartUpdate_Button->setMinimumHeight(SIZE_HEIGHT_OF_BUTTON_RIGHT);
    m_pStopUpdate_Button = new QPushButton(tr("ֹͣ����"));
    m_pStopUpdate_Button->setMinimumWidth(80);
    m_pStopUpdate_Button->setMinimumHeight(SIZE_HEIGHT_OF_BUTTON_RIGHT);

    connect(m_pOpenFile_Button, SIGNAL(clicked()), this, SLOT(OpenFilePath()));
    connect(m_pVerQuary_Button, SIGNAL(clicked()), this, SLOT(VersionQuery()));
    connect(m_pStartUpdate_Button, SIGNAL(clicked()), this, SLOT(StartUpdate()));
    connect(m_pStopUpdate_Button, SIGNAL(clicked()), this, SLOT(StopUpdate()));
    QVBoxLayout* pVLayout = new QVBoxLayout();
    pVLayout->addWidget(m_pVerQuary_Button);
    pVLayout->addStretch();
    pVLayout->addWidget(m_pOpenFile_Button);
    pVLayout->addWidget(m_pFileShow_LineEdit);
    
    pVLayout->addStretch();
    pVLayout->addLayout(pHLayoutT0);
    pVLayout->addLayout(pHLayoutT1);
    pVLayout->addStretch();
    pVLayout->addWidget(m_pStartUpdate_Button);
    pVLayout->addStretch();
    pVLayout->addWidget(m_pStopUpdate_Button);
    
    pVLayout->addStretch();
    
    pGroup->setLayout(pVLayout);

    return pGroup;
}

// ��ʾ��
QVBoxLayout* CUpdateTab::CreateShow()
{
    QVBoxLayout* pVLayout = new QVBoxLayout();

    m_pRecvShow_TextEdit = new QTextEdit();
    m_pRecvShow_TextEdit->setReadOnly(true);
    m_pTipShow_TextEdit = new QTextEdit();
    m_pTipShow_TextEdit->setReadOnly(true);

    m_pRecvShow_TextEdit->document()->setMaximumBlockCount(1024);
    m_pTipShow_TextEdit->document()->setMaximumBlockCount(1024);

    QSplitter* pSplitter = new QSplitter(Qt::Vertical);

    pSplitter->addWidget(m_pRecvShow_TextEdit);
    pSplitter->addWidget(m_pTipShow_TextEdit);

    pSplitter->setStretchFactor(0, 70);
    pSplitter->setStretchFactor(1, 30);

    connect(m_pRecvShow_TextEdit, SIGNAL(textChanged()), this, SLOT(ShowRecvAutoScroll_Slot()));
    connect(m_pTipShow_TextEdit, SIGNAL(textChanged()), this, SLOT(ShowTipAutoScroll_Slot()));

    pVLayout->addWidget(pSplitter);
   

    return pVLayout;
}


// �����豸���ź�״̬
void CUpdateTab::SetDeviceSignalStatus(int iValue)
{
    emit DeviceSignalStatus_Signal(iValue);
}
void CUpdateTab::DeviceSignalStatus_Slot(int iValue)
{
    if(iValue > 0)
    {
        m_p1PPSLVDS_Label->setPixmap(QPixmap(":/images/green.png"));
    }
    else
    {
        m_p1PPSLVDS_Label->setPixmap(QPixmap(":/images/red.png"));
    }
}

// ��ʼ����������
void CUpdateTab::InitComInfo()
{
    // ���ں�
    m_pComName_ComboBox->addItem("COM1");
    m_pComName_ComboBox->addItem("COM2");
    m_pComName_ComboBox->addItem("COM3");
    m_pComName_ComboBox->addItem("COM4");
    m_pComName_ComboBox->addItem("COM5");
    m_pComName_ComboBox->addItem("COM6");
    m_pComName_ComboBox->addItem("COM7");
    m_pComName_ComboBox->addItem("COM8");
    m_pComName_ComboBox->addItem("COM9");
    m_pComName_ComboBox->addItem("COM10");

    m_pComName_ComboBox->addItem("COM11");
    m_pComName_ComboBox->addItem("COM12");
    m_pComName_ComboBox->addItem("COM13");
    m_pComName_ComboBox->addItem("COM14");
    m_pComName_ComboBox->addItem("COM15");
    m_pComName_ComboBox->addItem("COM16");
    m_pComName_ComboBox->addItem("COM17");
    m_pComName_ComboBox->addItem("COM18");
    m_pComName_ComboBox->addItem("COM19");
    m_pComName_ComboBox->addItem("COM20");

    m_pComName_ComboBox->addItem("COM21");
    m_pComName_ComboBox->addItem("COM22");
    m_pComName_ComboBox->addItem("COM23");
    m_pComName_ComboBox->addItem("COM24");
    m_pComName_ComboBox->addItem("COM25");
    m_pComName_ComboBox->addItem("COM26");
    m_pComName_ComboBox->addItem("COM27");
    m_pComName_ComboBox->addItem("COM28");
    m_pComName_ComboBox->addItem("COM29");
    m_pComName_ComboBox->addItem("COM30");

    m_pComName_ComboBox->addItem("COM31");
    m_pComName_ComboBox->addItem("COM32");
    m_pComName_ComboBox->addItem("COM33");
    m_pComName_ComboBox->addItem("COM34");
    m_pComName_ComboBox->addItem("COM35");
    m_pComName_ComboBox->addItem("COM36");
    m_pComName_ComboBox->addItem("COM37");
    m_pComName_ComboBox->addItem("COM38");
    m_pComName_ComboBox->addItem("COM39");
    m_pComName_ComboBox->addItem("COM40");

    m_pComName_ComboBox->addItem("COM41");
    m_pComName_ComboBox->addItem("COM42");
    m_pComName_ComboBox->addItem("COM43");
    m_pComName_ComboBox->addItem("COM44");
    m_pComName_ComboBox->addItem("COM45");
    m_pComName_ComboBox->addItem("COM46");
    m_pComName_ComboBox->addItem("COM47");
    m_pComName_ComboBox->addItem("COM48");

    // ������
    m_pComBaud_ComboBox->addItem("9600");
    m_pComBaud_ComboBox->addItem("14400");
    m_pComBaud_ComboBox->addItem("19200");
    m_pComBaud_ComboBox->addItem("38400");

    m_pComBaud_ComboBox->addItem("56000");
    m_pComBaud_ComboBox->addItem("57600");
    m_pComBaud_ComboBox->addItem("115200");
    m_pComBaud_ComboBox->addItem("128000");
    m_pComBaud_ComboBox->addItem("256000");

    // ����λ
    m_pComDataBit_ComboBox->addItem("5");
    m_pComDataBit_ComboBox->addItem("6");
    m_pComDataBit_ComboBox->addItem("7");
    m_pComDataBit_ComboBox->addItem("8");

    // ֹͣλ
    m_pComStopBit_ComboBox->addItem("1");
    m_pComStopBit_ComboBox->addItem("1.5");
    m_pComStopBit_ComboBox->addItem("2");

    // У��λ
    m_pComCheck_ComboBox->addItem("��");
    m_pComCheck_ComboBox->addItem("��");
    m_pComCheck_ComboBox->addItem("ż");
    m_pComCheck_ComboBox->addItem("��־");
    m_pComCheck_ComboBox->addItem("�ո�");
}

// �رմ���
void CUpdateTab::CloseCom()
{
    m_pMyComBasic->CloseComPort();
    m_pComStatus_Label->setPixmap(QPixmap(":/images/red.png"));
    m_bComIsOpened = false;
    m_pComOpen_Button->setText(tr("�򿪴���"));
    SetFuncButtonEnable(false);
}

// �ر�����
void CUpdateTab::CloseNet()
{
    m_pMyComBasic->CloseNetPort();
    m_pNetStatus_Label->setPixmap(QPixmap(":/images/red.png"));
    m_bNetIsOpened = false;
    m_pNetOpen_Button->setText(tr("������"));
    SetFuncButtonEnable(false);
    m_pNetConnectTest_Button->setEnabled(false);
}

bool CUpdateTab::ComNetIsOpen()
{
    return (m_bComIsOpened || m_bNetIsOpened);
}

// ��/�رմ���
void CUpdateTab::OpenCloseCom()
{
    if(true == m_bComIsOpened)
    {
        CloseCom();
        return;
    }

    // ���ں�
    QString strComName = m_pComName_ComboBox->currentText();

    // ������
    QString strComBaud = m_pComBaud_ComboBox->currentText();
    int iComBaud = strComBaud.toInt();

    // ����λ
    QString strComDataBit = m_pComDataBit_ComboBox->currentText();
    int iComDataBit = 8;

    // ֹͣλ
    int iComStopBit = 0;

    // У��λ
    int iComCheck = 0;

    CloseCom();
    CloseNet();
    char chComName[1024] = {0};
    sprintf_s(chComName, sizeof(chComName), "\\\\.\\%s", strComName.toLatin1().data());
    m_pMyComBasic->SetComPort(chComName, iComBaud, iComDataBit, iComStopBit, iComCheck);
    if(true == m_pMyComBasic->OpenComPort())
    {
        m_bComIsOpened = true;
        m_pComStatus_Label->setPixmap(QPixmap(":/images/green.png"));
        m_pComOpen_Button->setText(tr("�رմ���"));

        SetFuncButtonEnable(true);

        // INI
        CFuncTool::SetComIndex(m_pComName_ComboBox->currentIndex());
        CFuncTool::SetComBaud(m_pComBaud_ComboBox->currentIndex());

        // ����Դ��ַ(���һ��ʼ�ʹ����ӵ���û��Դ��ַ������)
        SetSourceAddr(GetSourceAddr(), GetSourcePort(), GetSourceUse());

        // �·������������Ƿ�ɹ�
        CheckConnect();
    }
    else
    {
        ShowTipInfoFunc("�򿪴���ʧ��");
    }
}

// ��/�ر�����
void CUpdateTab::OpenCloseNet()
{
    if(true == m_bNetIsOpened)
    {
        CloseNet();
        return;
    }

    // IP��ַ
    QString strIPAddr = m_pNetIPAddr_LineEdit->text();
    if(strIPAddr.length() < 7)
    {
        return;
    }

    // IP�˿�
    QString strIPPort = m_pNetIPPort_LineEdit->text();
    if(strIPPort.length() < 1)
    {
        return;
    }

    CloseNet();
    CloseCom();
    m_pMyComBasic->SetNetPort(strIPAddr.toLatin1().data(), strIPPort.toInt());
    if(true == m_pMyComBasic->OpenNetPort())
    {
        m_bNetIsOpened = true;
        m_pNetStatus_Label->setPixmap(QPixmap(":/images/green.png"));
        m_pNetOpen_Button->setText(tr("�ر�����"));

        SetFuncButtonEnable(true);
        m_pNetConnectTest_Button->setEnabled(true);

        // ����Դ��ַ(���һ��ʼ�ʹ����ӵ���û��Դ��ַ������)
        SetSourceAddr(GetSourceAddr(), GetSourcePort(), GetSourceUse());

        // �·������������Ƿ�ɹ�
        CheckConnect();
    }
    else
    {
        ShowTipInfoFunc("������ʧ��");
    }
    
}

// ���ڸı�
void CUpdateTab::ComNameChanged(const QString& text)
{
    if(true == m_bComIsOpened)
    {
        CloseCom();
    }
}

// ��������������
void CUpdateTab::SaveInput(int iID)
{
    QString strInput = "";
    int iInput = 0;
    bool bOK = false;
    switch(iID)
    {
    case ID_OF_IP_ADDRESS:          // IP��ַ
        strInput = m_pNetIPAddr_LineEdit->text();
        CFuncTool::SetUDPAddr(strInput.toLatin1().data());
        break;
    case ID_OF_IP_PORT:             // IP�˿�
        strInput = m_pNetIPPort_LineEdit->text();
        CFuncTool::SetUDPPort(strInput.toInt());
        break;
    case ID_OF_SOURCE_ADDR:         // Դ��ַ
        strInput = m_pSourceAddr_LineEdit->text();
        m_pSourceAddr_LineEdit->setText(strInput.toUpper());
        iInput = strInput.toInt(&bOK, 16);
        CFuncTool::SetComSvrAddr(iInput);
        break;
    case ID_OF_SOURCE_PORT:         // Դ�˿�
        strInput = m_pSourcePort_LineEdit->text();
        m_pSourcePort_LineEdit->setText(strInput.toUpper());
        iInput = strInput.toInt(&bOK, 16);
        CFuncTool::SetComSvrPort(iInput);
        break;
    case ID_OF_SOURCE_USE:          // ����1
        strInput = m_pSourceResv_LineEdit->text();
        m_pSourceResv_LineEdit->setText(strInput.toUpper());
        CFuncTool::SetComBackup1(strInput.toInt(&bOK, 16));
        break;
    case ID_OF_TARGET_ADDR:         // Ŀ�ĵ�ַ
        strInput = m_pTargetAddr_LineEdit->text();
        m_pTargetAddr_LineEdit->setText(strInput.toUpper());
        CFuncTool::SetComDestAddr(strInput.toInt(&bOK, 16));
        break;
    case ID_OF_TARGET_PORT:         // Ŀ�Ķ˿�
        strInput = m_pTargetPort_LineEdit->text();
        m_pTargetPort_LineEdit->setText(strInput.toUpper());
        CFuncTool::SetComDestPort(strInput.toInt(&bOK, 16));
        break;
    case ID_OF_TARGET_USE:          // ����2
        strInput = m_pTargetResv_LineEdit->text();
        m_pTargetResv_LineEdit->setText(strInput.toUpper());
        CFuncTool::SetComBackup2(strInput.toInt(&bOK, 16));
        break;
    case ID_OF_FRAME_NUM:           // ֡ ��
        strInput = m_pFrameNo_LineEdit->text();
        m_pFrameNo_LineEdit->setText(strInput.toUpper());
        CFuncTool::SetComNumber(strInput.toInt(&bOK, 16));
        break;
    case ID_OF_FRAME_COMMAND:       // ������
        strInput = m_pFrameCmd_LineEdit->text();
        m_pFrameCmd_LineEdit->setText(strInput.toUpper());
        CFuncTool::SetComCommand(strInput.toInt(&bOK, 16));
        break;
    case ID_OF_RESEND_CIRCLE:       // �Զ��ط�����
        strInput = m_pTimeReSend_LineEdit->text();
        CFuncTool::SetComCircle(strInput.toInt());
        break;
    case ID_OF_TIMEOUT_MS:          // ��ʱms
        strInput = m_pTimeoutMs_LineEdit->text();
        iInput = strInput.toInt();
        m_iTimeOut_Save = iInput;
        CFuncTool::SetDownloadTimeoutMS(iInput);
        m_iTimeOut_Save = m_iTimeOut_Save / 100;                    // ת��: 2000ms / 100ms = 20
        break;
    case ID_OF_TIMEOUT_COUNT:       // ��ʱ����
        strInput = m_pTimeoutCount_LineEdit->text();
        iInput = strInput.toInt();
        m_iReDownCount_Save = iInput;
        CFuncTool::SetDownloadTimeoutCount(iInput);
        break;
    default:
        break;
    }
}

// ��ȡ Դ��Ŀ�ĺ������
unsigned char CUpdateTab::GetSourceAddr()
{
    return ((m_pSourceAddr_LineEdit->text()).toInt(NULL, 16));
}
unsigned char CUpdateTab::GetSourcePort()
{
    return ((m_pSourcePort_LineEdit->text()).toInt(NULL, 16));
}
unsigned char CUpdateTab::GetSourceUse()
{
    return ((m_pSourceResv_LineEdit->text()).toInt(NULL, 16));
}
unsigned char CUpdateTab::GetTargetAddr()
{
    return ((m_pTargetAddr_LineEdit->text()).toInt(NULL, 16));
}
unsigned char CUpdateTab::GetTargetPort()
{
    return ((m_pTargetPort_LineEdit->text()).toInt(NULL, 16));
}
unsigned char CUpdateTab::GetTargetUse()
{
    return ((m_pTargetResv_LineEdit->text()).toInt(NULL, 16));
}
unsigned char CUpdateTab::GetFrameNo()
{
    return ((m_pFrameNo_LineEdit->text()).toInt(NULL, 16));
}

void CUpdateTab::FrameNoInc(unsigned char chCurFrameNo)
{
    emit FrameNoInc_Signal(chCurFrameNo);
}

void CUpdateTab::BoardSelectFill()
{
    emit BoardSelectFill_Signal();
}

// ��ȡ������Ŀ�ĵ�ַ
void CUpdateTab::GetTargetForGNSS(QString& strAddr, QString& strPort)
{
    strAddr = m_pTargetAddr_LineEdit->text();
    strPort = m_pTargetPort_LineEdit->text();
}
void CUpdateTab::SetTargetForGNSS(QString strAddr, QString strPort)
{
    m_pTargetAddr_LineEdit->setText(strAddr);
    m_pTargetPort_LineEdit->setText(strPort);
}

// ѡ�����ڻ��Ǵ���
void CUpdateTab::ComNetChanged(int iSelect)
{
    m_pComOpen_Button->setEnabled(false);
    m_pNetOpen_Button->setEnabled(false);
    if(COMNET_SELECT_IS_COM == iSelect)
    {
        m_bUseIsCom = true;
        CloseNet();
        m_pComOpen_Button->setEnabled(true);
        m_pMyComBasic->SetCurrentIsCom(true);
    }
    else if(COMNET_SELECT_IS_NET == iSelect)
    {
        m_bUseIsCom = false;
        CloseCom();
        m_pNetOpen_Button->setEnabled(true);
        m_pMyComBasic->SetCurrentIsCom(false);
    }
    CFuncTool::SetComNetSelect(iSelect);
}

void CUpdateTab::SelectCom_Slot()
{
    if(true == m_bUseIsCom) 
    {
        m_pSelectCom_GroupBox->setChecked(true);
        return;
    }

    m_pSelectNet_GroupBox->setChecked(false);

    m_bUseIsCom = true;
    CloseNet();
    m_pComOpen_Button->setEnabled(true);
    m_pMyComBasic->SetCurrentIsCom(true);

    CFuncTool::SetComNetSelect(COMNET_SELECT_IS_COM);
}
void CUpdateTab::SelectNet_Slot()
{
    if(false == m_bUseIsCom) 
    {
        m_pSelectNet_GroupBox->setChecked(true);
        return;
    }

    m_pSelectCom_GroupBox->setChecked(false);

    m_bUseIsCom = false;
    CloseCom();
    m_pNetOpen_Button->setEnabled(true);
    m_pMyComBasic->SetCurrentIsCom(false);

    CFuncTool::SetComNetSelect(COMNET_SELECT_IS_NET);
}

QStringList g_strListAddrCheck;

// �� ��
void CUpdateTab::SendData()
{
    QString strData = m_pSendText_LineEdit->text();
    strData = strData.trimmed();        // �����˿ո�

    // �ȼ���Ƿ���ѡ������
    if("+debug" == strData)         // ���Թ���
    {
        m_pMyComBasic->SetShowDebug(true);
        m_pSendText_LineEdit->setText("");
        return;
    }
    if("-debug" == strData)         // ȡ�� ���Թ���
    {
        m_pMyComBasic->SetShowDebug(false);
        m_pSendText_LineEdit->setText("");
        return;
    }
    if("opennavtfadmin2015" == strData)             // �������߹ر�ĳЩ���ܡ����� open����Ҫ����ǰ��
    {
        m_bSomeFuncEnabled = !m_bSomeFuncEnabled;
        SetSomeFuncEnabled(m_bSomeFuncEnabled);
        m_pSendText_LineEdit->setText("");
        return;
    }
    if(strData.contains("open"))    // �Ƿ���ʾĳЩ��ַ����Ϣ
    {
        g_strListAddrCheck.clear();
        g_strListAddrCheck = strData.split(" ", QString::SkipEmptyParts);
        m_pSendText_LineEdit->setText("");
        return;
    }
    if("stoptip" == strData)        // ���Ρ����еİ忨���ź������ʾ�� ���
    {
        g_bShowBoardTip = false;
        m_pSendText_LineEdit->setText("");
        return;
    }
    

    // ����������Դ��ַ����Ϣ
    SetSourceTargetInfo(COMMAND_IS_AT);

    // ��������͡���ť���������������Ƿ���16����; ��������ఴť��ǿ������Ϊ16����
    SetFrameDataStyle(true, true, true, true);

    char chData[2048] = {0};
    strcpy_s(chData, sizeof(chData), strData.toLatin1().data());
    if(true == m_pEnter_CheckBox->isChecked())                      // �س�
    {
        int iLength = strlen(chData);
        chData[iLength] = 0x0D;
        chData[iLength+1] = 0x0A;
        chData[iLength+2] = '\0';
    }

    // ĳЩ���ܱ����Σ�Ŀ��̶�Ϊ [0, 0]
    if(false == m_bSomeFuncEnabled)
    {
        // ����Ŀ�ĵ�ַ
        SetTargetAddr(0, 0, GetTargetUse());
    }

    // д����/���ڡ�pszDataBuf�������������ݣ�������Դ��ַ��������Ϣ
    SendComNetData(0, chData, 0);

    // ������־
    CmdLog_Func(GetTargetAddr(), GetTargetPort(), strData);
}

// ѡ��ĳ���忨
void CUpdateTab::BoardSelect_Slot(const QString& strBoard)
{
    if(strBoard.isEmpty()) return;
    if(NAME_OF_VERSION_ALL == strBoard) 
    {
        return;
    }

    char chName[1024] = {0};
    if(false == GetBoardNameOnComBox(strBoard.toLatin1(), chName, sizeof(chName)))
    {
        //ShowTipInfoFunc("��ȡ�忨���ƴ���");
        return;
    }

    st_BoardVerInfo* pVerInfo = m_BoardVerInfo_map[chName];
    if(NULL == pVerInfo)
    {
        //ShowTipInfoFunc("��ȡ�忨��Ϣ�ṹ����");
        return;
    }

    m_pTargetAddr_LineEdit->setText(QString::number(pVerInfo->m_chAddr, 16));
    m_pTargetPort_LineEdit->setText(QString::number(pVerInfo->m_chPort, 16));

    // ToolTip
    m_pBoardSelect_ComboBox->setToolTip(strBoard);
}

// ��������
void CUpdateTab::SendComNetData(unsigned char chCommand, const char* pszDataBuf, int iDataLength_in)
{
    m_pMyComBasic->SendComNetData(chCommand, pszDataBuf, iDataLength_in);
}

// ȫ���忨 �汾��ѯ
void CUpdateTab::VersionQueryAllBoard()
{
    m_pBoardSelect_ComboBox->setCurrentIndex(0);
    BoardTypeChanged(UPDATE_SELECT_IS_SYSTEM);
    VersionQuery();
}

//  �� �ο������ �� ���ذ� ���� getsta ����
void CUpdateTab::Send_getsta_To_RInputAndMain()
{
    char chData[1024] = {0};

    st_BoardVerInfo* pVerInfo = NULL;
    BOARDVERINFOMAPTYPE::iterator itC = m_BoardVerInfo_map.begin();
    for(; itC != m_BoardVerInfo_map.end(); ++itC)
    {
        pVerInfo = itC->second;
        if(NULL == pVerInfo) continue;
    
        // �ο������, ���ذ�
        if(("Reference_Input" == pVerInfo->m_strOriName) || ("Main_Control" == pVerInfo->m_strOriName))
        {
            SetSourceAddr(GetSourceAddr(), GetSourcePort(), GetSourceUse());
            SetTargetAddr(pVerInfo->m_chAddr, pVerInfo->m_chPort, pVerInfo->m_chUse);
            SetCommand(COMMAND_IS_AT);

            // ��������͡���ť���������������Ƿ���16����; ��������ఴť��ǿ������Ϊ16����
            SetFrameDataStyle(true, true, true, true);

            strcpy_s(chData, sizeof(chData), "getsta");
            if(true == m_pEnter_CheckBox->isChecked())                      // �س�
            {
                int iLength = strlen(chData);
                chData[iLength] = 0x0D;
                chData[iLength+1] = '\0';
            }

            SendComNetData(0, chData, 0);
        }
    }
}

// ѡ��Ƶ������
void CUpdateTab::SendFreqSelect(int iIndex)
{
    char chData[1024] = {0};

    st_BoardVerInfo* pVerInfo = NULL;
    BOARDVERINFOMAPTYPE::iterator itC = m_BoardVerInfo_map.begin();
    for(; itC != m_BoardVerInfo_map.end(); ++itC)
    {
        pVerInfo = itC->second;
        if(NULL == pVerInfo) continue;

        // �ο������, ���ذ�
        if("Main_Control" == pVerInfo->m_strOriName)
        {
            SetSourceAddr(GetSourceAddr(), GetSourcePort(), GetSourceUse());
            SetTargetAddr(pVerInfo->m_chAddr, pVerInfo->m_chPort, pVerInfo->m_chUse);
            SetCommand(COMMAND_IS_AT);

            // ��������͡���ť���������������Ƿ���16����; ��������ఴť��ǿ������Ϊ16����
            SetFrameDataStyle(true, true, true, true);

            if(0 == iIndex) sprintf_s(chData, sizeof(chData), "useclk %d", 0);
            else sprintf_s(chData, sizeof(chData), "useclk %d", 1);
            if(true == m_pEnter_CheckBox->isChecked())                      // �س�
            {
                int iLength = strlen(chData);
                chData[iLength] = 0x0D;
                chData[iLength+1] = '\0';
            }

            SendComNetData(0, chData, 0);
        }
        else if("Reference_Input" == pVerInfo->m_strOriName)
        {
            if(iIndex > 0)
            {
                SetSourceAddr(GetSourceAddr(), GetSourcePort(), GetSourceUse());
                SetTargetAddr(pVerInfo->m_chAddr, pVerInfo->m_chPort, pVerInfo->m_chUse);
                SetCommand(COMMAND_IS_AT);

                // ��������͡���ť���������������Ƿ���16����; ��������ఴť��ǿ������Ϊ16����
                SetFrameDataStyle(true, true, true, true);

                sprintf_s(chData, sizeof(chData), "selclk %d", iIndex);
                if(true == m_pEnter_CheckBox->isChecked())                      // �س�
                {
                    int iLength = strlen(chData);
                    chData[iLength] = 0x0D;
                    chData[iLength+1] = '\0';
                }

                SendComNetData(0, chData, 0);
            }
        }
    }
}

// ѡ����������
void CUpdateTab::SendPlusSelect(int iIndex)
{
    char chData[1024] = {0};

    st_BoardVerInfo* pVerInfo = NULL;
    BOARDVERINFOMAPTYPE::iterator itC = m_BoardVerInfo_map.begin();
    for(; itC != m_BoardVerInfo_map.end(); ++itC)
    {
        pVerInfo = itC->second;
        if(NULL == pVerInfo) continue;

        // �ο������
        if("Reference_Input" == pVerInfo->m_strOriName)
        {
            SetSourceAddr(GetSourceAddr(), GetSourcePort(), GetSourceUse());
            SetTargetAddr(pVerInfo->m_chAddr, pVerInfo->m_chPort, pVerInfo->m_chUse);
            SetCommand(COMMAND_IS_AT);

            // ��������͡���ť���������������Ƿ���16����; ��������ఴť��ǿ������Ϊ16����
            SetFrameDataStyle(true, true, true, true);

            sprintf_s(chData, sizeof(chData), "selpps %d", iIndex);
            if(true == m_pEnter_CheckBox->isChecked())                      // �س�
            {
                int iLength = strlen(chData);
                chData[iLength] = 0x0D;
                chData[iLength+1] = '\0';
            }

            SendComNetData(0, chData, 0);
        }
    }
}

// ���� Ŀ���ַ�ַ������� ����
void CUpdateTab::SendChangeData(unsigned char chAddr, unsigned char chPort, char* pchData)
{
    SetSourceAddr(GetSourceAddr(), GetSourcePort(), GetSourceUse());
    SetTargetAddr(chAddr, chPort, 0);
    SetCommand(COMMAND_IS_AT);

    // ��������͡���ť���������������Ƿ���16����; ��������ఴť��ǿ������Ϊ16����
    SetFrameDataStyle(true, true, true, true);

    if(true == m_pEnter_CheckBox->isChecked())                      // �س�
    {
        int iLength = strlen(pchData);
        pchData[iLength] = 0x0D;
        pchData[iLength+1] = '\0';
    }

    SendComNetData(0, pchData, 0);
}

void CUpdateTab::paintEvent(QPaintEvent*)
{
    // Style Sheet
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CUpdateTab::keyPressEvent(QKeyEvent* keyEvent)
{
    if(Qt::Key_Return == keyEvent->key() || Qt::Key_Enter == keyEvent->key())
    {
        if((m_pSendText_LineEdit->hasFocus()) && (m_pSendText_LineEdit->text().length() > 0))
        {
            if((true == m_bComIsOpened) || (true == m_bNetIsOpened)) SendData();
        }
    }
}

// bManualSet=true, �ֹ�ѡ�񣻷�����������������
void CUpdateTab::SetFrameDataStyle(bool bManualSet, bool bSendIs16Hex, bool bRecvIs16Hex, bool bHasFrame)
{
    if(true == bManualSet)
    {
        CheckSend16DataClicked();
        CheckRecv16DataClicked();
        AutoMakeFrameClicked();
    }
    else
    {
        m_pMyComBasic->SetSendDataIs16Hex(bSendIs16Hex);
        m_pMyComBasic->SetRecvDataIs16Hex(bRecvIs16Hex);
        m_pMyComBasic->SetDataHasFrame(bHasFrame);
    }
}

// ����������Դ��ַ����Ϣ
void CUpdateTab::SetSourceTargetInfo(int iCmd)
{
    // ����Դ��ַ
    SetSourceAddr(GetSourceAddr(), GetSourcePort(), GetSourceUse());

    // ����Ŀ�ĵ�ַ
    SetTargetAddr(GetTargetAddr(), GetTargetPort(), GetTargetUse());

    // ����������
    SetCommand(iCmd);
}

// ����Դ��ַ
void CUpdateTab::SetSourceAddr(int iAddr, int iPort, int iResv)
{
    m_pMyComBasic->SetSourceAddr(iAddr, iPort, iResv);
}

// ����Ŀ�ĵ�ַ
void CUpdateTab::SetTargetAddr(int iAddr, int iPort, int iResv)
{
    m_pMyComBasic->SetTargetAddr(iAddr, iPort, iResv);
}

// ����������
void CUpdateTab::SetCommand(int iCmd)
{
    int iFrameNum = GetFrameNo();
    m_pMyComBasic->SetCommand(iFrameNum, iCmd);
    FrameNoInc(iFrameNum);
}

// �ط���ʱ��
void CUpdateTab::ReSendTimeReach()
{
    SendData();
}

// �� ��
void CUpdateTab::ClearShowInfo()
{
    m_pRecvShow_TextEdit->clear();
    m_pTipShow_TextEdit->clear();
}

// ��ʾ��������
void CUpdateTab::ShowRecvDataFunc(unsigned char chAddr, unsigned char chPort, const char* pszShowData, bool bChanged, int iR, int iG, int iB)
{
    QString strInfo = pszShowData;
    emit ShowRecvText_Signal(chAddr, chPort, strInfo, bChanged, iR, iG, iB);
}

// ��ʾ��������
void CUpdateTab::ShowSendDataFunc(unsigned char chAddr, unsigned char chPort, const char* pszShowData, int iLength)
{
    QString strShow = "";
    QString strOne = "";
    unsigned char chData[1024] = {0};
    unsigned char chOne = 0;
    for(int i = 0; i < iLength; ++i)
    {
        chOne = pszShowData[i];
        CFuncTool::Change16ToAscii(chOne, chData, sizeof(chData));
        strOne = (char*)chData;
        strShow += strOne;
    }
    strShow = strShow.toUpper();

    static int stiAllCount = 0;
    ++stiAllCount;
    if(true == m_pDebug_CheckBox->isChecked())
    {
        QString strCount = QString("[%1]").arg(stiAllCount);
        emit ShowSendText_Signal(chAddr, chPort, strCount);
    }

    emit ShowSendText_Signal(chAddr, chPort, strShow);
}

// ��ʾ��ʾ��Ϣ
void CUpdateTab::ShowTipInfoFunc(const char* pszShowData, QColor rgb)
{
    QString strInfo = pszShowData;
    emit ShowTipText_Signal(strInfo, rgb);
}

void CUpdateTab::SetRecvShowFunc(int iShowType)
{
    m_iRecvShowType = iShowType;
}

void CUpdateTab::ShowRecvText_Slot(unsigned char chAddr, unsigned char chPort, const QString strInfo, bool bChanged, int iR, int iG, int iB)
{
    // ����
    int iCount = g_strListAddrCheck.count();
    if(iCount > 1)
    {
        bool bOK = false;
        bool bShow = false;
        for(int i = 1; i < iCount; ++i)
        {
            if(g_strListAddrCheck[i].toInt(&bOK, 16) == chAddr)
            {
                bShow = true;
                break;
            }
        }
        if(false == bShow) return;
    }
    

    // ���� ��--> ����ʾͷ
    QString strAddInfo = "";

    // ����
    if((true == g_oPageCheck.UpdateTab_ExportFile_Use()) && (m_pAllowExport_CheckBox->isChecked()))
    {
        // �Ƿ�����ʱ��
        QString strExport = strInfo;
        if(true == m_pExportUseTime_CheckBox->isChecked())
        {
            strExport.remove(QChar(0x0A));
            strExport.remove(QChar(0x0D));
            strExport += tr(" %1").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        }

        if(NULL != m_hExportFile)
        {
            DWORD dwWrites;
            WriteFile((HANDLE)m_hExportFile, strExport.toLatin1().data(), strExport.toLatin1().length(), &dwWrites, NULL);
            char chEnter[2] = {0x0D, 0x0A};
            WriteFile((HANDLE)m_hExportFile, chEnter, 2, &dwWrites, NULL);
        }
        else
        {
            QString fileName = m_pExportFileShow_LineEdit->text();
            if(!(fileName.isEmpty()))
            {
                CloseExportFile();
                OpenExportFile(fileName.toLatin1().data());
                DWORD dwWrites;
                WriteFile((HANDLE)m_hExportFile, strExport.toLatin1().data(), strExport.toLatin1().length(), &dwWrites, NULL);
                char chEnter[2] = {0x0D, 0x0A};
                WriteFile((HANDLE)m_hExportFile, chEnter, 2, &dwWrites, NULL);
            }
        }
    }

    // Color
    if((0 == iR) && (0 == iG) && (0 == iB))
    {
        if(true == bChanged)
        {
            m_pRecvShow_TextEdit->setTextColor(QColor(0, 0, 250));
        }
        else
        {
            m_pRecvShow_TextEdit->setTextColor(QColor(0, 0, 10));
        }
    }
    else
    {
        m_pRecvShow_TextEdit->setTextColor(QColor(iR, iG, iB));
    }


    // Դ��ַ����ַ��ͬ�����
    static unsigned char st_chAddr = 0xFF;
    static unsigned char st_chPort = 0xFF;

    if((st_chAddr != chAddr) || (st_chPort != chPort))
    {
        char chDataAddr[16] = {0};
        char chDataPort[16] = {0};
        sprintf_s(chDataAddr, sizeof(chDataAddr), "%02X", chAddr);
        sprintf_s(chDataPort, sizeof(chDataPort), "%02X", chPort);
        if(m_pAutoMakeFrame_CheckBox->isChecked()) strAddInfo = tr("\n[%1,%2] ").arg(chDataAddr).arg(chDataPort);
        st_chAddr = chAddr;
        st_chPort = chPort;
    }

    // Info
    strAddInfo += strInfo;

    if(0 == m_iRecvShowType)
    {
        if(false == m_pRecv16Data_CheckBox->isChecked())
        {
            m_pRecvShow_TextEdit->insertPlainText(strAddInfo);

            // Scroll
            ShowRecvAutoScroll_Slot();
        }
        else
        {
            m_pRecvShow_TextEdit->append(strAddInfo);
        }
    }
    else if(1 == m_iRecvShowType)
    {
        m_pRecvShow_TextEdit->append(strAddInfo);
    }
    else if(2 == m_iRecvShowType)
    {
        m_pRecvShow_TextEdit->insertPlainText(strAddInfo);
        
        // Scroll
        ShowRecvAutoScroll_Slot();
    }
    else if(3 == m_iRecvShowType)
    {
        m_pRecvShow_TextEdit->insertPlainText(strAddInfo);
        
        // Scroll
        ShowRecvAutoScroll_Slot();
    }
    else
    {
        m_pRecvShow_TextEdit->append(strAddInfo);
    }
}
void CUpdateTab::ShowSendText_Slot(unsigned char chAddr, unsigned char chPort, const QString strInfo)
{
    // ���� "<-- " ��ʾͷ��
    QString strEnter = tr("\n");
    //if(true == g_oPageCheck.IsAdmin())
    if(true)                                // ���е�Ȩ�޶���ʾ
    {
        // ��������ֱ����ʾ������16����ת���ַ���ʾ
        char chDataAddr[16] = {0};
        char chDataPort[16] = {0};
        sprintf_s(chDataAddr, sizeof(chDataAddr), "%02X", chAddr);
        sprintf_s(chDataPort, sizeof(chDataPort), "%02X", chPort);
        QString strAddInfo = tr("\n[%1,%2]").arg(chDataAddr).arg(chDataPort) + "<-- ";

        if((NULL != m_pCmdItem_Cur) && (en_Cmd_A_Is_Update == m_pCmdItem_Cur->m_enFrameAType))  // ����
        {
            strAddInfo += strInfo;
        }
        else
        {
            char chData = 0;
            QStringList strList = strInfo.split(" ", QString::SkipEmptyParts);
            int iCount = strList.count();
            for(int i = 0; i < iCount; ++i)
            {
                chData = (char)strList[i].toInt(NULL, 16);
                if(0x0D != chData) strAddInfo += chData;
            }
        }

        // Send
        strAddInfo += strEnter;

        m_pRecvShow_TextEdit->setTextColor(QColor(0, 0, 150));

        // ͬʱ��Recv��ʾ(��������ShowRecvText_Slot�Ĺ���)
        if(0 == m_iRecvShowType)
        {
            m_pRecvShow_TextEdit->append(strAddInfo);
        }
        else if(1 == m_iRecvShowType)
        {
            m_pRecvShow_TextEdit->append(strAddInfo);
        }
        else if(2 == m_iRecvShowType)
        {
            m_pRecvShow_TextEdit->insertPlainText(strAddInfo);

            // Scroll
            ShowRecvAutoScroll_Slot();
        }
        else if(3 == m_iRecvShowType)
        {
            m_pRecvShow_TextEdit->insertPlainText(strAddInfo);

            // Scroll
            ShowRecvAutoScroll_Slot();
        }
        else
        {
            m_pRecvShow_TextEdit->append(strAddInfo);
        }
    }
}
void CUpdateTab::ShowTipText_Slot(const QString strInfo, QColor rgb)
{
    m_pTipShow_TextEdit->setTextColor(rgb);
    m_pTipShow_TextEdit->append(strInfo);
}

// Scroll
void CUpdateTab::ShowRecvAutoScroll_Slot()
{
    QTextCursor oCursor = m_pRecvShow_TextEdit->textCursor();
    {
        oCursor.movePosition(QTextCursor::End);
        m_pRecvShow_TextEdit->setTextCursor(oCursor);
    }
}
void CUpdateTab::ShowTipAutoScroll_Slot()
{
    QTextCursor oCursor = m_pTipShow_TextEdit->textCursor();
    {
        oCursor.movePosition(QTextCursor::End);
        m_pTipShow_TextEdit->setTextCursor(oCursor);
    }
}


void CUpdateTab::FrameNoInc_Slot(int iFrameNo)
{
    unsigned char chCurFrameNo = (unsigned char)iFrameNo;
    ++chCurFrameNo;
    if(chCurFrameNo > 255) chCurFrameNo = 0;
    m_pFrameNo_LineEdit->setText(QString::number(chCurFrameNo, 16));
}

void CUpdateTab::BoardSelectFill_Slot()
{
    m_pBoardSelect_ComboBox->clear();
    m_pBoardSelect_ComboBox->setToolTip("");

    m_pBoardSelect_ComboBox->addItem(NAME_OF_VERSION_ALL);

    char chName[1024] = {0};
    std::string strReadedName = "";
    st_BoardVerInfo* pVerInfo = NULL;
    BOARDVERINFOMAPTYPE::iterator itC = m_BoardVerInfo_map.begin();
    for(; itC != m_BoardVerInfo_map.end(); ++itC)
    {
        pVerInfo = itC->second;
        if(NULL == pVerInfo) continue;

        strReadedName = m_BoardName_map[pVerInfo->m_strOriName.c_str()];
        if(strReadedName.empty())
        {
            strReadedName = "δ֪�忨";
            continue;                       // ����
        }

        // �ο������_Ref_Input_P0102_Vx.y
        sprintf_s(chName, sizeof(chName), "%s_%s_V%0.1f", strReadedName.c_str(), pVerInfo->m_strName.c_str(), pVerInfo->m_fVer);
        m_pBoardSelect_ComboBox->addItem(chName);

        // ���� GNSS �忨��Ŀ�ĵ�ַ
        if(strcmp(pVerInfo->m_strOriName.c_str(), "Reference_GNSS") == 0)
        {
            g_strGNSSAddr = QString::number(pVerInfo->m_chAddr, 16);
            g_strGNSSPort = QString::number(pVerInfo->m_chPort, 16);
        }
    }

    // ����Ӧ��� ?

}

// �������
void CUpdateTab::CmdFinished_Slot()
{
    SetAllEnabled(true);

    // My Image Tab
    if(NULL != m_pMyImageTab)
    {
        m_pMyImageTab->CmdFinished_Func();
    }
}

// �������
void CUpdateTab::CmdFinished_Func()
{
    emit CmdFinished_Signal();
}

// ִ������ʱ��ֹ��������
void CUpdateTab::SetAllEnabled(bool bEnable)
{
    if(false == bEnable)
    {
        // ͨ������
        m_pComOpen_Button->setEnabled(false);
        m_pNetOpen_Button->setEnabled(false);

        // ����
        m_pAutoReSend_CheckBox->setEnabled(false);
        m_pSendData_Button->setEnabled(false);

        // ����
        m_pVerQuary_Button->setEnabled(false);
        m_pStartUpdate_Button->setEnabled(false);

        StopAutoReSend();

        // My Image Tab
        if(NULL != m_pMyImageTab)
        {
            m_pMyImageTab->SetFuncButtonEnable(false);
        }
        if(NULL != m_pMyAdjustTab)
        {
            m_pMyAdjustTab->SetFuncButtonEnable(false);
        }
        if(NULL != m_pGNSSTab)
        {
            m_pGNSSTab->SetFuncButtonEnable(false);
        }
    }
    else
    {
        // ͨ������
        if(true == m_pSelectCom_GroupBox->isChecked())
        {
            m_pComOpen_Button->setEnabled(true);
        }
        else
        {
            m_pNetOpen_Button->setEnabled(true);
        }

        if((true == m_bComIsOpened) || (true == m_bNetIsOpened))
        {
            SetFuncButtonEnable(true);
        }
    }
}

// ֹͣ �Զ��ط�
void CUpdateTab::StopAutoReSend()
{
    m_pTimer->stop();
    m_pAutoReSend_CheckBox->setChecked(false);
}

// ѡ��Զ��ط����Զ���֡��16���ơ�����
void CUpdateTab::AutoReSendClicked()
{
    if(Qt::Checked == m_pAutoReSend_CheckBox->checkState())
    {
        m_pTimer->stop();
        int iInterval = m_pTimeReSend_LineEdit->text().toInt();
        if(iInterval > 10)
        {
            m_pTimer->setInterval(iInterval);
            m_pTimer->start();
        }
    }
    else
    {
        m_pTimer->stop();
    }
}
void CUpdateTab::RecvAllClicked()
{
    if(true == m_pRecvAll_CheckBox->isChecked())
    {
        m_pMyComBasic->SetRecvAllData(true);
    }
    else
    {
        m_pMyComBasic->SetRecvAllData(false);
    }
}
void CUpdateTab::AutoMakeFrameClicked()
{
    bool bChecked = m_pAutoMakeFrame_CheckBox->isChecked();
    m_pMyComBasic->SetDataHasFrame(bChecked);

    // INI
    if(true == bChecked)
    {
        CFuncTool::SetAutoMakeFrame(1);
    }
    else
    {
        CFuncTool::SetAutoMakeFrame(0);

        // ������ʾ(��������ʹ�û᲻ͣ����ʾ����������)
        //ShowTipInfoFunc("ע�⣺���豸ͨ����Ҫ��ѡ \"�Զ���֡\" ");
    }
}
void CUpdateTab::CheckSend16DataClicked()
{
    bool bChecked = m_pSend16Data_CheckBox->isChecked();
    m_pMyComBasic->SetSendDataIs16Hex(bChecked);

    // INI
    if(true == bChecked)
    {
        CFuncTool::SetSend16Hex(1);
    }
    else
    {
        CFuncTool::SetSend16Hex(0);
    }
}
void CUpdateTab::CheckRecv16DataClicked()
{
    bool bChecked = m_pRecv16Data_CheckBox->isChecked();
    m_pMyComBasic->SetRecvDataIs16Hex(bChecked);

    // INI
    if(true == bChecked)
    {
        CFuncTool::SetRecv16Hex(1);
    }
    else
    {
        CFuncTool::SetRecv16Hex(0);
    }
}
void CUpdateTab::CheckDebugClicked()
{
    if(true == m_pDebug_CheckBox->isChecked())
    {
        m_pMyComBasic->SetShowDebug(true);
    }
    else
    {
        m_pMyComBasic->SetShowDebug(false);
    }
}

// �����Ƿ�׷�ӡ��س���
void CUpdateTab::EnterClicked()
{
    int iEnter = 0;
    if(true == m_pEnter_CheckBox->isChecked())
    {
        iEnter = 1;
    }

    // INI
    CFuncTool::SetEnterForSend(iEnter);
}

// �رմ���ʱ���õĺ���
void CUpdateTab::CloseFunction()
{
    // INI
    bool bWrited = true;
    QString strSend = m_pSendText_LineEdit->text();
    if((strSend.contains("open")) || (strSend.contains("debug"))) bWrited = false;
    if(true == bWrited) CFuncTool::SetComSendData(strSend.toLatin1());

    if(true == g_oPageCheck.UpdateTab_ExportFile_Use())
    {
        if(true == m_pAllowExport_CheckBox->isChecked())
        {
            CFuncTool::SetAllowExportFile(1);
        }
        else
        {
            CFuncTool::SetAllowExportFile(0);
        }

        if(true == m_pExportUseTime_CheckBox->isChecked())
        {
            CFuncTool::SetExportUseTime(1);
        }
        else
        {
            CFuncTool::SetExportUseTime(0);
        }
    }

    // �����˳�
    if(true == m_bComIsOpened) m_pMyComBasic->CloseComPort();
    if(true == m_bNetIsOpened) m_pMyComBasic->CloseNetPort();
    if((false == m_bComIsOpened) && (false == m_bNetIsOpened)) m_pMyComBasic->CloseComPort();

    // �ȴ��������
    volatile bool bThreadWorking = m_pMyComBasic->TaskIsRunning();
    while(true == bThreadWorking)
    {
        bThreadWorking = m_pMyComBasic->TaskIsRunning();
    }
}

// ���ļ�
void CUpdateTab::OpenFilePath()
{
    QFileDialog::Options options;
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                        tr("���ļ�"),
                                        tr(""),
                                        tr("Bin Files (*.bin)"),        // ;;Text Files(*.txt)
                                        &selectedFilter,
                                        options);
    if(!fileName.isEmpty())
    {
        // INI
        CFuncTool::SetDownloadFilePath(fileName.toLatin1());

        m_strFile = fileName;
        ShowTipInfoFunc(fileName.toLatin1());

        m_pFileShow_LineEdit->setText(GetShortFileOnPath(m_strFile.toLatin1()));
    }
}

// ��ȡ·�����ļ�����
QString CUpdateTab::GetShortFileOnPath(const char* pszPath)
{
    bool bFound = false;
    const char* ptrEnd = pszPath + strlen(pszPath);
    while(ptrEnd > pszPath)
    {
        if('/' == *ptrEnd)
        {
            bFound = true;
            break;
        }
        --ptrEnd;
    }

    if(true == bFound)
    {
        char chFile[1024] = {0};
        strcpy_s(chFile, sizeof(chFile), ptrEnd+1);
        QString strFile = chFile;
        return strFile;
    }
    else
    {
        return QString("");
    }
}

// �汾��ѯ
void CUpdateTab::VersionQuery()
{
    // ��������͡���ť���������������Ƿ���16����; ��������ఴť��ǿ������Ϊ16����
    SetFrameDataStyle(false, false, false, true);
    DestroyAllCmd();

    m_enOpType = en_Op_Is_Ver_One;

    // �޸ġ�ֹͣ����������
    m_pStopUpdate_Button->setText(tr("ֹͣ��ѯ"));

    // 1. "������Ԫ"��ȡ�����ĵ�ַ������ѯ 1 ��
    // 2. "ȫ��"��ȡ�̶��ĵ�ַ��ÿ����ַ��ѯ 1 �Σ��ܹ�Լ 15 ��
    // 3. �����������ַ�����Ϣ��ѯ���� 1 ��

    QString strText = m_pBoardSelect_ComboBox->currentText();
    if(m_pSelectAlone_Radio->isChecked()) strText = NAME_OF_VERSION_ONLYONE;
    if(NAME_OF_VERSION_ONLYONE == strText)              // "������Ԫ"
    {
        st_CommandItem* pCmdItem = new st_CommandItem(en_Cmd_A_Is_Version, en_Cmd_B_Ver_Is_One);
        AddOneCmd(pCmdItem);

        pCmdItem->m_chSourceAddr = GetSourceAddr();     // Դ��Ŀ�ĺ�����
        pCmdItem->m_chSourcePort = GetSourcePort();
        pCmdItem->m_chSourceUse = GetSourceUse();
        pCmdItem->m_chTargetAddr = GetTargetAddr();
        pCmdItem->m_chTargetPort = GetTargetPort();
        pCmdItem->m_chTargetUse = GetTargetUse();
        pCmdItem->m_chFrameCmd = COMMAND_IS_AT;
        pCmdItem->m_bManual = true;
    }
    else if(NAME_OF_VERSION_ALL == strText)             // "ȫ��"
    {
        DestroyAllBoardVerInfo();
        m_pBoardSelect_ComboBox->clear();
        m_pBoardSelect_ComboBox->setToolTip("");
        m_pBoardSelect_ComboBox->addItem(NAME_OF_VERSION_ALL);

        m_enOpType = en_Op_Is_Ver_More;
        for(int i = 0; i < COUNT_OF_AUTODOWN_CMD; ++i)
        {
            st_CommandItem* pCmdItem = new st_CommandItem(en_Cmd_A_Is_Version, en_Cmd_B_Ver_Is_More);
            AddOneCmd(pCmdItem);

            pCmdItem->m_chSourceAddr = GetSourceAddr();   // Դ��Ŀ�ĺ�����
            pCmdItem->m_chSourcePort = GetSourcePort();
            pCmdItem->m_chSourceUse = GetSourceUse();
            pCmdItem->m_chTargetAddr = g_BoardAddr[i][0];
            pCmdItem->m_chTargetPort = g_BoardAddr[i][1];
            pCmdItem->m_chTargetUse = GetTargetUse();
            pCmdItem->m_chFrameCmd = COMMAND_IS_AT;
            pCmdItem->m_bManual = true;
        }

        // ˢ������: ����������ͻ�ȡ����״̬������
        if(NULL != m_pMyImageTab)
        {
            m_pMyImageTab->CmdGetDeviceStatus();
        }
    }
    else
    {
        char chName[1024] = {0};
        if(false == GetBoardNameOnComBox(strText.toLatin1(), chName, sizeof(chName)))
        {
            ShowTipInfoFunc("��ȡ�忨���ƴ���");
            return;
        }

        st_BoardVerInfo* pVerInfo = m_BoardVerInfo_map[chName];
        if(NULL == pVerInfo)
        {
            ShowTipInfoFunc("��ȡ�忨��Ϣ�ṹ����");
            return;
        }
        
        st_CommandItem* pCmdItem = new st_CommandItem(en_Cmd_A_Is_Version, en_Cmd_B_Ver_Is_One);
        AddOneCmd(pCmdItem);

        pCmdItem->m_chSourceAddr = GetSourceAddr();     // Դ��Ŀ�ĺ�����
        pCmdItem->m_chSourcePort = GetSourcePort();
        pCmdItem->m_chSourceUse = GetSourceUse();
        pCmdItem->m_chTargetAddr = pVerInfo->m_chAddr;
        pCmdItem->m_chTargetPort = pVerInfo->m_chPort;
        pCmdItem->m_chTargetUse = pVerInfo->m_chUse;
        pCmdItem->m_chFrameCmd = COMMAND_IS_AT;
        pCmdItem->m_bManual = true;
    }

    GetFirstCmd();

    // ���͵�ǰ����ͷ��
    DownloadSendCurCmdFirstPack();
    m_iTimeOut = 0;
    m_bStopDownload = false;
    SetAllEnabled(false);

    ShowTipInfoFunc("���� ��ȡ�汾 ����");
}

// ��ʼ����
void CUpdateTab::StartUpdate()
{
    // 1. "������Ԫ"��ȡ�����ĵ�ַ��ȡ�ֶ��򿪵��ļ���ǿ������������ 1 ��
    // 2. "ȫ��"������
    // 3. ѡ��ĳ���忨�����ݸð忨����Ϣ�������汾��ͬ������������ 1��
    
    // �޸ġ�ֹͣ����������
    m_pStopUpdate_Button->setText(tr("ֹͣ����"));

    QString strText = m_pBoardSelect_ComboBox->currentText();
    if(m_pSelectAlone_Radio->isChecked()) strText = NAME_OF_VERSION_ONLYONE;
    if(NAME_OF_VERSION_ALL == strText)             // "ȫ��"
    {
        AutoUpdate();
        return;
    }

    // ��������͡���ť���������������Ƿ���16����; ��������ఴť��ǿ������Ϊ16����
    SetFrameDataStyle(false, true, true, true);
    DestroyAllCmd();

    m_enOpType = en_Op_Is_Update_One;
    
    if(NAME_OF_VERSION_ONLYONE == strText)              // "������Ԫ"
    {
        // ������ڲ����԰汾�������������ݣ�login(�س�)  123212(�س�)  inboot(�س�)
        SetFrameDataStyle(false, false, false, false);
        SetSourceAddr(GetSourceAddr(), GetSourcePort(), GetSourceUse());
        SetTargetAddr(GetTargetAddr(), GetTargetPort(), GetTargetUse());
        SetCommand(COMMAND_IS_AT);

        char chData[128] = {0};         // 0x03
        chData[0] = 0x03;
        chData[1] = 0x0D;
        chData[2] = 0x0A;
        chData[3] = '\0';
        SendComNetData(0, chData, 0);

        strcpy_s(chData, sizeof(chData), "login");
        int iLength = strlen(chData);
        chData[iLength] = 0x0D;
        chData[iLength+1] = 0x0A;
        chData[iLength+2] = '\0';
        SendComNetData(0, chData, 0);

        strcpy_s(chData, sizeof(chData), "123212");
        iLength = strlen(chData);
        chData[iLength] = 0x0D;
        chData[iLength+1] = 0x0A;
        chData[iLength+2] = '\0';
        SendComNetData(0, chData, 0);

        strcpy_s(chData, sizeof(chData), "inboot");
        iLength = strlen(chData);
        chData[iLength] = 0x0D;
        chData[iLength+1] = 0x0A;
        chData[iLength+2] = '\0';
        SendComNetData(0, chData, 0);

        SetFrameDataStyle(false, true, true, true); // �ָ�

        st_CommandItem* pCmdItem = new st_CommandItem(en_Cmd_A_Is_Update, en_Cmd_B_Down_Is_One);
        AddOneCmd(pCmdItem);

        pCmdItem->m_chSourceAddr = GetSourceAddr();     // Դ��Ŀ�ĺ�����
        pCmdItem->m_chSourcePort = GetSourcePort();
        pCmdItem->m_chSourceUse = GetSourceUse();
        pCmdItem->m_chTargetAddr = GetTargetAddr();
        pCmdItem->m_chTargetPort = GetTargetPort();
        pCmdItem->m_chTargetUse = GetTargetUse();
        pCmdItem->m_bManual = true;
        pCmdItem->m_strFilePath = m_strFile;            // ʹ���ֶ����ļ�
        errno_t err = fopen_s(&(pCmdItem->m_pFile), (pCmdItem->m_strFilePath).toLatin1(), "rb");
        if(0 != err)
        {
            pCmdItem->m_pFile = NULL;
            ShowTipInfoFunc("���ļ�ʧ��");
            return;
        }
    }
    else
    {
        char chName[1024] = {0};
        if(false == GetBoardNameOnComBox(strText.toLatin1(), chName, sizeof(chName)))
        {
            ShowTipInfoFunc("��ȡ�忨���ƴ���");
            return;
        }

        st_BoardVerInfo* pVerInfo = m_BoardVerInfo_map[chName];
        if(NULL == pVerInfo)
        {
            ShowTipInfoFunc("��ȡ�忨��Ϣ�ṹ����");
            return;
        }

        // 1. �����ļ��������ļ��ó��ļ����İ汾��Ϣ
        // 2. �汾��ͬ�����Ҳ����ļ�����ʾ�˳�
        float fVer_here = 0.0f;
        bool bRes = CFuncTool::GetVerOnDirAndShortName(m_strAutoPath.toLatin1(), pVerInfo->m_strOriName.c_str(), fVer_here);
        if((false == bRes))
        {
            ShowTipInfoFunc("�Ҳ����ļ�, �˳�");
            return;
        }
        else if((fVer_here == pVerInfo->m_fVer))
        {
            ShowTipInfoFunc("�汾��ͬ, �˳�");
            return;
        }
        else
        {
            st_CommandItem* pCmdItem = new st_CommandItem(en_Cmd_A_Is_Update, en_Cmd_B_Down_Is_One);
            

            // ʹ�ù̶��ļ� Ref_Input_Vx.y.bin
            sprintf_s(chName, sizeof(chName), "%s%s_V%0.1f.bin", m_strAutoPath.toLatin1().data(), pVerInfo->m_strOriName.c_str(), fVer_here);
            pCmdItem->m_strFilePath = chName;
            errno_t err = fopen_s(&(pCmdItem->m_pFile), (pCmdItem->m_strFilePath).toLatin1(), "rb");
            if(0 != err)
            {
                pCmdItem->m_pFile = NULL;
                ShowTipInfoFunc("���ļ�ʧ��, �˳�");
                delete pCmdItem;
                return;
            }
            
            AddOneCmd(pCmdItem);
            pCmdItem->m_chSourceAddr = GetSourceAddr();     // Դ��Ŀ�ĺ�����
            pCmdItem->m_chSourcePort = GetSourcePort();
            pCmdItem->m_chSourceUse = GetSourceUse();
            pCmdItem->m_chTargetAddr = pVerInfo->m_chAddr;
            pCmdItem->m_chTargetPort = pVerInfo->m_chPort;
            pCmdItem->m_chTargetUse = pVerInfo->m_chUse;
            pCmdItem->m_bManual = true;
        }
    }

    GetFirstCmd();

    // ���͵�ǰ����ͷ��
    DownloadSendCurCmdFirstPack();
    m_iTimeOut = 0;
    m_bStopDownload = false;
    SetAllEnabled(false);

    // ������־
    CmdLog_Func(GetTargetAddr(), GetTargetPort(), tr("Update One"));
}

// ֹͣ����(��ѯ)
void CUpdateTab::StopUpdate()
{
    if(NULL == m_pCmdItem_Cur) return;

    m_bStopDownload = true;

    // ˢ������: ������
    if(NULL != m_pMyImageTab)
    {
        m_pMyImageTab->CmdClear();
    }
}

// �Զ�����
void CUpdateTab::AutoUpdate()
{
    // ��Ҫ�Ȼ�ȡ�忨
    if(m_pBoardSelect_ComboBox->count() < 2)
    {
        ShowTipInfoFunc("���Ȳ�ѯ ȫ���忨");
        return;
    }

    // ��������͡���ť���������������Ƿ���16����; ��������ఴť��ǿ������Ϊ16����
    SetFrameDataStyle(false, true, true, true);
    DestroyAllCmd();
    SetAllEnabled(false);

    ShowTipInfoFunc("��ʼ �Զ�����...");
    m_enOpType = en_Op_Is_Auto_Download;

    // �޸ġ�ֹͣ����������
    m_pStopUpdate_Button->setText(tr("ֹͣ����"));

    // �� ����
    char* ptr = NULL;
    for(int i = 0; i < NUM_OF_AUTODOWN_REPORT; ++i)
    {
        ptr = m_pAutoDownloadReport[i];
        memset(ptr, 0, SIZE_OF_AUTODOWN_REPORT);
    }

    // ��� 15 �����ÿ��Ϊ�汾��ѯ+�������أ��緵�ذ汾�ͱ��������ļ��İ汾��һ�£���������
    // Ϊ�˱����û���������ѯȫ������˴���ͷ��ʼ��ѯ15��
    int iMaxCount = COUNT_OF_AUTODOWN_CMD;
    int iReport = 0;
    for(int i = 0; i < iMaxCount; ++i)
    {
        // ������忨û�в�ѯ��������
        if(false == BoardIsValid(g_BoardAddr[i][0], g_BoardAddr[i][1])) continue;

        // �汾
        st_CommandItem* pCmdItem = new st_CommandItem(en_Cmd_A_Is_Version, en_Cmd_B_Ver_Is_More);
        AddOneCmd(pCmdItem);

        pCmdItem->m_chSourceAddr = GetSourceAddr();   // Դ��Ŀ�ĺ�����
        pCmdItem->m_chSourcePort = GetSourcePort();
        pCmdItem->m_chSourceUse = GetSourceUse();
        pCmdItem->m_chTargetAddr = g_BoardAddr[i][0];
        pCmdItem->m_chTargetPort = g_BoardAddr[i][1];
        pCmdItem->m_chTargetUse = GetTargetUse();
        pCmdItem->m_chFrameCmd = COMMAND_IS_AT;
        pCmdItem->m_bManual = false;

        ptr = m_pAutoDownloadReport[iReport++];
        pCmdItem->m_pReportBuf = ptr;
        sprintf_s(ptr, SIZE_OF_AUTODOWN_REPORT, "��ѯ�汾: �忨��ַ[%02X, %02X](%s) ����Ӧ", 
                                                 pCmdItem->m_chTargetAddr, pCmdItem->m_chTargetPort,
                                                 GetBoardNameOnAddr(pCmdItem->m_chTargetAddr, pCmdItem->m_chTargetPort).toLatin1().data());

        // ��������
        pCmdItem = new st_CommandItem(en_Cmd_A_Is_Update, en_Cmd_B_Down_Is_More);
        AddOneCmd(pCmdItem);

        pCmdItem->m_chSourceAddr = GetSourceAddr();     // Դ��Ŀ�ĺ�����
        pCmdItem->m_chSourcePort = GetSourcePort();
        pCmdItem->m_chSourceUse = GetSourceUse();
        pCmdItem->m_chTargetAddr = g_BoardAddr[i][0];
        pCmdItem->m_chTargetPort = g_BoardAddr[i][1];
        pCmdItem->m_chTargetUse = GetTargetUse();
        pCmdItem->m_bManual = false;
        
        // ��ʱû���ļ������ɲ�ѯ����ó�
        pCmdItem->m_strFilePath = "";

        ptr = m_pAutoDownloadReport[iReport++];
        pCmdItem->m_pReportBuf = ptr;
        sprintf_s(ptr, SIZE_OF_AUTODOWN_REPORT, "�����ļ�: �忨��ַ[%02X, %02X](%s) ����Ӧ\n", 
                                                pCmdItem->m_chTargetAddr, pCmdItem->m_chTargetPort,
                                                GetBoardNameOnAddr(pCmdItem->m_chTargetAddr, pCmdItem->m_chTargetPort).toLatin1().data());

        ptr = m_pAutoUpdateSimpleReport[i];
        pCmdItem->m_pSimpleReportBuf = ptr;
        sprintf_s(ptr, SIZE_OF_AUTODOWN_REPORT, "�����ļ�: �忨��ַ[%02X, %02X](%s) ʧ��", 
                                                pCmdItem->m_chTargetAddr, pCmdItem->m_chTargetPort,
                                                GetBoardNameOnAddr(pCmdItem->m_chTargetAddr, pCmdItem->m_chTargetPort).toLatin1().data());

        // ��ʱ
        pCmdItem = new st_CommandItem(en_Cmd_A_Is_Delay, en_Cmd_B_Delay_Is_More);
        AddOneCmd(pCmdItem);

        pCmdItem->m_iDelayCount = 5;            // 5s
    }

    GetFirstCmd();

    // ���͵�ǰ����ͷ��
    DownloadSendCurCmdFirstPack();
    m_iTimeOut = 0;
    m_bStopDownload = false;

    // ������־
    CmdLog_Func(GetTargetAddr(), GetTargetPort(), tr("Update More"));
}

// ���͵�ǰ����ͷ��
void CUpdateTab::DownloadSendCurCmdFirstPack()
{
    if(NULL == m_pCmdItem_Cur)
    {
        ExitDownload();
        return;
    }

    if(en_Cmd_A_Is_Version == m_pCmdItem_Cur->m_enFrameAType)
    {
        // ����Դ��ַ
        m_pMyComBasic->SetSourceAddr(m_pCmdItem_Cur->m_chSourceAddr, m_pCmdItem_Cur->m_chSourcePort, m_pCmdItem_Cur->m_chSourceUse);

        // ����Ŀ�ĵ�ַ
        m_pMyComBasic->SetTargetAddr(m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort, m_pCmdItem_Cur->m_chTargetUse);

        // ����������
        int iFrameNum = GetFrameNo();
        m_pMyComBasic->SetCommand(iFrameNum, m_pCmdItem_Cur->m_chFrameCmd);

        FrameNoInc(iFrameNum);

        // ����
        SendOneFrame(m_pCmdItem_Cur->m_chFrameCmd, m_pCmdItem_Cur->m_chATCmd, sizeof(m_pCmdItem_Cur->m_chATCmd));

        // ��ʾ��Ϣ
        char chInfo[1024] = {0};
        sprintf_s(chInfo, sizeof(chInfo), "��ѯĿ��忨[%02X, %02X]�汾", m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort);
        ShowTipInfoFunc(chInfo);
    }
    else
    {
        // ���� ׼������ ����
        SendPrepareUpdate();

        // �Ƿ��Ǽ���
        m_bIsEncrypt = false;
        QString strShortName = GetShortFileOnPath((m_pCmdItem_Cur->m_strFilePath).toLatin1());
        if(strShortName.contains("_mi_", Qt::CaseInsensitive))
        {
            m_bIsEncrypt = true;
        }

        m_strFileNameUse = "";
        if(true == m_bUseFileName)
        {
            QString strShortName = GetShortFileOnPath((m_pCmdItem_Cur->m_strFilePath).toLatin1());
            QStringList listName = strShortName.split("_");
            m_strFileNameUse = listName[0];
            if(m_strFileNameUse.contains("Main", Qt::CaseInsensitive))
            {
                QString strUseSec = listName[1];
                if(strUseSec.contains("10.00M", Qt::CaseInsensitive))
                {
                    strUseSec = "P00";
                }
                else if(strUseSec.contains("10.23M", Qt::CaseInsensitive))
                {
                    strUseSec = "P23";
                }
                else
                {
                    strUseSec = "Ctrl";
                }
                m_strFileNameUse += strUseSec;
            }
        }
    }
}

// ���ð�ť״̬(��Զ˿��Ƿ��)
void CUpdateTab::SetFuncButtonEnable(bool bEnable)
{
    m_pSendData_Button->setEnabled(bEnable);
    m_pVerQuary_Button->setEnabled(bEnable);
    m_pStartUpdate_Button->setEnabled(bEnable);
    m_pStopUpdate_Button->setEnabled(bEnable);
    m_pAutoReSend_CheckBox->setEnabled(bEnable);

    if(false == m_bSomeFuncEnabled)
    {
        m_pAutoReSend_CheckBox->setEnabled(false);
    }

    if(false == bEnable)
    {
        StopAutoReSend();
    }

    // My Image Tab
    if(NULL != m_pMyImageTab)
    {
        m_pMyImageTab->SetFuncButtonEnable(bEnable);
    }
    if(NULL != m_pMyAdjustTab)
    {
        m_pMyAdjustTab->SetFuncButtonEnable(bEnable);
    }
    if(NULL != m_pGNSSTab)
    {
        m_pGNSSTab->SetFuncButtonEnable(bEnable);
    }
}

// ��ʾ�����ļ�������
void CUpdateTab::ShowUpdateFile()
{
    if(m_strFile.length() < 4)
    {
        ShowTipInfoFunc("�ļ�·������ȷ");
        return;
    }

    FILE* pFile = NULL;
    errno_t err = fopen_s(&pFile, m_strFile.toLatin1(), "rb");
    if(0 != err)
    {
        ShowTipInfoFunc("���ļ�ʧ��");
        return;
    }

    // ��ʾ
    int iRet = fseek(pFile, 0, SEEK_END);
    int iTotalSize = ftell(pFile);
    rewind(pFile);
    unsigned char chData[1024] = {0};
    int iReadSize = 256;
    int iMinSize = 0;

    QString strShow = "";
    int iShowLineCount = 0x0F;
    int iShowAllCount = 0;
    while(true)
    {
        iMinSize = qMin(iReadSize, iTotalSize);
        iRet = fread_s(chData, sizeof(chData), iMinSize, 1, pFile);
        if(iRet <= 0) break;
        iTotalSize -= iMinSize;

        // show
        QString strOne = "";
        unsigned char chOne = 0;
        unsigned char chOneBuf[16] = {0};
        for(int i = 0; i < iMinSize; ++i)
        {
            ++iShowLineCount;
            if(iShowLineCount > 0x0F) iShowLineCount = 0;
            if(0 == iShowLineCount)     // ��ͷ
            {
                Change16ToAscii8Bits(iShowAllCount, strOne);
                strShow += strOne;
            }

            chOne = chData[i];
            CFuncTool::Change16ToAscii(chOne, chOneBuf, sizeof(chOneBuf));
            strOne = (char*)chOneBuf;
            strShow += strOne;

            if(0x0F == iShowLineCount)  // ��β
            {
                strShow +="\n";
            }

            ++iShowAllCount;
        }
    }
    strShow = strShow.toUpper();
    ShowTipInfoFunc(strShow.toLatin1());
    fclose(pFile);
}

void CUpdateTab::Change16ToAscii8Bits(int iData, QString& strResult)
{
    strResult = QString::number(iData, 16);
    switch(strResult.length())
    {
    case 1:
        strResult = "0000000" + strResult;
        break;
    case 2:
        strResult = "000000" + strResult;
        break;
    case 3:
        strResult = "00000" + strResult;
        break;
    case 4:
        strResult = "0000" + strResult;
        break;
    case 5:
        strResult = "000" + strResult;
        break;
    case 6:
        strResult = "00" + strResult;
        break;
    case 7:
        strResult = "0" + strResult;
        break;
    default:
        break;
    }
    strResult = strResult + "h: ";
}

// �������س�ʱ����
void CUpdateTab::DownloadTimeoutFunc()
{
    ++m_iTimeOut;
    if(m_iTimeOut > m_iTimeOut_Save)
    {
        m_iTimeOut = 0;

        if(NULL != m_pCmdItem_Cur)          // ��ǰ���ʱ
        {
            m_pCmdItem_Cur->m_bTimeOut = true;
            HandleFrameData();
            return;
        }
    }
}

// ֡���ݴ�����
void CUpdateTab::FrameHanleFunc(const st_FrameData* pFrameData)
{
    // ���ݴ���ȫ�ֱ���1
    ((st_FrameData* )pFrameData)->Clone(&g_FrameData_1);
    HandleFrameData();
}

// ֡���ݴ����̵߳��ú���
void CUpdateTab::HandleFrameData()
{
    // 1. �Ƿ��ǳ�ʱ
    // 2. �����Ƿ���Ч
    // 3. ��ǰ������ʲô����
    // 4. �Ƿ����ֶ�
    // 5. �Ƿ�ǿ��ֹͣ
    // 6. ע�⣺��Ϊ�п����յ��Ĳ���ϣ�������ݣ����Ը�����һ������������ǣ�1���յ���ȷ����Ӧ��2����ʱ

    if(true == m_bStopDownload)
    {
        ExitDownload();
        CmdFinished_Func();
        m_pCmdItem_Cur = NULL;
        m_bStopDownload = false;
        m_iTimeOut = 0;
        ShowTipInfoFunc("�û�ǿ��ֹͣ�˵�ǰ����");
        return;
    }

    // ��ȫ�ֱ���1�������ݵ�ȫ�ֱ���2
    g_FrameData_1.Clone(&g_FrameData_2);
    m_iMultiFrame = g_FrameData_2.m_chTargetUse;        // ��ָ֡ʾ

    // GNSS �����
    if(COMMAND_IS_XXB2 == g_FrameData_2.m_chFrameCmd)
    {
        if(NULL != m_pGNSSTab)
        {
            m_pGNSSTab->HandleFrameData(g_FrameData_2.m_chFrameCmd, g_FrameData_2.m_pFrameDataBuf, 
                                g_FrameData_2.m_iFrameDataLength, g_FrameData_2.m_chSourceAddr, g_FrameData_2.m_chSourcePort);
        }
        return;
    }

    if(NULL != m_pCmdItem_Cur)              // �е�ǰ����
    {
        bool bCmdFinish = false;            // �����Ƿ��Ѿ�ִ�����
        if(en_Cmd_A_Is_Version == m_pCmdItem_Cur->m_enFrameAType)   // �汾��ѯ(��ǰ)
        {
            if(en_Cmd_B_Ver_Is_One == m_pCmdItem_Cur->m_enFrameBType)
            {
                if(true == m_pCmdItem_Cur->m_bTimeOut)                  // ��ʱ
                {
                    m_pCmdItem_Cur->m_bTimeOut = false;

                    if(m_pCmdItem_Cur->m_iReDownCount >= m_iReDownCount_Save)
                    {
                        bCmdFinish = true;
                        m_pCmdItem_Cur->m_iReDownCount = 0;
                        ShowTipInfoFunc("�ط����");
                    }
                    else
                    {
                        char chTip[1024] = {0};
                        sprintf_s(chTip, sizeof(chTip), "MCU ��Ӧ��ʱ���ط�����[%d]", m_pCmdItem_Cur->m_iReDownCount+1);
                        ShowTipInfoFunc(chTip);

                        SendOneFrame(0, NULL, 0, true);     // �ط���֡
                    }
                }
                else
                {
                    if(false == g_FrameData_2.m_bDataIsOK)              // ���ݲ���ȷ
                    {
                    }
                    else
                    {
                        char chName[1024] = {0};
                        float fVer = 0.0f;
                        bool bGet = GetBoardNameAndVerOnInput(g_FrameData_2.m_pFrameDataBuf, chName, sizeof(chName), fVer);
                        if(true == m_pCmdItem_Cur->m_bManual)           // �ֶ�
                        {
                            if(true == bGet)
                            {
                                m_iTimeOut = 0;
                                bCmdFinish = true;
                            }
                        }
                        else                                            // �Զ�
                        {
                            if(true == bGet)
                            {
                                m_iTimeOut = 0;
                                bCmdFinish = true;
                            }
                        }
                    }
                }
            }
            else if(en_Cmd_B_Ver_Is_More == m_pCmdItem_Cur->m_enFrameBType)
            {
                if(true == m_pCmdItem_Cur->m_bTimeOut)                  // ��ʱ
                {
                    m_pCmdItem_Cur->m_bTimeOut = false;
                    if(m_pCmdItem_Cur->m_iReDownCount >= m_iReDownCount_Save)
                    {
                        bCmdFinish = true;
                        m_pCmdItem_Cur->m_iReDownCount = 0;
                        ShowTipInfoFunc("�ط����");

                        if(false == m_pCmdItem_Cur->m_bManual)              // �Զ�
                        {
                            // Ӧ�����Զ�����������2��������������һ��
                            GetNextCmd();
                            GetNextCmd();
                            ShowTipInfoFunc("��ѯ�汾��ʱ");
                        }
                    }
                    else
                    {
                        char chTip[1024] = {0};
                        sprintf_s(chTip, sizeof(chTip), "MCU ��Ӧ��ʱ���ط�����[%d]", m_pCmdItem_Cur->m_iReDownCount+1);
                        ShowTipInfoFunc(chTip);

                        SendOneFrame(0, NULL, 0, true);     // �ط���֡
                    }
                }
                else
                {
                    if(false == g_FrameData_2.m_bDataIsOK)              // ���ݲ���ȷ
                    {
                    }
                    else
                    {
                        char chName[1024] = {0};
                        float fVer = 0.0f;
                        bool bGet = GetBoardNameAndVerOnInput(g_FrameData_2.m_pFrameDataBuf, chName, sizeof(chName), fVer);
                        if(true == m_pCmdItem_Cur->m_bManual)           // �ֶ�
                        {
                            if(true == bGet)
                            {
                                AddOneBoard(chName, sizeof(chName), g_FrameData_2.m_chSourceAddr, g_FrameData_2.m_chSourcePort, fVer);
                                bCmdFinish = true;          // ��Ӧ��ȷ�����������ϣ�������һ������
                                m_iTimeOut = 0;
                            }
                        }
                        else                                            // �Զ�(Ӧ�����Զ�����)
                        {
                            if(true == bGet)
                            {
                                // ʹ�ù̶��ļ� Ref_Input_Vx.y.bin��ע�⣺�汾��ͬ������
                                bCmdFinish = true;          // ��Ӧ��ȷ�����������ϣ�������һ������
                                m_iTimeOut = 0;
                                float fVer_here = 0.0f;
                                st_CommandItem* pCmdItemHereUse = m_pCmdItem_Cur;
                                bool bRes = CFuncTool::GetVerOnDirAndShortName(m_strAutoPath.toLatin1(), chName, fVer_here);
                                if((false == bRes) || (fVer_here == fVer))
                                {
                                    // ����ֻ������2����������������һ��
                                    GetNextCmd();
                                    GetNextCmd();
                                    if((false == bRes))
                                    {
                                        char chFilePath[1024] = {0};
                                        sprintf_s(chFilePath, sizeof(chFilePath), "�����ļ�ʧ��: %s%s_V%0.1f.bin", m_strAutoPath.toLatin1().data(), chName, fVer_here);
                                        ShowTipInfoFunc(chFilePath);

                                        // ����
                                        char* ptr = pCmdItemHereUse->m_pReportBuf;
                                        if((en_Op_Is_Auto_Download == m_enOpType) && (ptr != NULL))
                                        {
                                            sprintf_s(ptr, SIZE_OF_AUTODOWN_REPORT, "��ѯ�汾: �忨��ַ[%02X, %02X](%s), %s", 
                                                      pCmdItemHereUse->m_chTargetAddr, pCmdItemHereUse->m_chTargetPort, 
                                                      GetBoardNameOnAddr(pCmdItemHereUse->m_chTargetAddr, pCmdItemHereUse->m_chTargetPort).toLatin1().data(),
                                                      chFilePath);
                                        }
                                    }
                                    else if((fVer_here == fVer))
                                    {
                                        ShowTipInfoFunc("�汾��ͬ");

                                        // ����
                                        char* ptr = pCmdItemHereUse->m_pReportBuf;
                                        if((en_Op_Is_Auto_Download == m_enOpType) && (ptr != NULL))
                                        {
                                            sprintf_s(ptr, SIZE_OF_AUTODOWN_REPORT, "��ѯ�汾: �忨��ַ[%02X, %02X](%s), �汾��ͬ[%0.1f], ������", 
                                                        pCmdItemHereUse->m_chTargetAddr, pCmdItemHereUse->m_chTargetPort, 
                                                        GetBoardNameOnAddr(pCmdItemHereUse->m_chTargetAddr, pCmdItemHereUse->m_chTargetPort).toLatin1().data(),
                                                        fVer);
                                        }
                                    }

                                    // ���Ա���
                                    pCmdItemHereUse = pCmdItemHereUse->m_next;
                                    if((NULL != pCmdItemHereUse) && (en_Cmd_A_Is_Update == pCmdItemHereUse->m_enFrameAType))
                                    {
                                        char* ptr = pCmdItemHereUse->m_pSimpleReportBuf;
                                        if(NULL != ptr)
                                        {
                                            sprintf_s(ptr, SIZE_OF_AUTODOWN_REPORT, "��ѯ�汾: �忨��ַ[%02X, %02X](%s), ��ѯʧ��", 
                                                                pCmdItemHereUse->m_chTargetAddr, pCmdItemHereUse->m_chTargetPort,
                                                                GetBoardNameOnAddr(pCmdItemHereUse->m_chTargetAddr, pCmdItemHereUse->m_chTargetPort).toLatin1().data());
                                        }
                                    }
                                }
                                else
                                {
                                    // ����ѯ���Ľ�����浽��һ��������ļ�·��
                                    st_CommandItem* pCmdItemHere = m_pCmdItem_Cur;
                                    if(NULL != pCmdItemHere)
                                    {
                                        pCmdItemHere = pCmdItemHere->m_next;
                                        if((NULL != pCmdItemHere) && (en_Cmd_A_Is_Update == pCmdItemHere->m_enFrameAType))
                                        {
                                            char chFilePath[1024] = {0};
                                            sprintf_s(chFilePath, sizeof(chFilePath), "%s%s_V%0.1f.bin", m_strAutoPath.toLatin1().data(), chName, fVer_here);
                                            pCmdItemHere->m_strFilePath = chFilePath;

                                            // ����
                                            char* ptr = m_pCmdItem_Cur->m_pReportBuf;
                                            if((en_Op_Is_Auto_Download == m_enOpType) && (ptr != NULL))
                                            {
                                                sprintf_s(ptr, SIZE_OF_AUTODOWN_REPORT, "��ѯ�汾: �忨��ַ[%02X, %02X](%s), �汾[%0.1f], �ļ�[%s], ��ѯ�ɹ�", 
                                                            m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort, 
                                                            GetBoardNameOnAddr(m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort).toLatin1().data(),
                                                            fVer, chFilePath);
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                            }
                        }
                    }
                }
            }
            else if(en_Cmd_B_Send_One_Cmd == m_pCmdItem_Cur->m_enFrameBType)
            {
                if(true == m_pCmdItem_Cur->m_bTimeOut)                  // ��ʱ
                {
                    m_pCmdItem_Cur->m_bTimeOut = false;

                    if(m_pCmdItem_Cur->m_iReDownCount >= m_iReDownCount_Save)
                    {
                        bCmdFinish = true;
                        m_pCmdItem_Cur->m_iReDownCount = 0;
                        ShowTipInfoFunc("�ط����");
                    }
                    else
                    {
                        char chTip[1024] = {0};
                        sprintf_s(chTip, sizeof(chTip), "MCU ��Ӧ��ʱ���ط�����[%d]", m_pCmdItem_Cur->m_iReDownCount+1);
                        ShowTipInfoFunc(chTip);

                        SendOneFrame(0, NULL, 0, true);     // �ط���֡
                    }
                }
                else
                {
                    if(false == g_FrameData_2.m_bDataIsOK)              // ���ݲ���ȷ
                    {
                    }
                    else
                    {
                        CheckSendedOneCmd(g_FrameData_2.m_pFrameDataBuf, m_iMultiFrame);
                        m_iTimeOut = 0;
                        bCmdFinish = true;
                    }
                }
            }
        }
        else if(en_Cmd_A_Is_Update == m_pCmdItem_Cur->m_enFrameAType)   // ��������(��ǰ)
        {
            if(en_Cmd_B_Down_Is_One == m_pCmdItem_Cur->m_enFrameBType)
            {
                if(true == m_pCmdItem_Cur->m_bTimeOut)                  // ��ʱ
                {
                    m_pCmdItem_Cur->m_bTimeOut = false;

                    if(true == m_bSingleDebug)
                    {
                        m_iSingleNum = 2;
                        if(0 == m_pCmdItem_Cur->m_iReDownCount)
                        {
                            ++m_pCmdItem_Cur->m_iReDownCount;
                            ShowTipInfoFunc("��ʱ�������ط���֡");
                        }
                    }
                    else
                    {
                        if(m_pCmdItem_Cur->m_iReDownCount >= m_iReDownCount_Save)
                        {
                            m_pCmdItem_Cur->m_iReDownCount = 0;
                            ExitDownload();
                            ShowTipInfoFunc("�ط���ϣ��˳�����");
                        }
                        else
                        {
                            char chTip[1024] = {0};
                            sprintf_s(chTip, sizeof(chTip), "MCU ��Ӧ��ʱ���ط���֡[%d], �ط�����[%d]", m_pCmdItem_Cur->m_iPaksIndex+1, m_pCmdItem_Cur->m_iReDownCount+1);
                            ShowTipInfoFunc(chTip);

                            SendOneFrame(0, NULL, 0, true);     // �ط���֡
                        }
                    }
                }
                else
                {
                    if(false == g_FrameData_2.m_bDataIsOK)              // ���ݲ���ȷ
                    {
                        //ShowTipInfoFunc("���ݲ���ȷ");
                        //return;
                    }
                    else
                    {
                        unsigned char chCommand = g_FrameData_2.m_chFrameCmd;
                        unsigned char chHighPak = (g_FrameData_2.m_pFrameDataBuf)[0];     // �ظ��İ���
                        unsigned char chLowPak = (g_FrameData_2.m_pFrameDataBuf)[1];
                        unsigned char chParam = (g_FrameData_2.m_pFrameDataBuf)[2];       // �ظ��Ĳ���

                        if(true == m_pCmdItem_Cur->m_bManual)           // �ֶ�
                        {
                            HandleRecvCommand(chCommand, chHighPak, chLowPak, chParam);
                        }
                        else                                            // �Զ�
                        {
                            ;
                        }
                    }
                }

                // ֻ����һ��������������һ������Ϊ�գ��������������
                if(NULL == m_pCmdItem_Cur)
                {
                    // ��������͡���ť���������������Ƿ���16����; ��������ఴť��ǿ������Ϊ16����(�ָ�����)
                    SetFrameDataStyle(true, true, true, true);
                    CmdFinished_Func();
                }
            }
            else if(en_Cmd_B_Down_Is_More == m_pCmdItem_Cur->m_enFrameBType)
            {
                if(true == m_pCmdItem_Cur->m_bTimeOut)                  // ��ʱ
                {
                    m_pCmdItem_Cur->m_bTimeOut = false;

                    if(m_pCmdItem_Cur->m_iReDownCount >= m_iReDownCount_Save)
                    {
                        m_pCmdItem_Cur->m_iReDownCount = 0;
                        bCmdFinish = true;
                        GetNextCmd();
                    }
                    else
                    {
                        char chTip[1024] = {0};
                        sprintf_s(chTip, sizeof(chTip), "MCU ��Ӧ��ʱ���ط���֡[%d], �ط�����[%d]", m_pCmdItem_Cur->m_iPaksIndex+1, m_pCmdItem_Cur->m_iReDownCount+1);
                        ShowTipInfoFunc(chTip);

                        SendOneFrame(0, NULL, 0, true);     // �ط���֡
                    }
                }
                else
                {
                    if(false == g_FrameData_2.m_bDataIsOK)              // ���ݲ���ȷ
                    {
                        //ShowTipInfoFunc("���ݲ���ȷ");
                        
                    }
                    else
                    {
                        unsigned char chCommand = g_FrameData_2.m_chFrameCmd;
                        unsigned char chHighPak = (g_FrameData_2.m_pFrameDataBuf)[0];     // �ظ��İ���
                        unsigned char chLowPak = (g_FrameData_2.m_pFrameDataBuf)[1];
                        unsigned char chParam = (g_FrameData_2.m_pFrameDataBuf)[2];       // �ظ��Ĳ���

                        if(true == m_pCmdItem_Cur->m_bManual)           // �ֶ�
                        {
                            ;
                        }
                        else                                            // �Զ�
                        {
                            HandleRecvCommand(chCommand, chHighPak, chLowPak, chParam);
                            if(0x75 == chCommand)      // �ظ� ��������
                            {
                                m_pCmdItem_Cur->ExitDownload();         // �ر��ļ�
                                bCmdFinish = true;

                                // ����
                                char* ptr = m_pCmdItem_Cur->m_pReportBuf;
                                if((en_Op_Is_Auto_Download == m_enOpType) && (ptr != NULL))
                                {
                                    sprintf_s(ptr, SIZE_OF_AUTODOWN_REPORT, "�����ļ�: �忨��ַ[%02X, %02X](%s), �����ɹ�\n", 
                                        m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort,
                                        GetBoardNameOnAddr(m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort).toLatin1().data());
                                }

                                // ���Ա���
                                ptr = m_pCmdItem_Cur->m_pSimpleReportBuf;
                                if(NULL != ptr)
                                {
                                    sprintf_s(ptr, SIZE_OF_AUTODOWN_REPORT, "�����ļ�: �忨��ַ[%02X, %02X](%s), �����ɹ�", 
                                                        m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort,
                                                        GetBoardNameOnAddr(m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort).toLatin1().data());
                                }
                            }
                        }
                    }
                }
            }
        }
        else if(en_Cmd_A_Is_Delay == m_pCmdItem_Cur->m_enFrameAType)        // ��ʱ(��ǰ)
        {
            if(en_Cmd_B_Delay_Is_One == m_pCmdItem_Cur->m_enFrameBType)
            {
                ;
            }
            else if(en_Cmd_B_Delay_Is_More == m_pCmdItem_Cur->m_enFrameBType)
            {
                if(true == m_pCmdItem_Cur->m_bTimeOut)                  // ��ʱ
                {
                    m_pCmdItem_Cur->m_bTimeOut = false;
                    m_pCmdItem_Cur->m_iDelayCount = m_pCmdItem_Cur->m_iDelayCount - 1;
                    if(m_pCmdItem_Cur->m_iDelayCount <= 0)
                    {
                        bCmdFinish = true;
                    }
                }
            }
        }

        if(true == bCmdFinish)  // �����ǰ�����Ѿ�������ϣ��������һ��
        {
            if((en_Cmd_A_Is_Version == m_pCmdItem_Cur->m_enFrameAType) && 
               (en_Cmd_B_Ver_Is_More == m_pCmdItem_Cur->m_enFrameBType) &&
               (true == m_pCmdItem_Cur->m_bManual))
            {
                // ������е�����ѽ����������ؼ�
                if(NULL == m_pCmdItem_Cur->m_next) BoardSelectFill();
            }

            GetNextCmd();
            if(NULL != m_pCmdItem_Cur)
            {
                m_iTimeOut = 0;
                if(en_Cmd_A_Is_Version == m_pCmdItem_Cur->m_enFrameAType)  // �汾��ѯ(next)
                {
                    // ����Դ��ַ
                    m_pMyComBasic->SetSourceAddr(m_pCmdItem_Cur->m_chSourceAddr, m_pCmdItem_Cur->m_chSourcePort, m_pCmdItem_Cur->m_chSourceUse);

                    // ����Ŀ�ĵ�ַ
                    m_pMyComBasic->SetTargetAddr(m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort, m_pCmdItem_Cur->m_chTargetUse);

                    // ����������
                    int iFrameNum = GetFrameNo();
                    m_pMyComBasic->SetCommand(iFrameNum, m_pCmdItem_Cur->m_chFrameCmd);

                    FrameNoInc(iFrameNum);

                    // ����
                    SendOneFrame(m_pCmdItem_Cur->m_chFrameCmd, m_pCmdItem_Cur->m_chATCmd, sizeof(m_pCmdItem_Cur->m_chATCmd));

                    ShowTipInfoFunc("������һ���汾��ѯ����");

                    // ��ʾ��Ϣ
                    char chInfo[1024] = {0};
                    sprintf_s(chInfo, sizeof(chInfo), "��ѯĿ��忨[%02X, %02X]�汾", m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort);
                    ShowTipInfoFunc(chInfo);
                }
                else if(en_Cmd_A_Is_Update == m_pCmdItem_Cur->m_enFrameAType)   // ��������(next)
                {
                    errno_t err = fopen_s(&(m_pCmdItem_Cur->m_pFile), (m_pCmdItem_Cur->m_strFilePath).toLatin1(), "rb");
                    if(0 != err)
                    {
                        m_pCmdItem_Cur->m_pFile = NULL;
                        ShowTipInfoFunc("���ļ�ʧ��");
                    }
                    else
                    {
                        // ���͵�ǰ����ͷ��
                        DownloadSendCurCmdFirstPack();
                    }
                }
                else if(en_Cmd_A_Is_Delay == m_pCmdItem_Cur->m_enFrameAType)    // ��ʱ(next)
                {
                    ShowTipInfoFunc("�ȴ� 5 ��");
                }
            }
            else            // ����
            {
                ShowTipInfoFunc("��������");
                CmdFinished_Func();
                ExitDownload();

                // ��������͡���ť���������������Ƿ���16����; ��������ఴť��ǿ������Ϊ16����(�ָ�����)
                SetFrameDataStyle(true, true, true, true);

                // ������Զ����أ���ʾ����
                if(en_Op_Is_Auto_Download == m_enOpType)
                {
                    ShowTipInfoFunc("\n�Զ���������: ");     // ��ϸ
                    char* ptr = NULL;
                    for(int i = 0; i < NUM_OF_AUTODOWN_REPORT; ++i)
                    {
                        ptr = m_pAutoDownloadReport[i];
                        ShowTipInfoFunc(ptr, QColor(0, 0, 250));
                    }

                    // ��ʱ��ʹ�ü��Ա���
                    /*ShowTipInfoFunc("�Զ��������Ա���: ");
                    for(int i = 0; i < COUNT_OF_AUTODOWN_CMD; ++i)
                    {
                        ptr = m_pAutoUpdateSimpleReport[i];
                        ShowTipInfoFunc(ptr, QColor(28, 147, 148));
                    }*/
                    ShowTipInfoFunc(" \n");
                }

                // ��ʾ���ȴ��豸���� 8 �룬���µ�����忨��ѯ����ȡ��ȫ���忨�������°汾
                if((en_Op_Is_Update_One == m_enOpType) || (en_Op_Is_Auto_Download == m_enOpType))
                {
                    ShowTipInfoFunc("\n�����������忨������������Ⱥ� 8 ��\n", QColor(0, 0, 250));
                    ShowTipInfoFunc("8 �����ѡ��ȫ���忨����������忨��ѯ�����°忨�İ汾��\n", QColor(0, 0, 250));
                }

                m_enOpType = en_Op_Is_NULL;     // ��
            }
        }
    }
    else                    // �޵�ǰ����
    {
        if(NULL != m_pMyImageTab)
        {
            m_pMyImageTab->HandleFrameData(g_FrameData_2.m_chFrameCmd, g_FrameData_2.m_pFrameDataBuf, 
                                g_FrameData_2.m_iFrameDataLength, g_FrameData_2.m_chSourceAddr, g_FrameData_2.m_chSourcePort);
        }
        if(NULL != m_pStatusTab)
        {
            m_pStatusTab->HandleFrameData(g_FrameData_2.m_chFrameCmd, g_FrameData_2.m_pFrameDataBuf, 
                                g_FrameData_2.m_iFrameDataLength, g_FrameData_2.m_chSourceAddr, g_FrameData_2.m_chSourcePort);
        }
        if(NULL != m_pMyAdjustTab)
        {
            m_pMyAdjustTab->HandleFrameData(g_FrameData_2.m_chFrameCmd, g_FrameData_2.m_pFrameDataBuf, 
                                g_FrameData_2.m_iFrameDataLength, g_FrameData_2.m_chSourceAddr, g_FrameData_2.m_chSourcePort);
        }

        // �������ж�֡����
        if(m_iMultiFrame > 1)
        {
            CheckSendedOneCmd(g_FrameData_2.m_pFrameDataBuf, m_iMultiFrame);
        }

    }
}

// �ط���֡
void CUpdateTab::ReDownloadFunc(QString strError)
{
    if(true == m_bSingleDebug)          // ����
    {
        ShowTipInfoFunc("���ط�");
        m_iSingleNum = 2;
        return;
    }

    {
        if(m_pCmdItem_Cur->m_iReDownCount >= m_iReDownCount_Save)
        {
            m_pCmdItem_Cur->m_iReDownCount = 0;
            ExitDownload();
            ShowTipInfoFunc("�ط���ϣ��˳�����");
        }
        else
        {
            char chTip[1024] = {0};
            sprintf_s(chTip, sizeof(chTip), "%s �ط���֡[%d], �ط�����[%d]", strError.toLatin1().data(),m_pCmdItem_Cur->m_iPaksIndex+1, m_pCmdItem_Cur->m_iReDownCount+1);
            ShowTipInfoFunc(chTip);

            SendOneFrame(0, NULL, 0, true); // �ط���֡
        }
    }
}

// �����ص�����
void CUpdateTab::HandleRecvCommand(unsigned char chCommand, unsigned char chHighPak, unsigned char chLowPak, unsigned char chParam)
{
    static int stiOKRetPak = 0;     // �Ѿ����������ȷ�İ���

    int iRetPak = chHighPak * 256 + chLowPak;

    if(0x71 == chCommand)           // �ظ� ׼������
    {
        // ��λ���п����ظ��ϴ��������
        if(0x70 != m_pCmdItem_Cur->m_chCurCommand)
        {
            return;
        }

        stiOKRetPak = -1;
        m_iTimeOut = 0;

        if(iRetPak != m_pCmdItem_Cur->m_iTotalPaks) // �ܰ�������
        {
            ReDownloadFunc(tr("MCU �ظ��ܰ��� ����"));           // �ط���֡
            return;
        }

        if(0x00 == chParam)         // ����Ӧ
        {
            ReDownloadFunc(tr("MCU ����Ӧ ׼���������"));           // �ط���֡
        }
        else if(0x01 == chParam)    // ��Ӧ����
        {
            char chTip[1024] = {0};
            if((true == m_bIsEncrypt)) 
            {
                sprintf_s(chTip, sizeof(chTip), "MCU ��Ӧ ׼����������, ���ͼ�����������");
            }
            else if(true == m_bUseFileName)
            {
                sprintf_s(chTip, sizeof(chTip), "MCU ��Ӧ ׼����������, �����ļ�����������");
            }
            else 
            {
                sprintf_s(chTip, sizeof(chTip), "MCU ��Ӧ ׼����������, ���ص�һ��[%d/%d]", m_pCmdItem_Cur->m_iPaksIndex+1, m_pCmdItem_Cur->m_iTotalPaks+1);
            }
            ShowTipInfoFunc(chTip);

            if(false == m_bSingleDebug)
            {
                // ��ʼ����
                SendNextOnePak();
            }
            else                    // ��������
            {
                m_iSingleNum = 1;
            }
            m_pCmdItem_Cur->m_iReDownCount = 0;
        }
        else if(0x02 == chParam)    // �ļ�����忨��һ��
        {
            ExitDownload();
            ShowTipInfoFunc("�������ļ����뵥�岻һ�£��˳��������ء���ȷ���ļ����Ƿ���ȷ");
        }
        else
        {
            ReDownloadFunc(tr("MCU ��Ӧ ׼����������: δ֪������"));           // �ط���֡
        }
    }
    else if(0x77 == chCommand)      // �ǡ����ܡ��ظ�
    {
        // ��λ���п����ظ��ϴ��������
        if(0x76 != m_pCmdItem_Cur->m_chCurCommand)
        {
            return;
        }

        stiOKRetPak = -1;
        m_iTimeOut = 0;

        if(0x00 == chParam)         // ����Ӧ
        {
            ReDownloadFunc(tr("MCU ����Ӧ �����������"));           // �ط���֡
        }
        else if(0x01 == chParam)    // ��Ӧ����
        {
            char chTip[1024] = {0};
            if(false == m_bUseFileName) sprintf_s(chTip, sizeof(chTip), "MCU ��Ӧ ������������, ���ص�һ��[%d/%d]", m_pCmdItem_Cur->m_iPaksIndex+1, m_pCmdItem_Cur->m_iTotalPaks+1);
            else sprintf_s(chTip, sizeof(chTip), "MCU ��Ӧ ������������, �·��ļ���[%s]", m_strFileNameUse.toLatin1().data());
            ShowTipInfoFunc(chTip);

            // ��ʼ����
            SendNextOnePak();
            m_pCmdItem_Cur->m_iReDownCount = 0;
        }
        else
        {
            ReDownloadFunc(tr("MCU ��Ӧ ������������: δ֪����[%1]��").arg(QString::number(chParam)));           // �ط���֡
        }
    }
    else if(0x79 == chCommand)      // �ǡ��ļ������ظ�
    {
        // ��λ���п����ظ��ϴ��������
        if(0x78 != m_pCmdItem_Cur->m_chCurCommand)
        {
            return;
        }

        stiOKRetPak = -1;
        m_iTimeOut = 0;

        if(0x00 == chParam)         // ����Ӧ
        {
            ReDownloadFunc(tr("MCU ����Ӧ �ļ��������������ѡ���������ļ���"));           // �ط���֡
        }
        else if(0x01 == chParam)    // ��Ӧ����
        {
            char chTip[1024] = {0};
            sprintf_s(chTip, sizeof(chTip), "MCU ��Ӧ ������������, ���ص�һ��[%d/%d]", m_pCmdItem_Cur->m_iPaksIndex+1, m_pCmdItem_Cur->m_iTotalPaks+1);
            ShowTipInfoFunc(chTip);

            if(false == m_bSingleDebug)
            {
                // ��ʼ����
                SendNextOnePak();
            }
            else                    // ��������
            {
                m_iSingleNum = 1;
            }
            m_pCmdItem_Cur->m_iReDownCount = 0;
        }
        else
        {
            ReDownloadFunc(tr("MCU ��Ӧ ������������: δ֪����[%1]��").arg(QString::number(chParam)));           // �ط���֡
        }
    }
    else if(0x73 == chCommand)      // �ظ� �����·�
    {
        // ��λ���п����ظ��ϴ��������
        if(0x72 != m_pCmdItem_Cur->m_chCurCommand)
        {
            return;
        }

        m_iTimeOut = 0;

        if(iRetPak != m_pCmdItem_Cur->m_iPaksIndex) // ���Ų���
        {
            // ������Ѿ����չ��İ��ţ����ط�
            if(iRetPak == stiOKRetPak) return;

            ReDownloadFunc(tr("MCU �ظ����� ����"));           // �ط���֡
            return;
        }

        if(0x00 == chParam)         // ����Ӧ��д��ʧ�ܣ���λ��Ҫ�ط���֡���ݣ������һ��ʱ��ʲô���ݶ�û�յ�����λ��ҲҪ�ط���֡
        {
            // new : (�˳�����ʾ���¿�ʼ����)
            ExitDownload();
            ShowTipInfoFunc("MCU ��Ӧ�쳣���˳��������ء������¿�ʼ��������");
        }
        else if((0x01 == chParam) || (0x04 == chParam))    // ��ȷ��Ӧ����  // ��֡���ݼ��ɹ�д�룬�뷢��һ֡
        {
            stiOKRetPak = iRetPak;          // ������ȷ��Ӧ�İ���
            char chTip[1024] = {0};
            sprintf_s(chTip, sizeof(chTip), "MCU ��Ӧ �����·�: ��ȷ��Ӧ���������һ��[%d/%d]", m_pCmdItem_Cur->m_iPaksIndex+1, m_pCmdItem_Cur->m_iTotalPaks+1);
            ShowTipInfoFunc(chTip);
            if(false == m_bSingleDebug)
            {
                // ���� ��һ��
                SendNextOnePak();
            }
            else            // ��������
            {
                m_iSingleNum = 3;
            }
            m_pCmdItem_Cur->m_iReDownCount = 0;
        }
        else if(0x02 == chParam)    // ���Ų���ȷ
        {
            ReDownloadFunc(tr("MCU ��Ӧ �����·�: ���Ų���ȷ��"));           // �ط���֡
        }
        else if(0x03 == chParam)    // д��FLASHʧ��
        {
            ReDownloadFunc(tr("MCU ��Ӧ �����·�: д�� FLASH ʧ�ܡ�"));           // �ط���֡
        }
        else
        {
            ReDownloadFunc(tr("MCU ��Ӧ �����·�: δ֪������"));           // �ط���֡
        }
    }
    else if(0x75 == chCommand)      // �ظ� ��������
    {
        // ��λ���п����ظ��ϴ��������
        if(0x74 != m_pCmdItem_Cur->m_chCurCommand)
        {
            return;
        }

        m_iTimeOut = 0;

        if(iRetPak != m_pCmdItem_Cur->m_iTotalPaks) // �ܰ�������
        {
            // �˳�����
            ShowTipInfoFunc("MCU ��Ӧ ��������: �ظ��ܰ��� ����, �˳�����");
        }

        if(0x00 == chParam)         // ûȫ��ȷ
        {
            // �˳�����
            ShowTipInfoFunc("MCU ��Ӧ ��������: ûȫ��ȷ���˳�����");
        }
        else if(0x01 == chParam)    // ��ȷ�յ�ȫ������
        {
            ShowTipInfoFunc("MCU ��Ӧ ��������: ��ȷ�յ�ȫ�����ݣ��˳�����");
        }
        else
        {
            ShowTipInfoFunc("MCU ��Ӧ ��������: δ֪�������˳�����");
        }

        // ������Զ����أ��򲻵�������ĺ���
        if(en_Cmd_B_Down_Is_More != m_pCmdItem_Cur->m_enFrameBType) ExitDownload();
    }
    else
    {
        //ReDownloadFunc(tr("MCU �ظ�δ֪���"));           // �ط���֡

        // ����ʶ���������
        //m_pCmdItem_Cur->m_iReDownCount = 0;
    }
}

// ����һ֡(�������ط�)
void CUpdateTab::SendOneFrame(unsigned char chCommand, unsigned char* pchData, int iDataSize, bool isReSend)
{
    // ��ʱ��ʱ��λ
    m_iTimeOut = 0;

    if(true == isReSend)        // �ط���֡
    {
        // ����
        m_pMyComBasic->ReSendCurComNetData();

        ++ m_pCmdItem_Cur->m_iReDownCount;
    }
    else
    {
        // ����
        SendComNetData(chCommand, (const char*)pchData, iDataSize);
    }
}

// ���� ׼������ ����
void CUpdateTab::SendPrepareUpdate()
{
    m_pCmdItem_Cur->DownloadReset();

    // ����Դ��ַ
    m_pMyComBasic->SetSourceAddr(m_pCmdItem_Cur->m_chSourceAddr, m_pCmdItem_Cur->m_chSourcePort, m_pCmdItem_Cur->m_chSourceUse);

    // ����Ŀ�ĵ�ַ
    m_pMyComBasic->SetTargetAddr(m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort, m_pCmdItem_Cur->m_chTargetUse);

    // ����������
    int iFrameNum = GetFrameNo();
    m_pMyComBasic->SetCommand(iFrameNum, m_pCmdItem_Cur->m_chFrameCmd);

    FrameNoInc(iFrameNum);

    // ׼������
    fseek(m_pCmdItem_Cur->m_pFile, 0, SEEK_END);
    unsigned int iTotalSize = ftell(m_pCmdItem_Cur->m_pFile);
    m_pCmdItem_Cur->m_iTotalPaks = iTotalSize / 256 + 1;

    // �������ļ���
    char chFileName[MAX_PATH] = {0};
    strcpy_s(chFileName, sizeof(chFileName), m_pCmdItem_Cur->m_strFilePath.toLatin1().data());
    char* ptr = chFileName + strlen(chFileName) - 1;
    while(('V' != *ptr) && ('v' != *ptr)) --ptr;
    if('_' == *(ptr-1)) --ptr;
    *ptr = '\0';
    --ptr;
    while(('/' != *ptr) && ('\\' != *ptr)) --ptr;
    ++ptr;

    // �ܰ���(2)
    unsigned int iHighLength = (m_pCmdItem_Cur->m_iTotalPaks)/256;
    unsigned int iLowLength = (m_pCmdItem_Cur->m_iTotalPaks)%256;

    unsigned char chData[MAX_PATH] = {0};
    chData[0] = iHighLength;
    chData[1] = iLowLength;
    chData[2] = '@';
    memcpy(chData+3, ptr, strlen(ptr));
    iLowLength = strlen((char*)chData+2) + 2;
    chData[iLowLength] = '#';
    iLowLength = strlen((char*)chData+2) + 2;       // ���ĳ���

    // ����
    m_pCmdItem_Cur->m_chCurCommand = 0x70;
    SendOneFrame(m_pCmdItem_Cur->m_chCurCommand, chData, iLowLength);

    ShowTipInfoFunc("���� ׼������ ����");

    m_iTimeOut = 0;

    rewind(m_pCmdItem_Cur->m_pFile);                // ��λ�ļ�ָ��

    // ��ʾ��Ϣ
    char chInfo[1024] = {0};
    sprintf_s(chInfo, sizeof(chInfo), "����Ŀ��忨[%02X, %02X][%s]���", 
                            m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort, 
                            m_pCmdItem_Cur->m_strFilePath.toLatin1().data());
    ShowTipInfoFunc(chInfo);

    // ����Ŀ���ַ
    if(true == g_oPageCheck.IsAdmin())
    {
        sprintf_s(chInfo, sizeof(chInfo), "%X",m_pCmdItem_Cur->m_chTargetAddr);
        m_pTargetAddr_LineEdit->setText(chInfo);
        sprintf_s(chInfo, sizeof(chInfo), "%X", m_pCmdItem_Cur->m_chTargetPort);
        m_pTargetPort_LineEdit->setText(chInfo);
    }
}

// ����һ������
void CUpdateTab::SendNextOnePak()
{
    if(feof(m_pCmdItem_Cur->m_pFile))           // �ļ�����
    {
        // �ܰ���(2)
        unsigned int iHighLength = (m_pCmdItem_Cur->m_iTotalPaks)/256;
        unsigned int iLowLength = (m_pCmdItem_Cur->m_iTotalPaks)%256;

        unsigned char chDataH[2] = {0};
        chDataH[0] = iHighLength;
        chDataH[1] = iLowLength;

        // ����
        m_pCmdItem_Cur->m_chCurCommand = 0x74;
        SendOneFrame(m_pCmdItem_Cur->m_chCurCommand, chDataH, 2);

        return;
    }

    unsigned char chData[256+2];
    memset(chData, 0xFF, sizeof(chData));

    // �ǡ����ܡ�
    if((true == m_bIsEncrypt) && (0x70 == m_pCmdItem_Cur->m_chCurCommand))
    {
        // �ܰ���(2)
        unsigned int iHighLength = (m_pCmdItem_Cur->m_iTotalPaks)/256;
        unsigned int iLowLength = (m_pCmdItem_Cur->m_iTotalPaks)%256;

        chData[0] = iHighLength;
        chData[1] = iLowLength;

        // ����
        m_pCmdItem_Cur->m_chCurCommand = 0x76;
        SendOneFrame(m_pCmdItem_Cur->m_chCurCommand, chData, 2);
    }
   else if((true == m_bUseFileName) && ((0x70 == m_pCmdItem_Cur->m_chCurCommand) || (0x76 == m_pCmdItem_Cur->m_chCurCommand)))
    {
        // �ļ���
        int iLength = m_strFileNameUse.length();
        memcpy(chData, m_strFileNameUse.toLatin1().data(), iLength);
        chData[iLength] = '\0';

        // ����
        m_pCmdItem_Cur->m_chCurCommand = 0x78;
        SendOneFrame(m_pCmdItem_Cur->m_chCurCommand, chData, iLength+1);
    }
    else            // ��������
    {
        // ����(2)
        ++m_pCmdItem_Cur->m_iPaksIndex;
        unsigned int iHighLength = (m_pCmdItem_Cur->m_iPaksIndex)/256;
        unsigned int iLowLength = (m_pCmdItem_Cur->m_iPaksIndex)%256;

        chData[0] = iHighLength;
        chData[1] = iLowLength;

        int iReadSize = 256;
        fread_s(chData+2, iReadSize, iReadSize, 1, m_pCmdItem_Cur->m_pFile);

        // ����
        m_pCmdItem_Cur->m_chCurCommand = 0x72;
        SendOneFrame(m_pCmdItem_Cur->m_chCurCommand, chData, sizeof(chData));
    }
    
}

// ��λ ��������
void CUpdateTab::DownloadReset()
{
    m_iTimeOut = 0;

    m_bSingleDebug = false;
    m_iSingleNum = 0;
}

// �˳� ����
void CUpdateTab::ExitDownload()
{
    if(NULL != m_pCmdItem_Cur) m_pCmdItem_Cur->ExitDownload();
    GetNextCmd();                   // ��һ������

    m_bStopDownload = false;
    m_bSingleDebug = false;
    m_iSingleNum = 0;
}

void CUpdateTab::DestroyAllCmd()
{
    st_CommandItem* pCmdItem = m_pCmdItem_Head;
    st_CommandItem* pCmdDel = NULL;
    while(NULL != pCmdItem)
    {
        pCmdDel = pCmdItem;
        pCmdItem = pCmdItem->m_next;
        delete pCmdDel;
    }
    m_pCmdItem_Head = NULL;
    m_pCmdItem_Cur = NULL;
}
void CUpdateTab::AddOneCmd(st_CommandItem* pOneItem)
{
    st_CommandItem* pCmdItem = m_pCmdItem_Head;
    if(NULL == pCmdItem)
    {
        m_pCmdItem_Head = pOneItem;
    }
    else
    {
        while(NULL != pCmdItem)
        {
            if(NULL == pCmdItem->m_next)
            {
                pCmdItem->m_next = pOneItem;
                break;
            }
            pCmdItem = pCmdItem->m_next;
        }
    }
}

void CUpdateTab::GetFirstCmd()
{
    m_pCmdItem_Cur = m_pCmdItem_Head;
}
void CUpdateTab::GetNextCmd()
{
    if(NULL != m_pCmdItem_Cur) m_pCmdItem_Cur = m_pCmdItem_Cur->m_next;
}

void CUpdateTab::DestroyAllBoardVerInfo()
{
    st_BoardVerInfo* pVerInfo = NULL;
    BOARDVERINFOMAPTYPE::iterator itC = m_BoardVerInfo_map.begin();
    for(; itC != m_BoardVerInfo_map.end(); ++itC)
    {
        pVerInfo = itC->second;
        if(NULL == pVerInfo) continue;
        delete pVerInfo;
    }
    m_BoardVerInfo_map.clear();
}

bool CUpdateTab::GetBoardNameAndVerOnInput(const char* pszInput, char* pszName, int iNamelength, float& fVersion)
{
    // Rem_Control NavtfVerx.y (Vx.yǰ���пո�С�������ֻ��һλ��)
    char chInput[1024] = {0};
    strcpy_s(chInput, sizeof(chInput), pszInput);
    char* ptr = ::strstr(chInput, "NavtfVer");
    char* ptrValue = ptr;
    if(NULL != ptr)
    {
        // 1. ���ƣ�ȥ����Ŀո�
        ptr -= 1;
        while(' ' == *ptr) --ptr;
        ++ptr;
        *ptr = '\0';
        strcpy_s(pszName, iNamelength, chInput);

        // 2. �汾��ȥǰ��Ŀո�
        ptr = ptrValue;
        while((' ' != *ptr) && ('\0' != *ptr)) ++ptr;
        *ptr = '\0';
        ptrValue += 8;
        fVersion = atof(ptrValue);
        if(strlen(pszName) < 1) return false;
        if(fVersion < 0.0f) return false;
        return true;
    }
    else
    {
        return false;
    }
}
bool CUpdateTab::GetBoardNameOnComBox(const char* pszInput, char* pszName, int iNameLength)
{
    // �ο������_Ref_Input_P0102_Vx.y
    char chInput[1024] = {0};
    strcpy_s(chInput, sizeof(chInput), pszInput);
    char* ptr = ::strstr(chInput, "_");
    if(NULL != ptr)
    {
        *ptr = '\0';
        ++ptr;
        char* ptrValue = ptr;
        ptr = ::strstr(ptr, "_V");
        if(NULL != ptr)
        {
            *ptr = '\0';
            strcpy_s(pszName, iNameLength, ptrValue);
            if(strlen(pszName) < 1) return false;
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

// �ɰ忨��ַ��ȡ�忨����
QString CUpdateTab::GetBoardNameOnAddr(unsigned int iAddr, unsigned int iPort)
{
    QString strName = tr("����δ��");
    st_BoardVerInfo* pBVInfo = NULL;
    BOARDVERINFOMAPTYPE::iterator itC = m_BoardVerInfo_map.begin();
    for(; itC != m_BoardVerInfo_map.end(); ++itC)
    {
        pBVInfo = itC->second;
        if(NULL == pBVInfo) continue;
        if((iAddr == pBVInfo->m_chAddr) && (iPort == pBVInfo->m_chPort))
        {
            strName = pBVInfo->m_strOriName.c_str();

            std::string strHereName = m_BoardName_map[strName.toLatin1().data()];
            if(!strHereName.empty())
            {
                strName = strHereName.c_str();
            }
            break;
        }
    }
    return strName;
}

// �·������������Ƿ�ɹ�
void CUpdateTab::CheckConnect()
{
    SetFrameDataStyle(false, false, false, true);
    DestroyAllCmd();
    m_enOpType = en_Op_Is_Ver_One;

    st_CommandItem* pCmdItem = new st_CommandItem(en_Cmd_A_Is_Version, en_Cmd_B_Send_One_Cmd);
    AddOneCmd(pCmdItem);

    pCmdItem->m_chSourceAddr = 0x60;     // Դ��Ŀ�ĺ�����
    pCmdItem->m_chSourcePort = 0x00;
    pCmdItem->m_chSourceUse = 0x00;
    pCmdItem->m_chTargetAddr = 0x80;
    pCmdItem->m_chTargetPort = 0x00;
    pCmdItem->m_chTargetUse = 0x00;
    pCmdItem->m_chFrameCmd = COMMAND_IS_AT;
    pCmdItem->m_bManual = true;

    GetFirstCmd();

    // ���͵�ǰ����ͷ��
    {
        // ����Դ��ַ
        m_pMyComBasic->SetSourceAddr(m_pCmdItem_Cur->m_chSourceAddr, m_pCmdItem_Cur->m_chSourcePort, m_pCmdItem_Cur->m_chSourceUse);

        // ����Ŀ�ĵ�ַ
        m_pMyComBasic->SetTargetAddr(m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort, m_pCmdItem_Cur->m_chTargetUse);

        // ����������
        int iFrameNum = GetFrameNo();
        m_pMyComBasic->SetCommand(iFrameNum, m_pCmdItem_Cur->m_chFrameCmd);

        FrameNoInc(iFrameNum);

        m_bCmdIsNavtfIsOne = false;
        m_bCmdIsNavtfFinished = false;

        // ����
        char chData[64] = "navtf2000";
        SendOneFrame(m_pCmdItem_Cur->m_chFrameCmd, (unsigned char*)chData, strlen(chData));

        ShowTipInfoFunc("���� navtf2000 ����");

        if(NULL != m_pMyImageTab)
        {
            m_pMyImageTab->CmdClear();
            m_pMyImageTab->ClearAllReduceShow();
        }
    }

    m_iTimeOut = 0;
    m_bStopDownload = false;
    SetAllEnabled(false);

    DestroyAllBoardVerInfo();
    m_pBoardSelect_ComboBox->clear();
    m_pBoardSelect_ComboBox->setToolTip("");
    m_pBoardSelect_ComboBox->addItem(NAME_OF_VERSION_ALL);

}

// ����·�������ķ��ؽ��
char g_chOneCmdUseBuf[1024*5] = {0};
char g_chTextBuf[1024*5] = {0};
void CUpdateTab::CheckSendedOneCmd(const char* pszInput, int iMultiFrame)
{
    emit CheckSendedOneCmd_Signal(pszInput, iMultiFrame);
}
void CUpdateTab::CheckSendedOneCmd_Slot(const QString strData, int iMultiFrame)
{
    strcpy_s(g_chTextBuf, sizeof(g_chTextBuf), strData.toLatin1().data());
    const char* pszInput = g_chTextBuf;

    if(iMultiFrame < 1) return;
    if(true == m_bCmdIsNavtfFinished) return;

    strcat_s(g_chOneCmdUseBuf, sizeof(g_chOneCmdUseBuf), pszInput);

    // ָʾ�Ƿ��Ƕ�֡
    char chLow = ((char)iMultiFrame) & 0x0F;
    char chHigh = ((char)iMultiFrame >> 4) & 0x0F;
    if((iMultiFrame > 1) && (chLow != chHigh))
    {
        return;
    }

    char* ptr = NULL;
    char* chInput = g_chOneCmdUseBuf;
    char chKey[32] = {0};
    QString strInfo = "";

    // navtf2000 0/1
    strcpy_s(chKey, sizeof(chKey), "navtf2000");
    if((true == m_bCmdIsNavtfIsOne) || (NULL != (ptr = strstr(chInput, chKey))))
    {
        char chData_1[64] = {0};
        char chData_2[32] = {0};
        char chData_3[32] = {0};
        char chData_4[32] = {0};
        char chAddr = 0;
        char chPort = 0;
        float fVer = 0.0f;
        bool bOK = false;
        if((true == m_bCmdIsNavtfIsOne) || (true == ParseInputString(ptr, 2, chData_1, chData_2, chData_3, chData_4)))
        {
            if((true == m_bCmdIsNavtfIsOne) || ((strcmp(chKey, chData_1) == 0) && (1 == atoi(chData_2))))
            {
                m_bCmdIsNavtfIsOne = true;

                // Main_Control  V2.1  8000  369
                strcpy_s(chKey, sizeof(chKey), "Main_Control");
                if(NULL != (ptr = strstr(chInput, chKey)))
                {
                    if(true == ParseInputString(ptr, 4, chData_1, chData_2, chData_3, chData_4))
                    {
                        strInfo = chData_3 + 2;
                        chPort = strInfo.toInt(&bOK, 16);
                        chData_3[2] = '\0';
                        strInfo = chData_3;
                        chAddr = strInfo.toInt(&bOK, 16);
                        fVer = atof(chData_2+1);
                        AddOneBoard(chData_1, sizeof(chData_1), chAddr, chPort, fVer);

                        if(NULL != m_pMyAdjustTab)
                        {
                            m_pMyAdjustTab->SetAdjustAll(chData_4);
                        }
                    }
                }

                // Reference_GNSS V2.5 0100
                strcpy_s(chKey, sizeof(chKey), "Reference_GNSS");
                if(NULL != (ptr = strstr(chInput, chKey)))
                {
                    if(true == ParseInputString(ptr, 3, chData_1, chData_2, chData_3, chData_4))
                    {
                        strInfo = chData_3 + 2;
                        chPort = strInfo.toInt(&bOK, 16);
                        chData_3[2] = '\0';
                        strInfo = chData_3;
                        chAddr = strInfo.toInt(&bOK, 16);
                        fVer = atof(chData_2+1);
                        AddOneBoard(chData_1, sizeof(chData_1), chAddr, chPort, fVer);
                    }
                }

                // Main_PLL10.23M  V2.1  8023 6
                strcpy_s(chKey, sizeof(chKey), "Main_PLL10.23M");
                if(NULL != (ptr = strstr(chInput, chKey)))
                {
                    if(true == ParseInputString(ptr, 4, chData_1, chData_2, chData_3, chData_4))
                    {
                        strInfo = chData_3 + 2;
                        chPort = strInfo.toInt(&bOK, 16);
                        chData_3[2] = '\0';
                        strInfo = chData_3;
                        chAddr = strInfo.toInt(&bOK, 16);
                        fVer = atof(chData_2+1);
                        AddOneBoard(chData_1, sizeof(chData_1), chAddr, chPort, fVer);

                        if(NULL != m_pMyImageTab)
                        {
                            m_pMyImageTab->SetFreqAndPhaseEdit(6, chData_4);
                        }
                    }
                }

                // Main_PLL10.00M  V2.1  80aa
                strcpy_s(chKey, sizeof(chKey), "Main_PLL10.00M");
                if(NULL != (ptr = strstr(chInput, chKey)))
                {
                    if(true == ParseInputString(ptr, 3, chData_1, chData_2, chData_3, chData_4))
                    {
                        strInfo = chData_3 + 2;
                        chPort = strInfo.toInt(&bOK, 16);
                        chData_3[2] = '\0';
                        strInfo = chData_3;
                        chAddr = strInfo.toInt(&bOK, 16);
                        fVer = atof(chData_2+1);
                        AddOneBoard(chData_1, sizeof(chData_1), chAddr, chPort, fVer);
                    }
                }

                // Remote_Monitoring  V2.1  2100  
                strcpy_s(chKey, sizeof(chKey), "Remote_Monitoring");
                if(NULL != (ptr = strstr(chInput, chKey)))
                {
                    if(true == ParseInputString(ptr, 3, chData_1, chData_2, chData_3, chData_4))
                    {
                        strInfo = chData_3 + 2;
                        chPort = strInfo.toInt(&bOK, 16);
                        chData_3[2] = '\0';
                        strInfo = chData_3;
                        chAddr = strInfo.toInt(&bOK, 16);
                        fVer = atof(chData_2+1);
                        AddOneBoard(chData_1, sizeof(chData_1), chAddr, chPort, fVer);
                    }
                }

                // Reference_Input  V2.1  2200  
                strcpy_s(chKey, sizeof(chKey), "Reference_Input");
                if(NULL != (ptr = strstr(chInput, chKey)))
                {
                    if(true == ParseInputString(ptr, 3, chData_1, chData_2, chData_3, chData_4))
                    {
                        strInfo = chData_3 + 2;
                        chPort = strInfo.toInt(&bOK, 16);
                        chData_3[2] = '\0';
                        strInfo = chData_3;
                        chAddr = strInfo.toInt(&bOK, 16);
                        fVer = atof(chData_2+1);
                        AddOneBoard(chData_1, sizeof(chData_1), chAddr, chPort, fVer);
                    }
                }

                // priority 123412 1
                strcpy_s(chKey, sizeof(chKey), "priority");
                if(NULL != (ptr = strstr(chInput, chKey)))
                {
                    int iPriority1 = 0;
                    int iPriority2 = 0;
                    int iPriority3 = 0;
                    int iPriority4 = 0;

                    int iPriority5 = 0;
                    int iPriority6 = 0;

                    int iPriorityType = 0;

                    char chValue[2] = {0};
                    while((*ptr < '0') || (*ptr > '9')) ++ptr;
                    chValue[0] = *ptr;
                    ++ptr;
                    iPriority1 = atoi(chValue);
                    chValue[0] = *ptr;
                    ++ptr;
                    iPriority2 = atoi(chValue);
                    chValue[0] = *ptr;
                    ++ptr;
                    iPriority3 = atoi(chValue);
                    chValue[0] = *ptr;
                    ++ptr;
                    iPriority4 = atoi(chValue);

                    chValue[0] = *ptr;
                    ++ptr;
                    iPriority5 = atoi(chValue);
                    chValue[0] = *ptr;
                    ++ptr;
                    iPriority6 = atoi(chValue);

                    while((*ptr < '0') || (*ptr > '9')) ++ptr;
                    chValue[0] = *ptr;
                    iPriorityType = atoi(chValue);

                    if(iPriority1 < 1 || iPriority1 > 4) iPriority1 = 4;
                    if(iPriority2 < 1 || iPriority2 > 4) iPriority2 = 4;
                    if(iPriority3 < 1 || iPriority3 > 4) iPriority3 = 4;
                    if(iPriority4 < 1 || iPriority4 > 4) iPriority4 = 4;

                    if(iPriority5 < 1 || iPriority5 > 2) iPriority5 = 2;
                    if(iPriority6 < 1 || iPriority6 > 2) iPriority6 = 2;

                    if(iPriorityType < 1 || iPriorityType > 3) iPriorityType = 3;

                    if(NULL != m_pMyImageTab)
                    {
                        m_pMyImageTab->SetPriority(iPriority1, iPriority2, iPriority3, iPriority4, iPriority5, iPriority6, iPriorityType);
                    }
                }

                // Frequ_1 6
                strcpy_s(chKey, sizeof(chKey), "Frequ_1");
                if(NULL != (ptr = strstr(chInput, chKey)))
                {
                    if(true == ParseInputString(ptr, 2, chData_1, chData_2, chData_3, chData_4))
                    {
                        if(NULL != m_pMyImageTab)
                        {
                            m_pMyImageTab->SetFreqAndPhaseEdit(0, chData_2);
                        }
                    }
                }

                // Frequ_2 2
                strcpy_s(chKey, sizeof(chKey), "Frequ_2");
                if(NULL != (ptr = strstr(chInput, chKey)))
                {
                    if(true == ParseInputString(ptr, 2, chData_1, chData_2, chData_3, chData_4))
                    {
                        if(NULL != m_pMyImageTab)
                        {
                            m_pMyImageTab->SetFreqAndPhaseEdit(1, chData_2);
                        }
                    }
                }

                // Phase_1 0
                strcpy_s(chKey, sizeof(chKey), "Phase_1");
                if(NULL != (ptr = strstr(chInput, chKey)))
                {
                    if(true == ParseInputString(ptr, 2, chData_1, chData_2, chData_3, chData_4))
                    {
                        if(NULL != m_pMyImageTab)
                        {
                            m_pMyImageTab->SetFreqAndPhaseEdit(2, chData_2);
                        }
                    }
                }

                // Phase_2 4
                strcpy_s(chKey, sizeof(chKey), "Phase_2");
                if(NULL != (ptr = strstr(chInput, chKey)))
                {
                    if(true == ParseInputString(ptr, 2, chData_1, chData_2, chData_3, chData_4))
                    {
                        if(NULL != m_pMyImageTab)
                        {
                            m_pMyImageTab->SetFreqAndPhaseEdit(3, chData_2);
                        }
                    }
                }

                // PPS_Pulse_2  N
                strcpy_s(chKey, sizeof(chKey), "PPS_Pulse_2");
                if(NULL != (ptr = strstr(chInput, chKey)))
                {
                    if(true == ParseInputString(ptr, 2, chData_1, chData_2, chData_3, chData_4))
                    {
                        if(NULL != m_pMyImageTab)
                        {
                            m_pMyImageTab->SetFreqAndPhaseEdit(4, chData_2);
                        }
                    }
                }

                // PPS_Delay_2  N
                strcpy_s(chKey, sizeof(chKey), "PPS_Delay_2");
                if(NULL != (ptr = strstr(chInput, chKey)))
                {
                    if(true == ParseInputString(ptr, 2, chData_1, chData_2, chData_3, chData_4))
                    {
                        if(NULL != m_pMyImageTab)
                        {
                            m_pMyImageTab->SetFreqAndPhaseEdit(5, chData_2);
                        }
                    }
                }

                // RF_Distri125M  V2.5  0400  0
                // RF_Distribute  V2.1  0500  3 
                // RF_Distribute  V2.1  0600  3 
                // RF_Distribute  V2.1  2500  3 
                // RF_Distribute  V2.1  2600  3 
                ptr = chInput;
                strcpy_s(chKey, sizeof(chKey), "RF_Distri"); // RF_Distribute n
                while(NULL != (ptr = strstr(ptr, chKey)))
                {
                    if(true == ParseInputString(ptr, 4, chData_1, chData_2, chData_3, chData_4))
                    {
                        strInfo = chData_3 + 2;
                        chPort = strInfo.toInt(&bOK, 16);
                        chData_3[2] = '\0';
                        strInfo = chData_3;
                        chAddr = strInfo.toInt(&bOK, 16);
                        fVer = atof(chData_2+1);
                        AddOneBoard(chData_1, sizeof(chData_1), chAddr, chPort, fVer);

                        if(NULL != m_pMyImageTab)
                        {
                            m_pMyImageTab->SetReduceShowValue(atoi(chData_4), chAddr, chPort);
                        }
                    }

                    // next
                    ++ptr;
                }

                // LVDS_Distribute  V2.1  0100  
                ptr = chInput;
                strcpy_s(chKey, sizeof(chKey), "LVDS_Distribute");
                while(NULL != (ptr = strstr(ptr, chKey)))
                {
                    if(true == ParseInputString(ptr, 3, chData_1, chData_2, chData_3, chData_4))
                    {
                        strInfo = chData_3 + 2;
                        chPort = strInfo.toInt(&bOK, 16);
                        chData_3[2] = '\0';
                        strInfo = chData_3;
                        chAddr = strInfo.toInt(&bOK, 16);
                        fVer = atof(chData_2+1);
                        AddOneBoard(chData_1, sizeof(chData_1), chAddr, chPort, fVer);
                    }

                    // next
                    ++ptr;
                }

                // LVTTL_Distribute  V2.1  0200  
                ptr = chInput;
                strcpy_s(chKey, sizeof(chKey), "LVTTL_Distribute");
                while(NULL != (ptr = strstr(ptr, chKey)))
                {
                    if(true == ParseInputString(ptr, 3, chData_1, chData_2, chData_3, chData_4))
                    {
                        strInfo = chData_3 + 2;
                        chPort = strInfo.toInt(&bOK, 16);
                        chData_3[2] = '\0';
                        strInfo = chData_3;
                        chAddr = strInfo.toInt(&bOK, 16);
                        fVer = atof(chData_2+1);
                        AddOneBoard(chData_1, sizeof(chData_1), chAddr, chPort, fVer);
                    }

                    // next
                    ++ptr;
                }

                // ���ܶ��: PLL_Universal V2.5 0400
                ptr = chInput;
                strcpy_s(chKey, sizeof(chKey), "PLL_Universal"); 
                while(NULL != (ptr = strstr(ptr, chKey)))
                {
                    if(true == ParseInputString(ptr, 3, chData_1, chData_2, chData_3, chData_4))
                    {
                        strInfo = chData_3 + 2;
                        chPort = strInfo.toInt(&bOK, 16);
                        chData_3[2] = '\0';
                        strInfo = chData_3;
                        chAddr = strInfo.toInt(&bOK, 16);
                        fVer = atof(chData_2+1);
                        AddOneBoard(chData_1, sizeof(chData_1), chAddr, chPort, fVer);
                    }

                    // next
                    ++ptr;
                }
                
                // ���ؼ�
                m_bCmdIsNavtfFinished = true;
                m_bCmdIsNavtfIsOne = false;
                BoardSelectFill();

                ShowTipInfoFunc("\n�豸���ӳɹ�\n", QColor(0, 0, 250));

                // �ֹ�ִ��һ��
                CmdFinished_Func();
            }
            else
            {
                ;
            }
        }
    }

    // ��
    memset(g_chOneCmdUseBuf, 0, sizeof(g_chOneCmdUseBuf));
}
bool CUpdateTab::ParseInputString(const char* pszInput, int iCheckNum, char* pszOut1, char* pszOut2, char* pszOut3, char* pszOut4)
{
    if(iCheckNum < 1) iCheckNum = 1;
    if(iCheckNum > 4) iCheckNum = 4;

    char* ptr1 = (char*)pszInput;
    char* ptr2 = NULL;
    int i = 0;
    int iLength = 0;
    for(i = 0; i < iCheckNum; ++i)
    {
        while(('\0' != *ptr1) && ((',' == *ptr1) || (' ' == *ptr1))) ++ptr1;
        ptr2 = ptr1;
        while(('\0' != *ptr2) && (',' != *ptr2) && (' ' != *ptr2)) ++ptr2;
        iLength = ptr2 - ptr1;
        if((iLength <= 0) || (iLength > 30)) 
        {
            i = 0;          // �� ����
            break;
        }

        switch(i)
        {
        case 0:
            memcpy(pszOut1, ptr1, iLength);
            pszOut1[iLength] = '\0';
            break;
        case 1:
            memcpy(pszOut2, ptr1, iLength);
            pszOut2[iLength] = '\0';
            break;
        case 2:
            memcpy(pszOut3, ptr1, iLength);
            pszOut3[iLength] = '\0';
            break;
        case 3:
            memcpy(pszOut4, ptr1, iLength);
            pszOut4[iLength] = '\0';
            break;
        default:
            break;
        }

        ptr1 = ptr2;
    }

    if(i == iCheckNum) return true;
    return false;
}

// ����һ���忨
void CUpdateTab::AddOneBoard(char* chName, int iNameLength, unsigned char chAddr, unsigned char chPort, float fVer)
{
    // ע�⣺Ϊ������ͬ�İ忨�����ƺ�����ϵ�ַ��Ϣ��Ϊ��׺ "_P0101" : Ref_Input_P0102
    st_BoardVerInfo* pVerInfo = new st_BoardVerInfo();
    pVerInfo->m_strName = chName;
    pVerInfo->m_strOriName = chName;
    pVerInfo->m_chAddr = chAddr;
    pVerInfo->m_chPort = chPort;
    pVerInfo->m_chUse = 0;
    pVerInfo->m_fVer = fVer;
    sprintf_s(chName, iNameLength, "_P%02X%02X", pVerInfo->m_chAddr, pVerInfo->m_chPort);
    pVerInfo->m_strName += chName;
    m_BoardVerInfo_map[pVerInfo->m_strName] = pVerInfo;
}

// ǿ����������=1����ѡ���Զ���֡������ѡ���Զ��ط���������16���ơ�������16���ơ�
void CUpdateTab::SetCmdAndOptionManu()
{
    m_pFrameCmd_LineEdit->setText("1");
    m_pAutoMakeFrame_CheckBox->setChecked(true);

    m_pAutoReSend_CheckBox->setChecked(false);
    m_pSend16Data_CheckBox->setChecked(false);
    m_pRecv16Data_CheckBox->setChecked(false);
}


// ѡ�񵼳��ļ�
void CUpdateTab::SelectExportFilePath()
{
    QFileDialog::Options options;
    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(this, tr("�ļ�����"), tr("�����ļ�"), 
                                                    tr("Text Files (*.txt)"), &selectedFilter, options);
    if(false == fileName.isEmpty())
    {
        m_pExportFileShow_LineEdit->setText(fileName);

        CloseExportFile();
        OpenExportFile(fileName.toLatin1().data());
        if(NULL != m_hExportFile)
        {
            // INI
            CFuncTool::SetExportFilePath(fileName.toLatin1().data());
        }
    }
}

// ������������
void CUpdateTab::NetConnectTestFunc()
{
    m_pNetConnectTest_Button->setEnabled(false);

    QString strData = tr("ping %1 -l 2000").arg(m_pNetIPAddr_LineEdit->text());
    CFuncTool::ExecCommand(strData.toLatin1().data());
}

// �򿪺͹رյ����ļ�
void CUpdateTab::CloseExportFile()
{
    if(NULL != m_hExportFile)
    {
        CloseHandle((HANDLE)m_hExportFile);
        m_hExportFile = NULL;
    }
}
void CUpdateTab::OpenExportFile(char* pszFilePath)
{
    HANDLE hFile = CreateFile(pszFilePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
    if(INVALID_HANDLE_VALUE == hFile)
    {
        m_hExportFile = NULL;
        //ShowTipInfoFunc("�򿪵����ļ�ʧ��");
    }
    else
    {
        // ָ���ƶ����ļ�β
        SetFilePointer(hFile , 0, NULL, FILE_END);
        m_hExportFile = hFile;
    }
}

// ѡ�񵥰廹��ϵͳ(���)
void CUpdateTab::BoardTypeChanged(int iIndex)
{
    if(UPDATE_SELECT_IS_ALONE == iIndex)
    {
        m_pSelectAlone_Radio->setChecked(true);
        m_pSelectSystem_Radio->setChecked(false);

        m_pOpenFile_Button->setEnabled(true);
        m_pFileShow_LineEdit->setEnabled(true);
        m_pTargetAddr_LineEdit->setEnabled(true);
        m_pTargetPort_LineEdit->setEnabled(true);

        m_pBoardSelect_ComboBox->setEnabled(false);
    }
    else if(UPDATE_SELECT_IS_SYSTEM == iIndex)
    {
        m_pSelectAlone_Radio->setChecked(false);
        m_pSelectSystem_Radio->setChecked(true);

        m_pOpenFile_Button->setEnabled(false);
        m_pFileShow_LineEdit->setEnabled(false);
        m_pTargetAddr_LineEdit->setEnabled(false);
        m_pTargetPort_LineEdit->setEnabled(false);

        m_pBoardSelect_ComboBox->setEnabled(true);
    }

    // INI
    CFuncTool::SetUpdateSelect(iIndex);
}

// �����·� ������־
void CUpdateTab::CmdLog_Func(unsigned char chAddr, unsigned char chPort, QString& strLog)
{
    emit CmdLog_Signal(chAddr, chPort, strLog);
}
void CUpdateTab::CmdLog_Slot(unsigned char chAddr, unsigned char chPort, QString strLog)
{
    if(NULL == m_hCmdLogFile)
    {
        char chPath[512] = {0};
        strcpy_s(chPath, sizeof(chPath), "./Log");
        if(false == CFuncTool::FolderExist(chPath))
        {
            if(FALSE == CreateDirectory(chPath, NULL))
            {
                ShowTipInfoFunc("���� Log �ļ���ʧ��");
                return;
            }
        }

        QString strTime = QDate::currentDate().toString("yyyy_MM_dd");
        sprintf_s(chPath, sizeof(chPath), "./Log/%s.txt", strTime.toLatin1().data());
        HANDLE hFile = CreateFile(chPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
        if(INVALID_HANDLE_VALUE == hFile)
        {
            ShowTipInfoFunc("�� Log �ļ���ʧ��");
            return;
        }

        // ָ���ƶ����ļ�β
        SetFilePointer(hFile , 0, NULL, FILE_END);
        m_hCmdLogFile = hFile;
    }

    // ����ʱ��
    QString strCur = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    strCur += tr("  [%1, %2] ").arg(QString::number(chAddr, 16)).arg(QString::number(chPort)) + strLog;

    DWORD dwWrites;
    WriteFile((HANDLE)m_hCmdLogFile, strCur.toLatin1().data(), strCur.toLatin1().length(), &dwWrites, NULL);
    char chEnter[2] = {0x0D, 0x0A};
    WriteFile((HANDLE)m_hCmdLogFile, chEnter, 2, &dwWrites, NULL);
}

// ĳ����ַ�İ忨�Ƿ��Ѿ���ѯ��
bool CUpdateTab::BoardIsValid(unsigned char chAddr, unsigned char chPort)
{
    bool bIsValid = false;
    st_BoardVerInfo* pVerInfo = NULL;
    BOARDVERINFOMAPTYPE::iterator itC = m_BoardVerInfo_map.begin();
    for(; itC != m_BoardVerInfo_map.end(); ++itC)
    {
        pVerInfo = itC->second;
        if(NULL == pVerInfo) continue;
        if((chAddr == pVerInfo->m_chAddr) && (chPort == pVerInfo->m_chPort))
        {
            bIsValid = true;
            break;
        }
    }

    return bIsValid;
}

// ���ݵ�ַ��ȡ�忨��ԭʼ����
std::string CUpdateTab::GetBoardOriName(unsigned char chAddr, unsigned char chPort)
{
    std::string strRes = "";
    st_BoardVerInfo* pVerInfo = NULL;
    BOARDVERINFOMAPTYPE::iterator itC = m_BoardVerInfo_map.begin();
    for(; itC != m_BoardVerInfo_map.end(); ++itC)
    {
        pVerInfo = itC->second;
        if(NULL == pVerInfo) continue;
        if((chAddr == pVerInfo->m_chAddr) && (chPort == pVerInfo->m_chPort))
        {
            strRes = pVerInfo->m_strOriName;
            break;
        }
    }
    return strRes;
}

/****************************************************************************************
    ȫ�ֺ���
*****************************************************************************************/
// ��ʾ��������
void ShowRecvDataFunc(unsigned char chAddr, unsigned char chPort, const char* pszShowData, bool bChanged, int iR, int iG, int iB)
{
    if(NULL == g_pUpdateTab) return;
    g_pUpdateTab->ShowRecvDataFunc(chAddr, chPort, pszShowData, bChanged, iR, iG, iB);
}
void SetRecvShowFunc(int iShowType)
{
    if(NULL == g_pUpdateTab) return;
    g_pUpdateTab->SetRecvShowFunc(iShowType);
}

// ��ʾ��������
void ShowSendDataFunc(unsigned char chAddr, unsigned char chPort, const char* pszShowData, int iLength)
{
    if(NULL == g_pUpdateTab) return;
    g_pUpdateTab->ShowSendDataFunc(chAddr, chPort, pszShowData, iLength);
}


// �������س�ʱ����
void DownloadTimeoutFunc()
{
    if(NULL == g_pUpdateTab) return;
    g_pUpdateTab->DownloadTimeoutFunc();
}

// ֡���ݴ�����
void FrameHanleFunc(const st_FrameData* pFrameData)
{
    if(NULL == g_pUpdateTab) return;
    g_pUpdateTab->FrameHanleFunc(pFrameData);
}
