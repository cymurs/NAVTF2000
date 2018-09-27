#include "stdafx.h"
#include "updateTab.h"

#include <Windows.h>
#include <QtGui>

#include "../../../Common/Function/Function.h"
#include "../../../Libase/Libase.h"

#include "imageTab.h"               // 互相包含
#include "statusTab.h"
#include "adjustTab.h"
#include "gnssTab.h"

#include "../../../Common/Define/DefineFile.h"

// --------------------------------------------------------------------------------------
extern st_PageCheck g_oPageCheck;   // 功能权限

extern QString g_strGNSSAddr;       // GNSS 板卡的目的地址
extern QString g_strGNSSPort;

// 版本设置
extern int g_iLargeVer;
extern int g_iSmallVer;
extern int g_iYear;
extern int g_iMonth;
extern int g_iDay;


bool g_bShowBoardTip = true;        // 屏蔽“所有的板卡无信号输出提示” 输出


// --------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------
// 显示接收数据
void ShowRecvDataFunc(unsigned char chAddr, unsigned char chPort, const char* pszShowData, bool bChanged, int iR, int iG, int iB);
void SetRecvShowFunc(int iShowType);

// 显示发送数据
void ShowSendDataFunc(unsigned char chAddr, unsigned char chPort, const char* pszShowData, int iLength);

// 发生错误显示的颜色(设置颜色)
void ShowErrorColorFunc();

// 升级下载超时函数
void DownloadTimeoutFunc();

// 帧数据处理函数
void FrameHanleFunc(const st_FrameData* pFrameData);

// --------------------------------------------------------------------------------------
CUpdateTab* g_pUpdateTab = NULL;

// 帧处理数据变量
st_FrameData g_FrameData_1;
char* g_pFrameBuf_1 = NULL;
st_FrameData g_FrameData_2;
char* g_pFrameBuf_2 = NULL;

// 板卡地址
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
// 通信/升级 tab
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

    // 显示
    connect(this, SIGNAL(ShowRecvText_Signal(unsigned char, unsigned char, const QString, bool, int, int, int)), this, SLOT(ShowRecvText_Slot(unsigned char, unsigned char, const QString, bool, int, int, int)));
    connect(this, SIGNAL(ShowSendText_Signal(unsigned char, unsigned char, const QString)), this, SLOT(ShowSendText_Slot(unsigned char, unsigned char, const QString)));
    connect(this, SIGNAL(ShowTipText_Signal(const QString, QColor)), this, SLOT(ShowTipText_Slot(const QString, QColor)));

    // 控件
    connect(this, SIGNAL(FrameNoInc_Signal(int)), this, SLOT(FrameNoInc_Slot(int)));
    connect(this, SIGNAL(BoardSelectFill_Signal()), this, SLOT(BoardSelectFill_Slot()));

    // 命令结束
    connect(this, SIGNAL(CmdFinished_Signal()), this, SLOT(CmdFinished_Slot()));

    // 设备信号状态
    connect(this, SIGNAL(DeviceSignalStatus_Signal(int)), this, SLOT(DeviceSignalStatus_Slot(int)));

    connect(this, SIGNAL(CmdLog_Signal(unsigned char, unsigned char, QString)), this, SLOT(CmdLog_Slot(unsigned char, unsigned char, QString)));
    connect(this, SIGNAL(CheckSendedOneCmd_Signal(const QString, int)), this, SLOT(CheckSendedOneCmd_Slot(const QString, int)));

    // 自动升级目录
    char chAutoPath[1024] = {0};
    CFuncTool::GetMoudlePath(chAutoPath, sizeof(chAutoPath));
    m_strAutoPath = chAutoPath;
    m_strAutoPath += NAME_OF_AUTO_UPDATE_DIR;
    if(false == CFuncTool::FolderExist(m_strAutoPath.toLatin1()))
    {
        CFuncTool::CreateFolder(m_strAutoPath.toLatin1());
    }
    m_strAutoPath += "/";

    // 正则表达式
    // 1~2位16进制数字
    QRegExp rx1("^0|([1-9a-fA-F][0-9a-fA-F]{0,1}$)");
    m_p0x16_RegExpVal = new QRegExpValidator(rx1, this);

    // 1~6位数字
    QRegExp rx2("^[1-9][0-9]{0,5}$");
    m_pNum_RegExpVal = new QRegExpValidator(rx2, this);

    // IP 地址
    QRegExp rx3("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    m_pIP_RegExpVal = new QRegExpValidator(rx3, this);


    CreateWidgets();

    // 如果不是管理员版本，需要输入超级命令才能允许某些功能
    SetSomeFuncEnabled(g_oPageCheck.IsAdmin());
    
    // 初始化串口设置
    InitComInfo();

    // INI
    m_pComName_ComboBox->setCurrentIndex(CFuncTool::GetComIndex());
    m_pComBaud_ComboBox->setCurrentIndex(CFuncTool::GetComBaud());

    // 注意：串口波特率暂时固定为57600
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

    // 初始化板卡的名称对应关系
    {
        m_BoardName_map["Reference_Input"] = "参考输入板"; 
        m_BoardName_map["Remote_Monitoring"] = "远程监控板";
        m_BoardName_map["Main_PLL10.23M"] = "锁相环10.23M";
        m_BoardName_map["Main_PLL10.00M"] = "锁相环10.00M";
        m_BoardName_map["Main_Control"] = "主控板";
        m_BoardName_map["RF_Distribute"] = "正弦波时钟分配板";
        m_BoardName_map["LVTTL_Distribute"] = "LVTTL脉冲分配板";
        m_BoardName_map["LVDS_Distribute"] = "LVDS脉冲分配板";
        m_BoardName_map["Reference_GNSS"] = "GNSS输入板";
        m_BoardName_map["RF_Distri125M"] = "正弦波时钟分配板";
        m_BoardName_map["PLL_Universal"] = "通用锁相环板";
    }

    // 申请自动下载报告内存
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

// 如果不是管理员版本，需要输入超级命令才能允许某些功能
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

    // 因为物体生成顺序先后之间的问题，这里在重新设置一遍选项
    // 点击“发送”按钮，重新设置数据是否是16进制; 点击升级类按钮，强制设置为16进制(恢复设置)
    SetFrameDataStyle(true, true, true, true);

    // 显示使用的DLL版本
    int iLargeVer = 0;
    int iSmallVer = 0;
    int iYear = 0;
    int iMonth = 0;
    int iDay = 0;
    QString strInfo = "";

    // 通信
    m_pMyComBasic->GetDllLibVerAndDate(iLargeVer, iSmallVer, iYear, iMonth, iDay);
    strInfo = tr("通信库 版本: V%1.%2, %3年%4月%5日").arg(iLargeVer).arg(iSmallVer).arg(iYear).arg(iMonth).arg(iDay);
    ShowTipInfoFunc(strInfo.toLatin1().data());

    // UI
    strInfo = tr("界面库 版本: V%1.%2, %3年%4月%5日").arg(g_iLargeVer).arg(g_iSmallVer).arg(g_iYear).arg(g_iMonth).arg(g_iDay);
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
    // 隐藏部分控件
    // 暂时不显示“源地址”和“命令字”
    CreateSourceAddr()->hide();
    CreateFrameCmd()->hide();

    // 左
    QVBoxLayout* pLeftVLayout = new QVBoxLayout();
    pLeftVLayout->addLayout(CreateCommSet());
    pLeftVLayout->addStretch();


    QVBoxLayout* pRefStatusLayout = CreateRefStatus();
    if(true == g_oPageCheck.UpdateTab_RefStatus_Use())
    {
        pLeftVLayout->addLayout(pRefStatusLayout);          // 参考输入 状态显示
    }
    else
    {
        m_pRefInput_Group->hide();
    }

    pLeftVLayout->addWidget(CreateClear());

    // 中
    QVBoxLayout* pMidVLayout = new QVBoxLayout();
    pMidVLayout->addLayout(CreateShow());
    pMidVLayout->addWidget(CreateOption());

    // 右
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

// 通信设置
QVBoxLayout* CUpdateTab::CreateCommSet()
{
    QVBoxLayout* pVLayout = new QVBoxLayout();
    pVLayout->addWidget(CreateComInfo());
    pVLayout->addWidget(CreateNetInfo());

    return pVLayout;
}
QGroupBox* CUpdateTab::CreateComInfo()
{
    QGroupBox* pGroup = new QGroupBox(tr("串口设置"));
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

    QLabel* pComNameLabel = new QLabel(tr("串口号"));
    pComNameLabel->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    pComNameLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pComName_ComboBox = new QComboBox();
    m_pComName_ComboBox->setMaximumWidth(SIZE_WIDTH_OF_EDIT);

    QLabel* pComBaudLabel = new QLabel(tr("波特率"));
    pComBaudLabel->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    pComBaudLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pComBaud_ComboBox = new QComboBox();
    m_pComBaud_ComboBox->setMaximumWidth(SIZE_WIDTH_OF_EDIT);

    QLabel* pComDataBitsLabel = new QLabel(tr("数据位"));
    pComDataBitsLabel->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    pComDataBitsLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pComDataBit_ComboBox = new QComboBox();
    m_pComDataBit_ComboBox->setMaximumWidth(SIZE_WIDTH_OF_EDIT);

    QLabel* pComStopBitsLabel = new QLabel(tr("停止位"));
    pComStopBitsLabel->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    pComStopBitsLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pComStopBit_ComboBox = new QComboBox();
    m_pComStopBit_ComboBox->setMaximumWidth(SIZE_WIDTH_OF_EDIT);

    QLabel* pComParityLabel = new QLabel(tr("校验位"));
    pComParityLabel->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    pComParityLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pComCheck_ComboBox = new QComboBox();
    m_pComCheck_ComboBox->setMaximumWidth(SIZE_WIDTH_OF_EDIT);

    m_pComStatus_Label = new QLabel(tr(""));
    m_pComStatus_Label->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    m_pComStatus_Label->setPixmap(QPixmap(":/images/red.png"));
    m_pComStatus_Label->setAlignment(Qt::AlignCenter);

    m_pComOpen_Button = new QPushButton(tr("打开串口"));
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
    QGroupBox* pGroup = new QGroupBox(tr("网口设置"));
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

    QLabel* pNetIPLable = new QLabel(tr("IP地址"));
    pNetIPLable->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    pNetIPLable->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pNetIPAddr_LineEdit = new QLineEdit();
    m_pNetIPAddr_LineEdit->setMaximumWidth(SIZE_WIDTH_OF_EDIT);
    m_pNetIPAddr_LineEdit->setValidator(m_pIP_RegExpVal);

    QLabel* pNetIPPort = new QLabel(tr("IP端口"));
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

    m_pNetOpen_Button = new QPushButton(tr("打开网口"));
    m_pNetOpen_Button->setMaximumWidth(SIZE_WIDTH_OF_EDIT);
    m_pNetOpen_Button->setEnabled(false);
    connect(m_pNetOpen_Button, SIGNAL(clicked()), this, SLOT(OpenCloseNet()));

    connect(m_pNetIPAddr_LineEdit, SIGNAL(editingFinished()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pNetIPAddr_LineEdit, ID_OF_IP_ADDRESS);
    connect(m_pNetIPPort_LineEdit, SIGNAL(editingFinished()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pNetIPPort_LineEdit, ID_OF_IP_PORT);

    // 网口测试
    QLabel* pLabel = new QLabel(tr("  "));
    pLabel->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    pLabel->setAlignment(Qt::AlignCenter);
    m_pNetConnectTest_Button = new QPushButton(tr("重新连接"));
    m_pNetConnectTest_Button->setMaximumWidth(SIZE_WIDTH_OF_EDIT);
    m_pNetConnectTest_Button->setEnabled(false);
    connect(m_pNetConnectTest_Button, SIGNAL(clicked()), this, SLOT(NetConnectTestFunc()));

    char chAddr[1024] = {0};
    CFuncTool::GetUDPAddr(chAddr, sizeof(chAddr));
    m_pNetIPAddr_LineEdit->setText(chAddr);
    m_pNetIPPort_LineEdit->setText(QString::number(CFuncTool::GetUDPPort()));

    // 注意：网口端口暂时固定为 20212
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

// 导出文件
QGroupBox* CUpdateTab::CreateExportGroup()
{
    QGroupBox* pGroup = new QGroupBox(tr("文件保存"));

    m_pExportPath_Button = new QPushButton(tr("路径..."));
    m_pExportFileShow_LineEdit = new QLineEdit();
    m_pExportFileShow_LineEdit->setReadOnly(true);
    m_pAllowExport_CheckBox = new QCheckBox(tr("保存"));
    m_pExportUseTime_CheckBox = new QCheckBox(tr("时间"));

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
    QGroupBox* pGroup = new QGroupBox(tr("源地址"));

    QLabel* pAddrLabel = new QLabel(tr("源地址"));
    pAddrLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pSourceAddr_LineEdit = new QLineEdit();
    m_pSourceAddr_LineEdit->setReadOnly(true);
    m_pSourceAddr_LineEdit->setMaximumWidth(SIZE_WIDTH_OF_EDIT);
    m_pSourceAddr_LineEdit->setValidator(m_p0x16_RegExpVal);

    QLabel* pPortLabel = new QLabel(tr("源端口"));
    pPortLabel->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    m_pSourcePort_LineEdit = new QLineEdit();
    m_pSourcePort_LineEdit->setReadOnly(true);
    m_pSourcePort_LineEdit->setMaximumWidth(SIZE_WIDTH_OF_EDIT);
    m_pSourcePort_LineEdit->setValidator(m_p0x16_RegExpVal);

    QLabel* pRsvLabel = new QLabel(tr("备用一"));
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

    // 初始成0x60
    if(0 == iValue) iValue = 0x60;

    m_pSourceAddr_LineEdit->setText(QString::number(iValue, 16).toUpper());
    iValue = CFuncTool::GetComSvrPort();
    m_pSourcePort_LineEdit->setText(QString::number(iValue, 16).toUpper());
    m_pSourceResv_LineEdit->setText(QString::number(CFuncTool::GetComBackup1(), 16).toUpper());

    // 注意：源地址暂时固定为[0x60, 00]
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
    QGroupBox* pGroup = new QGroupBox(tr("命令字"));

    QLabel* pAddrLabel = new QLabel(tr("帧 号"));
    pAddrLabel->setAlignment(Qt::AlignRight);
    m_pFrameNo_LineEdit = new QLineEdit();
    m_pFrameNo_LineEdit->setMaximumWidth(SIZE_WIDTH_OF_EDIT);
    m_pFrameNo_LineEdit->setValidator(m_p0x16_RegExpVal);

    QLabel* pPortLabel = new QLabel(tr("命令字"));
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

// 参考输入
QVBoxLayout* CUpdateTab::CreateRefStatus()
{
    QGroupBox* pGroup = new QGroupBox(tr("参考输入"));
    m_pRefInput_Group = pGroup;

    // 内频标
    QLabel* pRefStatus_Label = new QLabel(tr(""));
    pRefStatus_Label->setMaximumWidth(SIZE_WIDTH_OF_LABEL);
    pRefStatus_Label->setPixmap(QPixmap(":/images/green.png"));
    pRefStatus_Label->setAlignment(Qt::AlignCenter);

    QLabel* pRefSignalName_Label = new QLabel(tr("内频标10M"));
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

    m_pRecvAll_CheckBox = new QCheckBox(tr("接收所有"));
    m_pRecvAll_CheckBox->setEnabled(false);

    m_pDebug_CheckBox = new QCheckBox(tr("调试"));
    m_pDebug_CheckBox->setEnabled(false);

    m_pClearShow_Button = new QPushButton(tr("清 屏"));
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

    m_pAutoReSend_CheckBox = new QCheckBox(tr("自动重发"));
    connect(m_pAutoReSend_CheckBox, SIGNAL(clicked()), this, SLOT(AutoReSendClicked()));

    QLabel* pTimeLabel = new QLabel(tr("周期"));
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


    m_pAutoMakeFrame_CheckBox = new QCheckBox(tr("自动成帧"));
    connect(m_pAutoMakeFrame_CheckBox, SIGNAL(clicked()), this, SLOT(AutoMakeFrameClicked()));

    m_pSend16Data_CheckBox = new QCheckBox(tr("发16进制"));
    connect(m_pSend16Data_CheckBox, SIGNAL(clicked()), this, SLOT(CheckSend16DataClicked()));

    m_pRecv16Data_CheckBox = new QCheckBox(tr("收16进制"));
    connect(m_pRecv16Data_CheckBox, SIGNAL(clicked()), this, SLOT(CheckRecv16DataClicked()));

    m_pEnter_CheckBox = new QCheckBox(tr("回 车"));
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

    m_pSendData_Button = new QPushButton(tr("发送命令"));
    m_pSendData_Button->setMinimumHeight(50);
    m_pSendData_Button->setMinimumWidth(50);
    m_pSendData_Button->setEnabled(false);

    connect(m_pSendData_Button, SIGNAL(clicked()), this, SLOT(SendData()));

    pHLayout->addWidget(m_pSendData_Button);

    pGroup->setLayout(pHLayout);

    return pGroup;
}

// 软件升级
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
// 板卡选择
QGroupBox* CUpdateTab::CreateBoardSelectGroup()
{
    QGroupBox* pGroup = new QGroupBox(tr("板卡选择"));

    // 单个板卡
    QButtonGroup* pButtonGroup = new QButtonGroup(this);
    m_pSelectAlone_Radio = new QRadioButton(tr("单个板卡"));
    m_pSelectSystem_Radio = new QRadioButton(tr("设备板卡"));
    pButtonGroup->addButton(m_pSelectAlone_Radio, UPDATE_SELECT_IS_ALONE);
    pButtonGroup->addButton(m_pSelectSystem_Radio, UPDATE_SELECT_IS_SYSTEM);
    connect(pButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(BoardTypeChanged(int)));

    QLabel* pLabelAddr = new QLabel(tr("地址"));
    m_pTargetAddr_LineEdit = new QLineEdit();
    m_pTargetAddr_LineEdit->setMaximumWidth(100);
    m_pTargetAddr_LineEdit->setValidator(m_p0x16_RegExpVal);
    QLabel* pLabelPort = new QLabel(tr("端口"));
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

    // 全部板卡
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
    QGroupBox* pGroup = new QGroupBox(tr("功能操作"));              // 原先为：手动升级

    m_pVerQuary_Button = new QPushButton(tr("板卡查询"));           // 原先为：版本查询
    m_pVerQuary_Button->setMinimumWidth(80);
    m_pVerQuary_Button->setMinimumHeight(SIZE_HEIGHT_OF_BUTTON_RIGHT);
    m_pOpenFile_Button = new QPushButton(tr("打开文件..."));
    m_pOpenFile_Button->setMinimumWidth(80);
    m_pOpenFile_Button->setMinimumHeight(SIZE_HEIGHT_OF_BUTTON_RIGHT);
    m_pFileShow_LineEdit = new QLineEdit();
    m_pFileShow_LineEdit->setReadOnly(true);
    m_pFileShow_LineEdit->setMinimumWidth(80);
    m_pFileShow_LineEdit->setMinimumWidth(SIZE_WIDTH_OF_BUTTON_RIGHT);

    QLabel* pTimeOutLabel = new QLabel(tr("超时毫秒"));
    pTimeOutLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    m_pTimeoutMs_LineEdit = new QLineEdit();
    m_pTimeoutMs_LineEdit->setMinimumWidth(SIZE_WIDTH_OF_BUTTON_RIGHT);
    m_pTimeoutMs_LineEdit->setValidator(m_pNum_RegExpVal);
    QLabel* pTimeOutCountLabel = new QLabel(tr("超时次数"));
    pTimeOutCountLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    m_pTimeoutCount_LineEdit = new QLineEdit();
    m_pTimeoutCount_LineEdit->setMinimumWidth(SIZE_WIDTH_OF_BUTTON_RIGHT);
    m_pTimeoutCount_LineEdit->setValidator(m_pNum_RegExpVal);

    connect(m_pTimeoutMs_LineEdit, SIGNAL(editingFinished()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pTimeoutMs_LineEdit, ID_OF_TIMEOUT_MS);
    connect(m_pTimeoutCount_LineEdit, SIGNAL(editingFinished()), m_pSignalMapper, SLOT(map()));
    m_pSignalMapper->setMapping(m_pTimeoutCount_LineEdit, ID_OF_TIMEOUT_COUNT);

    m_iTimeOut_Save = CFuncTool::GetDownloadTimeoutMS();
    m_iReDownCount_Save = 5;//CFuncTool::GetDownloadTimeoutCount(); // 强制为5

    m_pTimeoutMs_LineEdit->setText(QString::number(m_iTimeOut_Save));
    m_pTimeoutCount_LineEdit->setText(QString::number(m_iReDownCount_Save));

    m_iTimeOut_Save = m_iTimeOut_Save / 100;                    // 转换: 2000ms / 100ms = 20

    QHBoxLayout* pHLayoutT0 = new QHBoxLayout();
    pHLayoutT0->addWidget(pTimeOutLabel);
    pHLayoutT0->addWidget(m_pTimeoutMs_LineEdit);
    pHLayoutT0->addStretch();
    QHBoxLayout* pHLayoutT1 = new QHBoxLayout();
    pHLayoutT1->addWidget(pTimeOutCountLabel);
    pHLayoutT1->addWidget(m_pTimeoutCount_LineEdit);
    pHLayoutT1->addStretch();

    m_pStartUpdate_Button = new QPushButton(tr("开始升级"));
    m_pStartUpdate_Button->setMinimumWidth(80);
    m_pStartUpdate_Button->setMinimumHeight(SIZE_HEIGHT_OF_BUTTON_RIGHT);
    m_pStopUpdate_Button = new QPushButton(tr("停止升级"));
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

// 显示区
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


// 设置设备的信号状态
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

// 初始化串口设置
void CUpdateTab::InitComInfo()
{
    // 串口号
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

    // 波特率
    m_pComBaud_ComboBox->addItem("9600");
    m_pComBaud_ComboBox->addItem("14400");
    m_pComBaud_ComboBox->addItem("19200");
    m_pComBaud_ComboBox->addItem("38400");

    m_pComBaud_ComboBox->addItem("56000");
    m_pComBaud_ComboBox->addItem("57600");
    m_pComBaud_ComboBox->addItem("115200");
    m_pComBaud_ComboBox->addItem("128000");
    m_pComBaud_ComboBox->addItem("256000");

    // 数据位
    m_pComDataBit_ComboBox->addItem("5");
    m_pComDataBit_ComboBox->addItem("6");
    m_pComDataBit_ComboBox->addItem("7");
    m_pComDataBit_ComboBox->addItem("8");

    // 停止位
    m_pComStopBit_ComboBox->addItem("1");
    m_pComStopBit_ComboBox->addItem("1.5");
    m_pComStopBit_ComboBox->addItem("2");

    // 校验位
    m_pComCheck_ComboBox->addItem("无");
    m_pComCheck_ComboBox->addItem("奇");
    m_pComCheck_ComboBox->addItem("偶");
    m_pComCheck_ComboBox->addItem("标志");
    m_pComCheck_ComboBox->addItem("空格");
}

// 关闭串口
void CUpdateTab::CloseCom()
{
    m_pMyComBasic->CloseComPort();
    m_pComStatus_Label->setPixmap(QPixmap(":/images/red.png"));
    m_bComIsOpened = false;
    m_pComOpen_Button->setText(tr("打开串口"));
    SetFuncButtonEnable(false);
}

// 关闭网口
void CUpdateTab::CloseNet()
{
    m_pMyComBasic->CloseNetPort();
    m_pNetStatus_Label->setPixmap(QPixmap(":/images/red.png"));
    m_bNetIsOpened = false;
    m_pNetOpen_Button->setText(tr("打开网口"));
    SetFuncButtonEnable(false);
    m_pNetConnectTest_Button->setEnabled(false);
}

bool CUpdateTab::ComNetIsOpen()
{
    return (m_bComIsOpened || m_bNetIsOpened);
}

// 打开/关闭串口
void CUpdateTab::OpenCloseCom()
{
    if(true == m_bComIsOpened)
    {
        CloseCom();
        return;
    }

    // 串口号
    QString strComName = m_pComName_ComboBox->currentText();

    // 波特率
    QString strComBaud = m_pComBaud_ComboBox->currentText();
    int iComBaud = strComBaud.toInt();

    // 数据位
    QString strComDataBit = m_pComDataBit_ComboBox->currentText();
    int iComDataBit = 8;

    // 停止位
    int iComStopBit = 0;

    // 校验位
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
        m_pComOpen_Button->setText(tr("关闭串口"));

        SetFuncButtonEnable(true);

        // INI
        CFuncTool::SetComIndex(m_pComName_ComboBox->currentIndex());
        CFuncTool::SetComBaud(m_pComBaud_ComboBox->currentIndex());

        // 设置源地址(解决一开始就打开连接导致没有源地址的问题)
        SetSourceAddr(GetSourceAddr(), GetSourcePort(), GetSourceUse());

        // 下发命令，检测连接是否成功
        CheckConnect();
    }
    else
    {
        ShowTipInfoFunc("打开串口失败");
    }
}

// 打开/关闭网口
void CUpdateTab::OpenCloseNet()
{
    if(true == m_bNetIsOpened)
    {
        CloseNet();
        return;
    }

    // IP地址
    QString strIPAddr = m_pNetIPAddr_LineEdit->text();
    if(strIPAddr.length() < 7)
    {
        return;
    }

    // IP端口
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
        m_pNetOpen_Button->setText(tr("关闭网口"));

        SetFuncButtonEnable(true);
        m_pNetConnectTest_Button->setEnabled(true);

        // 设置源地址(解决一开始就打开连接导致没有源地址的问题)
        SetSourceAddr(GetSourceAddr(), GetSourcePort(), GetSourceUse());

        // 下发命令，检测连接是否成功
        CheckConnect();
    }
    else
    {
        ShowTipInfoFunc("打开网口失败");
    }
    
}

// 串口改变
void CUpdateTab::ComNameChanged(const QString& text)
{
    if(true == m_bComIsOpened)
    {
        CloseCom();
    }
}

// 保存输入框的内容
void CUpdateTab::SaveInput(int iID)
{
    QString strInput = "";
    int iInput = 0;
    bool bOK = false;
    switch(iID)
    {
    case ID_OF_IP_ADDRESS:          // IP地址
        strInput = m_pNetIPAddr_LineEdit->text();
        CFuncTool::SetUDPAddr(strInput.toLatin1().data());
        break;
    case ID_OF_IP_PORT:             // IP端口
        strInput = m_pNetIPPort_LineEdit->text();
        CFuncTool::SetUDPPort(strInput.toInt());
        break;
    case ID_OF_SOURCE_ADDR:         // 源地址
        strInput = m_pSourceAddr_LineEdit->text();
        m_pSourceAddr_LineEdit->setText(strInput.toUpper());
        iInput = strInput.toInt(&bOK, 16);
        CFuncTool::SetComSvrAddr(iInput);
        break;
    case ID_OF_SOURCE_PORT:         // 源端口
        strInput = m_pSourcePort_LineEdit->text();
        m_pSourcePort_LineEdit->setText(strInput.toUpper());
        iInput = strInput.toInt(&bOK, 16);
        CFuncTool::SetComSvrPort(iInput);
        break;
    case ID_OF_SOURCE_USE:          // 备用1
        strInput = m_pSourceResv_LineEdit->text();
        m_pSourceResv_LineEdit->setText(strInput.toUpper());
        CFuncTool::SetComBackup1(strInput.toInt(&bOK, 16));
        break;
    case ID_OF_TARGET_ADDR:         // 目的地址
        strInput = m_pTargetAddr_LineEdit->text();
        m_pTargetAddr_LineEdit->setText(strInput.toUpper());
        CFuncTool::SetComDestAddr(strInput.toInt(&bOK, 16));
        break;
    case ID_OF_TARGET_PORT:         // 目的端口
        strInput = m_pTargetPort_LineEdit->text();
        m_pTargetPort_LineEdit->setText(strInput.toUpper());
        CFuncTool::SetComDestPort(strInput.toInt(&bOK, 16));
        break;
    case ID_OF_TARGET_USE:          // 备用2
        strInput = m_pTargetResv_LineEdit->text();
        m_pTargetResv_LineEdit->setText(strInput.toUpper());
        CFuncTool::SetComBackup2(strInput.toInt(&bOK, 16));
        break;
    case ID_OF_FRAME_NUM:           // 帧 号
        strInput = m_pFrameNo_LineEdit->text();
        m_pFrameNo_LineEdit->setText(strInput.toUpper());
        CFuncTool::SetComNumber(strInput.toInt(&bOK, 16));
        break;
    case ID_OF_FRAME_COMMAND:       // 命令字
        strInput = m_pFrameCmd_LineEdit->text();
        m_pFrameCmd_LineEdit->setText(strInput.toUpper());
        CFuncTool::SetComCommand(strInput.toInt(&bOK, 16));
        break;
    case ID_OF_RESEND_CIRCLE:       // 自动重发周期
        strInput = m_pTimeReSend_LineEdit->text();
        CFuncTool::SetComCircle(strInput.toInt());
        break;
    case ID_OF_TIMEOUT_MS:          // 超时ms
        strInput = m_pTimeoutMs_LineEdit->text();
        iInput = strInput.toInt();
        m_iTimeOut_Save = iInput;
        CFuncTool::SetDownloadTimeoutMS(iInput);
        m_iTimeOut_Save = m_iTimeOut_Save / 100;                    // 转换: 2000ms / 100ms = 20
        break;
    case ID_OF_TIMEOUT_COUNT:       // 超时次数
        strInput = m_pTimeoutCount_LineEdit->text();
        iInput = strInput.toInt();
        m_iReDownCount_Save = iInput;
        CFuncTool::SetDownloadTimeoutCount(iInput);
        break;
    default:
        break;
    }
}

// 获取 源、目的和命令等
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

// 读取和设置目的地址
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

// 选择网口还是串口
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

// 发 送
void CUpdateTab::SendData()
{
    QString strData = m_pSendText_LineEdit->text();
    strData = strData.trimmed();        // 清两端空格

    // 先检测是否是选项命令
    if("+debug" == strData)         // 调试功能
    {
        m_pMyComBasic->SetShowDebug(true);
        m_pSendText_LineEdit->setText("");
        return;
    }
    if("-debug" == strData)         // 取消 调试功能
    {
        m_pMyComBasic->SetShowDebug(false);
        m_pSendText_LineEdit->setText("");
        return;
    }
    if("opennavtfadmin2015" == strData)             // 开启或者关闭某些功能。都有 open，需要放在前面
    {
        m_bSomeFuncEnabled = !m_bSomeFuncEnabled;
        SetSomeFuncEnabled(m_bSomeFuncEnabled);
        m_pSendText_LineEdit->setText("");
        return;
    }
    if(strData.contains("open"))    // 是否显示某些地址的信息
    {
        g_strListAddrCheck.clear();
        g_strListAddrCheck = strData.split(" ", QString::SkipEmptyParts);
        m_pSendText_LineEdit->setText("");
        return;
    }
    if("stoptip" == strData)        // 屏蔽“所有的板卡无信号输出提示” 输出
    {
        g_bShowBoardTip = false;
        m_pSendText_LineEdit->setText("");
        return;
    }
    

    // 设置输入框的源地址等信息
    SetSourceTargetInfo(COMMAND_IS_AT);

    // 点击“发送”按钮，重新设置数据是否是16进制; 点击升级类按钮，强制设置为16进制
    SetFrameDataStyle(true, true, true, true);

    char chData[2048] = {0};
    strcpy_s(chData, sizeof(chData), strData.toLatin1().data());
    if(true == m_pEnter_CheckBox->isChecked())                      // 回车
    {
        int iLength = strlen(chData);
        chData[iLength] = 0x0D;
        chData[iLength+1] = 0x0A;
        chData[iLength+2] = '\0';
    }

    // 某些功能被屏蔽，目标固定为 [0, 0]
    if(false == m_bSomeFuncEnabled)
    {
        // 设置目的地址
        SetTargetAddr(0, 0, GetTargetUse());
    }

    // 写串口/网口。pszDataBuf仅仅是数据内容，不包括源地址等其他信息
    SendComNetData(0, chData, 0);

    // 保存日志
    CmdLog_Func(GetTargetAddr(), GetTargetPort(), strData);
}

// 选择某个板卡
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
        //ShowTipInfoFunc("获取板卡名称错误");
        return;
    }

    st_BoardVerInfo* pVerInfo = m_BoardVerInfo_map[chName];
    if(NULL == pVerInfo)
    {
        //ShowTipInfoFunc("获取板卡信息结构错误");
        return;
    }

    m_pTargetAddr_LineEdit->setText(QString::number(pVerInfo->m_chAddr, 16));
    m_pTargetPort_LineEdit->setText(QString::number(pVerInfo->m_chPort, 16));

    // ToolTip
    m_pBoardSelect_ComboBox->setToolTip(strBoard);
}

// 发送数据
void CUpdateTab::SendComNetData(unsigned char chCommand, const char* pszDataBuf, int iDataLength_in)
{
    m_pMyComBasic->SendComNetData(chCommand, pszDataBuf, iDataLength_in);
}

// 全部板卡 版本查询
void CUpdateTab::VersionQueryAllBoard()
{
    m_pBoardSelect_ComboBox->setCurrentIndex(0);
    BoardTypeChanged(UPDATE_SELECT_IS_SYSTEM);
    VersionQuery();
}

//  给 参考输入板 和 主控板 发送 getsta 命令
void CUpdateTab::Send_getsta_To_RInputAndMain()
{
    char chData[1024] = {0};

    st_BoardVerInfo* pVerInfo = NULL;
    BOARDVERINFOMAPTYPE::iterator itC = m_BoardVerInfo_map.begin();
    for(; itC != m_BoardVerInfo_map.end(); ++itC)
    {
        pVerInfo = itC->second;
        if(NULL == pVerInfo) continue;
    
        // 参考输入板, 主控板
        if(("Reference_Input" == pVerInfo->m_strOriName) || ("Main_Control" == pVerInfo->m_strOriName))
        {
            SetSourceAddr(GetSourceAddr(), GetSourcePort(), GetSourceUse());
            SetTargetAddr(pVerInfo->m_chAddr, pVerInfo->m_chPort, pVerInfo->m_chUse);
            SetCommand(COMMAND_IS_AT);

            // 点击“发送”按钮，重新设置数据是否是16进制; 点击升级类按钮，强制设置为16进制
            SetFrameDataStyle(true, true, true, true);

            strcpy_s(chData, sizeof(chData), "getsta");
            if(true == m_pEnter_CheckBox->isChecked())                      // 回车
            {
                int iLength = strlen(chData);
                chData[iLength] = 0x0D;
                chData[iLength+1] = '\0';
            }

            SendComNetData(0, chData, 0);
        }
    }
}

// 选择频率输入
void CUpdateTab::SendFreqSelect(int iIndex)
{
    char chData[1024] = {0};

    st_BoardVerInfo* pVerInfo = NULL;
    BOARDVERINFOMAPTYPE::iterator itC = m_BoardVerInfo_map.begin();
    for(; itC != m_BoardVerInfo_map.end(); ++itC)
    {
        pVerInfo = itC->second;
        if(NULL == pVerInfo) continue;

        // 参考输入板, 主控板
        if("Main_Control" == pVerInfo->m_strOriName)
        {
            SetSourceAddr(GetSourceAddr(), GetSourcePort(), GetSourceUse());
            SetTargetAddr(pVerInfo->m_chAddr, pVerInfo->m_chPort, pVerInfo->m_chUse);
            SetCommand(COMMAND_IS_AT);

            // 点击“发送”按钮，重新设置数据是否是16进制; 点击升级类按钮，强制设置为16进制
            SetFrameDataStyle(true, true, true, true);

            if(0 == iIndex) sprintf_s(chData, sizeof(chData), "useclk %d", 0);
            else sprintf_s(chData, sizeof(chData), "useclk %d", 1);
            if(true == m_pEnter_CheckBox->isChecked())                      // 回车
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

                // 点击“发送”按钮，重新设置数据是否是16进制; 点击升级类按钮，强制设置为16进制
                SetFrameDataStyle(true, true, true, true);

                sprintf_s(chData, sizeof(chData), "selclk %d", iIndex);
                if(true == m_pEnter_CheckBox->isChecked())                      // 回车
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

// 选择脉冲输入
void CUpdateTab::SendPlusSelect(int iIndex)
{
    char chData[1024] = {0};

    st_BoardVerInfo* pVerInfo = NULL;
    BOARDVERINFOMAPTYPE::iterator itC = m_BoardVerInfo_map.begin();
    for(; itC != m_BoardVerInfo_map.end(); ++itC)
    {
        pVerInfo = itC->second;
        if(NULL == pVerInfo) continue;

        // 参考输入板
        if("Reference_Input" == pVerInfo->m_strOriName)
        {
            SetSourceAddr(GetSourceAddr(), GetSourcePort(), GetSourceUse());
            SetTargetAddr(pVerInfo->m_chAddr, pVerInfo->m_chPort, pVerInfo->m_chUse);
            SetCommand(COMMAND_IS_AT);

            // 点击“发送”按钮，重新设置数据是否是16进制; 点击升级类按钮，强制设置为16进制
            SetFrameDataStyle(true, true, true, true);

            sprintf_s(chData, sizeof(chData), "selpps %d", iIndex);
            if(true == m_pEnter_CheckBox->isChecked())                      // 回车
            {
                int iLength = strlen(chData);
                chData[iLength] = 0x0D;
                chData[iLength+1] = '\0';
            }

            SendComNetData(0, chData, 0);
        }
    }
}

// 发送 目标地址字符串内容 功能
void CUpdateTab::SendChangeData(unsigned char chAddr, unsigned char chPort, char* pchData)
{
    SetSourceAddr(GetSourceAddr(), GetSourcePort(), GetSourceUse());
    SetTargetAddr(chAddr, chPort, 0);
    SetCommand(COMMAND_IS_AT);

    // 点击“发送”按钮，重新设置数据是否是16进制; 点击升级类按钮，强制设置为16进制
    SetFrameDataStyle(true, true, true, true);

    if(true == m_pEnter_CheckBox->isChecked())                      // 回车
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

// bManualSet=true, 手工选择；否则，由其他变量决定
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

// 设置输入框的源地址等信息
void CUpdateTab::SetSourceTargetInfo(int iCmd)
{
    // 设置源地址
    SetSourceAddr(GetSourceAddr(), GetSourcePort(), GetSourceUse());

    // 设置目的地址
    SetTargetAddr(GetTargetAddr(), GetTargetPort(), GetTargetUse());

    // 设置命令字
    SetCommand(iCmd);
}

// 设置源地址
void CUpdateTab::SetSourceAddr(int iAddr, int iPort, int iResv)
{
    m_pMyComBasic->SetSourceAddr(iAddr, iPort, iResv);
}

// 设置目的地址
void CUpdateTab::SetTargetAddr(int iAddr, int iPort, int iResv)
{
    m_pMyComBasic->SetTargetAddr(iAddr, iPort, iResv);
}

// 设置命令字
void CUpdateTab::SetCommand(int iCmd)
{
    int iFrameNum = GetFrameNo();
    m_pMyComBasic->SetCommand(iFrameNum, iCmd);
    FrameNoInc(iFrameNum);
}

// 重发定时到
void CUpdateTab::ReSendTimeReach()
{
    SendData();
}

// 清 屏
void CUpdateTab::ClearShowInfo()
{
    m_pRecvShow_TextEdit->clear();
    m_pTipShow_TextEdit->clear();
}

// 显示接收数据
void CUpdateTab::ShowRecvDataFunc(unsigned char chAddr, unsigned char chPort, const char* pszShowData, bool bChanged, int iR, int iG, int iB)
{
    QString strInfo = pszShowData;
    emit ShowRecvText_Signal(chAddr, chPort, strInfo, bChanged, iR, iG, iB);
}

// 显示发送数据
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

// 显示提示信息
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
    // 过滤
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
    

    // 增加 “--> ”显示头
    QString strAddInfo = "";

    // 导出
    if((true == g_oPageCheck.UpdateTab_ExportFile_Use()) && (m_pAllowExport_CheckBox->isChecked()))
    {
        // 是否增加时间
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


    // 源地址：地址相同则不添加
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
    // 增加 "<-- " 显示头部
    QString strEnter = tr("\n");
    //if(true == g_oPageCheck.IsAdmin())
    if(true)                                // 所有的权限都显示
    {
        // 是升级则直接显示；否则将16进制转成字符显示
        char chDataAddr[16] = {0};
        char chDataPort[16] = {0};
        sprintf_s(chDataAddr, sizeof(chDataAddr), "%02X", chAddr);
        sprintf_s(chDataPort, sizeof(chDataPort), "%02X", chPort);
        QString strAddInfo = tr("\n[%1,%2]").arg(chDataAddr).arg(chDataPort) + "<-- ";

        if((NULL != m_pCmdItem_Cur) && (en_Cmd_A_Is_Update == m_pCmdItem_Cur->m_enFrameAType))  // 升级
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

        // 同时在Recv显示(拷贝上面ShowRecvText_Slot的功能)
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
            strReadedName = "未知板卡";
            continue;                       // 忽略
        }

        // 参考输入板_Ref_Input_P0102_Vx.y
        sprintf_s(chName, sizeof(chName), "%s_%s_V%0.1f", strReadedName.c_str(), pVerInfo->m_strName.c_str(), pVerInfo->m_fVer);
        m_pBoardSelect_ComboBox->addItem(chName);

        // 更新 GNSS 板卡的目的地址
        if(strcmp(pVerInfo->m_strOriName.c_str(), "Reference_GNSS") == 0)
        {
            g_strGNSSAddr = QString::number(pVerInfo->m_chAddr, 16);
            g_strGNSSPort = QString::number(pVerInfo->m_chPort, 16);
        }
    }

    // 自适应宽度 ?

}

// 命令结束
void CUpdateTab::CmdFinished_Slot()
{
    SetAllEnabled(true);

    // My Image Tab
    if(NULL != m_pMyImageTab)
    {
        m_pMyImageTab->CmdFinished_Func();
    }
}

// 命令结束
void CUpdateTab::CmdFinished_Func()
{
    emit CmdFinished_Signal();
}

// 执行命令时禁止其他操作
void CUpdateTab::SetAllEnabled(bool bEnable)
{
    if(false == bEnable)
    {
        // 通信设置
        m_pComOpen_Button->setEnabled(false);
        m_pNetOpen_Button->setEnabled(false);

        // 发送
        m_pAutoReSend_CheckBox->setEnabled(false);
        m_pSendData_Button->setEnabled(false);

        // 升级
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
        // 通信设置
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

// 停止 自动重发
void CUpdateTab::StopAutoReSend()
{
    m_pTimer->stop();
    m_pAutoReSend_CheckBox->setChecked(false);
}

// 选项：自动重发、自动成帧、16进制、调试
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

        // 给出提示(其他场合使用会不停的提示，所以屏蔽)
        //ShowTipInfoFunc("注意：与设备通信需要勾选 \"自动成帧\" ");
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

// 发送是否追加“回车”
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

// 关闭窗口时调用的函数
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

    // 任务退出
    if(true == m_bComIsOpened) m_pMyComBasic->CloseComPort();
    if(true == m_bNetIsOpened) m_pMyComBasic->CloseNetPort();
    if((false == m_bComIsOpened) && (false == m_bNetIsOpened)) m_pMyComBasic->CloseComPort();

    // 等待任务结束
    volatile bool bThreadWorking = m_pMyComBasic->TaskIsRunning();
    while(true == bThreadWorking)
    {
        bThreadWorking = m_pMyComBasic->TaskIsRunning();
    }
}

// 打开文件
void CUpdateTab::OpenFilePath()
{
    QFileDialog::Options options;
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(this,
                                        tr("打开文件"),
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

// 获取路径的文件名称
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

// 版本查询
void CUpdateTab::VersionQuery()
{
    // 点击“发送”按钮，重新设置数据是否是16进制; 点击升级类按钮，强制设置为16进制
    SetFrameDataStyle(false, false, false, true);
    DestroyAllCmd();

    m_enOpType = en_Op_Is_Ver_One;

    // 修改“停止升级”名称
    m_pStopUpdate_Button->setText(tr("停止查询"));

    // 1. "独立单元"：取输入框的地址，仅查询 1 次
    // 2. "全部"：取固定的地址，每个地址查询 1 次，总共约 15 次
    // 3. 其他：根据字符串信息查询，仅 1 次

    QString strText = m_pBoardSelect_ComboBox->currentText();
    if(m_pSelectAlone_Radio->isChecked()) strText = NAME_OF_VERSION_ONLYONE;
    if(NAME_OF_VERSION_ONLYONE == strText)              // "独立单元"
    {
        st_CommandItem* pCmdItem = new st_CommandItem(en_Cmd_A_Is_Version, en_Cmd_B_Ver_Is_One);
        AddOneCmd(pCmdItem);

        pCmdItem->m_chSourceAddr = GetSourceAddr();     // 源、目的和命令
        pCmdItem->m_chSourcePort = GetSourcePort();
        pCmdItem->m_chSourceUse = GetSourceUse();
        pCmdItem->m_chTargetAddr = GetTargetAddr();
        pCmdItem->m_chTargetPort = GetTargetPort();
        pCmdItem->m_chTargetUse = GetTargetUse();
        pCmdItem->m_chFrameCmd = COMMAND_IS_AT;
        pCmdItem->m_bManual = true;
    }
    else if(NAME_OF_VERSION_ALL == strText)             // "全部"
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

            pCmdItem->m_chSourceAddr = GetSourceAddr();   // 源、目的和命令
            pCmdItem->m_chSourcePort = GetSourcePort();
            pCmdItem->m_chSourceUse = GetSourceUse();
            pCmdItem->m_chTargetAddr = g_BoardAddr[i][0];
            pCmdItem->m_chTargetPort = g_BoardAddr[i][1];
            pCmdItem->m_chTargetUse = GetTargetUse();
            pCmdItem->m_chFrameCmd = COMMAND_IS_AT;
            pCmdItem->m_bManual = true;
        }

        // 刷新命令: 命令结束后发送获取连接状态等命令
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
            ShowTipInfoFunc("获取板卡名称错误");
            return;
        }

        st_BoardVerInfo* pVerInfo = m_BoardVerInfo_map[chName];
        if(NULL == pVerInfo)
        {
            ShowTipInfoFunc("获取板卡信息结构错误");
            return;
        }
        
        st_CommandItem* pCmdItem = new st_CommandItem(en_Cmd_A_Is_Version, en_Cmd_B_Ver_Is_One);
        AddOneCmd(pCmdItem);

        pCmdItem->m_chSourceAddr = GetSourceAddr();     // 源、目的和命令
        pCmdItem->m_chSourcePort = GetSourcePort();
        pCmdItem->m_chSourceUse = GetSourceUse();
        pCmdItem->m_chTargetAddr = pVerInfo->m_chAddr;
        pCmdItem->m_chTargetPort = pVerInfo->m_chPort;
        pCmdItem->m_chTargetUse = pVerInfo->m_chUse;
        pCmdItem->m_chFrameCmd = COMMAND_IS_AT;
        pCmdItem->m_bManual = true;
    }

    GetFirstCmd();

    // 发送当前命令头包
    DownloadSendCurCmdFirstPack();
    m_iTimeOut = 0;
    m_bStopDownload = false;
    SetAllEnabled(false);

    ShowTipInfoFunc("发送 读取版本 命令");
}

// 开始升级
void CUpdateTab::StartUpdate()
{
    // 1. "独立单元"：取输入框的地址，取手动打开的文件，强制升级，升级 1 次
    // 2. "全部"：忽略
    // 3. 选择某个板卡：根据该板卡的信息决定，版本相同则不升级，升级 1次
    
    // 修改“停止升级”名称
    m_pStopUpdate_Button->setText(tr("停止升级"));

    QString strText = m_pBoardSelect_ComboBox->currentText();
    if(m_pSelectAlone_Radio->isChecked()) strText = NAME_OF_VERSION_ONLYONE;
    if(NAME_OF_VERSION_ALL == strText)             // "全部"
    {
        AutoUpdate();
        return;
    }

    // 点击“发送”按钮，重新设置数据是否是16进制; 点击升级类按钮，强制设置为16进制
    SetFrameDataStyle(false, true, true, true);
    DestroyAllCmd();

    m_enOpType = en_Op_Is_Update_One;
    
    if(NAME_OF_VERSION_ONLYONE == strText)              // "独立单元"
    {
        // 如果是内部调试版本，增加以下内容：login(回车)  123212(回车)  inboot(回车)
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

        SetFrameDataStyle(false, true, true, true); // 恢复

        st_CommandItem* pCmdItem = new st_CommandItem(en_Cmd_A_Is_Update, en_Cmd_B_Down_Is_One);
        AddOneCmd(pCmdItem);

        pCmdItem->m_chSourceAddr = GetSourceAddr();     // 源、目的和命令
        pCmdItem->m_chSourcePort = GetSourcePort();
        pCmdItem->m_chSourceUse = GetSourceUse();
        pCmdItem->m_chTargetAddr = GetTargetAddr();
        pCmdItem->m_chTargetPort = GetTargetPort();
        pCmdItem->m_chTargetUse = GetTargetUse();
        pCmdItem->m_bManual = true;
        pCmdItem->m_strFilePath = m_strFile;            // 使用手动打开文件
        errno_t err = fopen_s(&(pCmdItem->m_pFile), (pCmdItem->m_strFilePath).toLatin1(), "rb");
        if(0 != err)
        {
            pCmdItem->m_pFile = NULL;
            ShowTipInfoFunc("打开文件失败");
            return;
        }
    }
    else
    {
        char chName[1024] = {0};
        if(false == GetBoardNameOnComBox(strText.toLatin1(), chName, sizeof(chName)))
        {
            ShowTipInfoFunc("获取板卡名称错误");
            return;
        }

        st_BoardVerInfo* pVerInfo = m_BoardVerInfo_map[chName];
        if(NULL == pVerInfo)
        {
            ShowTipInfoFunc("获取板卡信息结构错误");
            return;
        }

        // 1. 查找文件，根据文件得出文件名的版本信息
        // 2. 版本相同或者找不到文件，提示退出
        float fVer_here = 0.0f;
        bool bRes = CFuncTool::GetVerOnDirAndShortName(m_strAutoPath.toLatin1(), pVerInfo->m_strOriName.c_str(), fVer_here);
        if((false == bRes))
        {
            ShowTipInfoFunc("找不到文件, 退出");
            return;
        }
        else if((fVer_here == pVerInfo->m_fVer))
        {
            ShowTipInfoFunc("版本相同, 退出");
            return;
        }
        else
        {
            st_CommandItem* pCmdItem = new st_CommandItem(en_Cmd_A_Is_Update, en_Cmd_B_Down_Is_One);
            

            // 使用固定文件 Ref_Input_Vx.y.bin
            sprintf_s(chName, sizeof(chName), "%s%s_V%0.1f.bin", m_strAutoPath.toLatin1().data(), pVerInfo->m_strOriName.c_str(), fVer_here);
            pCmdItem->m_strFilePath = chName;
            errno_t err = fopen_s(&(pCmdItem->m_pFile), (pCmdItem->m_strFilePath).toLatin1(), "rb");
            if(0 != err)
            {
                pCmdItem->m_pFile = NULL;
                ShowTipInfoFunc("打开文件失败, 退出");
                delete pCmdItem;
                return;
            }
            
            AddOneCmd(pCmdItem);
            pCmdItem->m_chSourceAddr = GetSourceAddr();     // 源、目的和命令
            pCmdItem->m_chSourcePort = GetSourcePort();
            pCmdItem->m_chSourceUse = GetSourceUse();
            pCmdItem->m_chTargetAddr = pVerInfo->m_chAddr;
            pCmdItem->m_chTargetPort = pVerInfo->m_chPort;
            pCmdItem->m_chTargetUse = pVerInfo->m_chUse;
            pCmdItem->m_bManual = true;
        }
    }

    GetFirstCmd();

    // 发送当前命令头包
    DownloadSendCurCmdFirstPack();
    m_iTimeOut = 0;
    m_bStopDownload = false;
    SetAllEnabled(false);

    // 保存日志
    CmdLog_Func(GetTargetAddr(), GetTargetPort(), tr("Update One"));
}

// 停止升级(查询)
void CUpdateTab::StopUpdate()
{
    if(NULL == m_pCmdItem_Cur) return;

    m_bStopDownload = true;

    // 刷新命令: 清命令
    if(NULL != m_pMyImageTab)
    {
        m_pMyImageTab->CmdClear();
    }
}

// 自动升级
void CUpdateTab::AutoUpdate()
{
    // 需要先获取板卡
    if(m_pBoardSelect_ComboBox->count() < 2)
    {
        ShowTipInfoFunc("请先查询 全部板卡");
        return;
    }

    // 点击“发送”按钮，重新设置数据是否是16进制; 点击升级类按钮，强制设置为16进制
    SetFrameDataStyle(false, true, true, true);
    DestroyAllCmd();
    SetAllEnabled(false);

    ShowTipInfoFunc("开始 自动升级...");
    m_enOpType = en_Op_Is_Auto_Download;

    // 修改“停止升级”名称
    m_pStopUpdate_Button->setText(tr("停止升级"));

    // 清 报告
    char* ptr = NULL;
    for(int i = 0; i < NUM_OF_AUTODOWN_REPORT; ++i)
    {
        ptr = m_pAutoDownloadReport[i];
        memset(ptr, 0, SIZE_OF_AUTODOWN_REPORT);
    }

    // 组合 15 对命令，每对为版本查询+升级下载；如返回版本和本地升级文件的版本号一致，则不升级。
    // 为了避免用户必须点击查询全部命令，此处从头开始查询15次
    int iMaxCount = COUNT_OF_AUTODOWN_CMD;
    int iReport = 0;
    for(int i = 0; i < iMaxCount; ++i)
    {
        // 如果这块板卡没有查询到，跳过
        if(false == BoardIsValid(g_BoardAddr[i][0], g_BoardAddr[i][1])) continue;

        // 版本
        st_CommandItem* pCmdItem = new st_CommandItem(en_Cmd_A_Is_Version, en_Cmd_B_Ver_Is_More);
        AddOneCmd(pCmdItem);

        pCmdItem->m_chSourceAddr = GetSourceAddr();   // 源、目的和命令
        pCmdItem->m_chSourcePort = GetSourcePort();
        pCmdItem->m_chSourceUse = GetSourceUse();
        pCmdItem->m_chTargetAddr = g_BoardAddr[i][0];
        pCmdItem->m_chTargetPort = g_BoardAddr[i][1];
        pCmdItem->m_chTargetUse = GetTargetUse();
        pCmdItem->m_chFrameCmd = COMMAND_IS_AT;
        pCmdItem->m_bManual = false;

        ptr = m_pAutoDownloadReport[iReport++];
        pCmdItem->m_pReportBuf = ptr;
        sprintf_s(ptr, SIZE_OF_AUTODOWN_REPORT, "查询版本: 板卡地址[%02X, %02X](%s) 无响应", 
                                                 pCmdItem->m_chTargetAddr, pCmdItem->m_chTargetPort,
                                                 GetBoardNameOnAddr(pCmdItem->m_chTargetAddr, pCmdItem->m_chTargetPort).toLatin1().data());

        // 升级下载
        pCmdItem = new st_CommandItem(en_Cmd_A_Is_Update, en_Cmd_B_Down_Is_More);
        AddOneCmd(pCmdItem);

        pCmdItem->m_chSourceAddr = GetSourceAddr();     // 源、目的和命令
        pCmdItem->m_chSourcePort = GetSourcePort();
        pCmdItem->m_chSourceUse = GetSourceUse();
        pCmdItem->m_chTargetAddr = g_BoardAddr[i][0];
        pCmdItem->m_chTargetPort = g_BoardAddr[i][1];
        pCmdItem->m_chTargetUse = GetTargetUse();
        pCmdItem->m_bManual = false;
        
        // 暂时没有文件名，由查询结果得出
        pCmdItem->m_strFilePath = "";

        ptr = m_pAutoDownloadReport[iReport++];
        pCmdItem->m_pReportBuf = ptr;
        sprintf_s(ptr, SIZE_OF_AUTODOWN_REPORT, "下载文件: 板卡地址[%02X, %02X](%s) 无响应\n", 
                                                pCmdItem->m_chTargetAddr, pCmdItem->m_chTargetPort,
                                                GetBoardNameOnAddr(pCmdItem->m_chTargetAddr, pCmdItem->m_chTargetPort).toLatin1().data());

        ptr = m_pAutoUpdateSimpleReport[i];
        pCmdItem->m_pSimpleReportBuf = ptr;
        sprintf_s(ptr, SIZE_OF_AUTODOWN_REPORT, "下载文件: 板卡地址[%02X, %02X](%s) 失败", 
                                                pCmdItem->m_chTargetAddr, pCmdItem->m_chTargetPort,
                                                GetBoardNameOnAddr(pCmdItem->m_chTargetAddr, pCmdItem->m_chTargetPort).toLatin1().data());

        // 延时
        pCmdItem = new st_CommandItem(en_Cmd_A_Is_Delay, en_Cmd_B_Delay_Is_More);
        AddOneCmd(pCmdItem);

        pCmdItem->m_iDelayCount = 5;            // 5s
    }

    GetFirstCmd();

    // 发送当前命令头包
    DownloadSendCurCmdFirstPack();
    m_iTimeOut = 0;
    m_bStopDownload = false;

    // 保存日志
    CmdLog_Func(GetTargetAddr(), GetTargetPort(), tr("Update More"));
}

// 发送当前命令头包
void CUpdateTab::DownloadSendCurCmdFirstPack()
{
    if(NULL == m_pCmdItem_Cur)
    {
        ExitDownload();
        return;
    }

    if(en_Cmd_A_Is_Version == m_pCmdItem_Cur->m_enFrameAType)
    {
        // 设置源地址
        m_pMyComBasic->SetSourceAddr(m_pCmdItem_Cur->m_chSourceAddr, m_pCmdItem_Cur->m_chSourcePort, m_pCmdItem_Cur->m_chSourceUse);

        // 设置目的地址
        m_pMyComBasic->SetTargetAddr(m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort, m_pCmdItem_Cur->m_chTargetUse);

        // 设置命令字
        int iFrameNum = GetFrameNo();
        m_pMyComBasic->SetCommand(iFrameNum, m_pCmdItem_Cur->m_chFrameCmd);

        FrameNoInc(iFrameNum);

        // 发送
        SendOneFrame(m_pCmdItem_Cur->m_chFrameCmd, m_pCmdItem_Cur->m_chATCmd, sizeof(m_pCmdItem_Cur->m_chATCmd));

        // 显示信息
        char chInfo[1024] = {0};
        sprintf_s(chInfo, sizeof(chInfo), "查询目标板卡[%02X, %02X]版本", m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort);
        ShowTipInfoFunc(chInfo);
    }
    else
    {
        // 发送 准备升级 命令
        SendPrepareUpdate();

        // 是否是加密
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

// 设置按钮状态(针对端口是否打开)
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

// 显示下载文件的内容
void CUpdateTab::ShowUpdateFile()
{
    if(m_strFile.length() < 4)
    {
        ShowTipInfoFunc("文件路径不正确");
        return;
    }

    FILE* pFile = NULL;
    errno_t err = fopen_s(&pFile, m_strFile.toLatin1(), "rb");
    if(0 != err)
    {
        ShowTipInfoFunc("打开文件失败");
        return;
    }

    // 显示
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
            if(0 == iShowLineCount)     // 开头
            {
                Change16ToAscii8Bits(iShowAllCount, strOne);
                strShow += strOne;
            }

            chOne = chData[i];
            CFuncTool::Change16ToAscii(chOne, chOneBuf, sizeof(chOneBuf));
            strOne = (char*)chOneBuf;
            strShow += strOne;

            if(0x0F == iShowLineCount)  // 结尾
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

// 升级下载超时函数
void CUpdateTab::DownloadTimeoutFunc()
{
    ++m_iTimeOut;
    if(m_iTimeOut > m_iTimeOut_Save)
    {
        m_iTimeOut = 0;

        if(NULL != m_pCmdItem_Cur)          // 当前命令超时
        {
            m_pCmdItem_Cur->m_bTimeOut = true;
            HandleFrameData();
            return;
        }
    }
}

// 帧数据处理函数
void CUpdateTab::FrameHanleFunc(const st_FrameData* pFrameData)
{
    // 数据传给全局变量1
    ((st_FrameData* )pFrameData)->Clone(&g_FrameData_1);
    HandleFrameData();
}

// 帧数据处理线程调用函数
void CUpdateTab::HandleFrameData()
{
    // 1. 是否是超时
    // 2. 数据是否有效
    // 3. 当前命令是什么类型
    // 4. 是否是手动
    // 5. 是否强制停止
    // 6. 注意：因为有可能收到的不是希望的内容，所以更新下一条命令的依据是：1）收到正确的响应，2）超时

    if(true == m_bStopDownload)
    {
        ExitDownload();
        CmdFinished_Func();
        m_pCmdItem_Cur = NULL;
        m_bStopDownload = false;
        m_iTimeOut = 0;
        ShowTipInfoFunc("用户强制停止了当前任务");
        return;
    }

    // 从全局变量1拷贝数据到全局变量2
    g_FrameData_1.Clone(&g_FrameData_2);
    m_iMultiFrame = g_FrameData_2.m_chTargetUse;        // 多帧指示

    // GNSS 命令包
    if(COMMAND_IS_XXB2 == g_FrameData_2.m_chFrameCmd)
    {
        if(NULL != m_pGNSSTab)
        {
            m_pGNSSTab->HandleFrameData(g_FrameData_2.m_chFrameCmd, g_FrameData_2.m_pFrameDataBuf, 
                                g_FrameData_2.m_iFrameDataLength, g_FrameData_2.m_chSourceAddr, g_FrameData_2.m_chSourcePort);
        }
        return;
    }

    if(NULL != m_pCmdItem_Cur)              // 有当前命令
    {
        bool bCmdFinish = false;            // 命令是否已经执行完毕
        if(en_Cmd_A_Is_Version == m_pCmdItem_Cur->m_enFrameAType)   // 版本查询(当前)
        {
            if(en_Cmd_B_Ver_Is_One == m_pCmdItem_Cur->m_enFrameBType)
            {
                if(true == m_pCmdItem_Cur->m_bTimeOut)                  // 超时
                {
                    m_pCmdItem_Cur->m_bTimeOut = false;

                    if(m_pCmdItem_Cur->m_iReDownCount >= m_iReDownCount_Save)
                    {
                        bCmdFinish = true;
                        m_pCmdItem_Cur->m_iReDownCount = 0;
                        ShowTipInfoFunc("重发完毕");
                    }
                    else
                    {
                        char chTip[1024] = {0};
                        sprintf_s(chTip, sizeof(chTip), "MCU 响应超时，重发次数[%d]", m_pCmdItem_Cur->m_iReDownCount+1);
                        ShowTipInfoFunc(chTip);

                        SendOneFrame(0, NULL, 0, true);     // 重发本帧
                    }
                }
                else
                {
                    if(false == g_FrameData_2.m_bDataIsOK)              // 数据不正确
                    {
                    }
                    else
                    {
                        char chName[1024] = {0};
                        float fVer = 0.0f;
                        bool bGet = GetBoardNameAndVerOnInput(g_FrameData_2.m_pFrameDataBuf, chName, sizeof(chName), fVer);
                        if(true == m_pCmdItem_Cur->m_bManual)           // 手动
                        {
                            if(true == bGet)
                            {
                                m_iTimeOut = 0;
                                bCmdFinish = true;
                            }
                        }
                        else                                            // 自动
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
                if(true == m_pCmdItem_Cur->m_bTimeOut)                  // 超时
                {
                    m_pCmdItem_Cur->m_bTimeOut = false;
                    if(m_pCmdItem_Cur->m_iReDownCount >= m_iReDownCount_Save)
                    {
                        bCmdFinish = true;
                        m_pCmdItem_Cur->m_iReDownCount = 0;
                        ShowTipInfoFunc("重发完毕");

                        if(false == m_pCmdItem_Cur->m_bManual)              // 自动
                        {
                            // 应该是自动升级，往下2步。后面会继续下一步
                            GetNextCmd();
                            GetNextCmd();
                            ShowTipInfoFunc("查询版本超时");
                        }
                    }
                    else
                    {
                        char chTip[1024] = {0};
                        sprintf_s(chTip, sizeof(chTip), "MCU 响应超时，重发次数[%d]", m_pCmdItem_Cur->m_iReDownCount+1);
                        ShowTipInfoFunc(chTip);

                        SendOneFrame(0, NULL, 0, true);     // 重发本帧
                    }
                }
                else
                {
                    if(false == g_FrameData_2.m_bDataIsOK)              // 数据不正确
                    {
                    }
                    else
                    {
                        char chName[1024] = {0};
                        float fVer = 0.0f;
                        bool bGet = GetBoardNameAndVerOnInput(g_FrameData_2.m_pFrameDataBuf, chName, sizeof(chName), fVer);
                        if(true == m_pCmdItem_Cur->m_bManual)           // 手动
                        {
                            if(true == bGet)
                            {
                                AddOneBoard(chName, sizeof(chName), g_FrameData_2.m_chSourceAddr, g_FrameData_2.m_chSourcePort, fVer);
                                bCmdFinish = true;          // 响应正确，此命令处理完毕，发送下一条命令
                                m_iTimeOut = 0;
                            }
                        }
                        else                                            // 自动(应该是自动升级)
                        {
                            if(true == bGet)
                            {
                                // 使用固定文件 Ref_Input_Vx.y.bin。注意：版本相同则不升级
                                bCmdFinish = true;          // 响应正确，此命令处理完毕，发送下一条命令
                                m_iTimeOut = 0;
                                float fVer_here = 0.0f;
                                st_CommandItem* pCmdItemHereUse = m_pCmdItem_Cur;
                                bool bRes = CFuncTool::GetVerOnDirAndShortName(m_strAutoPath.toLatin1(), chName, fVer_here);
                                if((false == bRes) || (fVer_here == fVer))
                                {
                                    // 这里只需往下2步，后面会继续往下一步
                                    GetNextCmd();
                                    GetNextCmd();
                                    if((false == bRes))
                                    {
                                        char chFilePath[1024] = {0};
                                        sprintf_s(chFilePath, sizeof(chFilePath), "查找文件失败: %s%s_V%0.1f.bin", m_strAutoPath.toLatin1().data(), chName, fVer_here);
                                        ShowTipInfoFunc(chFilePath);

                                        // 报告
                                        char* ptr = pCmdItemHereUse->m_pReportBuf;
                                        if((en_Op_Is_Auto_Download == m_enOpType) && (ptr != NULL))
                                        {
                                            sprintf_s(ptr, SIZE_OF_AUTODOWN_REPORT, "查询版本: 板卡地址[%02X, %02X](%s), %s", 
                                                      pCmdItemHereUse->m_chTargetAddr, pCmdItemHereUse->m_chTargetPort, 
                                                      GetBoardNameOnAddr(pCmdItemHereUse->m_chTargetAddr, pCmdItemHereUse->m_chTargetPort).toLatin1().data(),
                                                      chFilePath);
                                        }
                                    }
                                    else if((fVer_here == fVer))
                                    {
                                        ShowTipInfoFunc("版本相同");

                                        // 报告
                                        char* ptr = pCmdItemHereUse->m_pReportBuf;
                                        if((en_Op_Is_Auto_Download == m_enOpType) && (ptr != NULL))
                                        {
                                            sprintf_s(ptr, SIZE_OF_AUTODOWN_REPORT, "查询版本: 板卡地址[%02X, %02X](%s), 版本相同[%0.1f], 不升级", 
                                                        pCmdItemHereUse->m_chTargetAddr, pCmdItemHereUse->m_chTargetPort, 
                                                        GetBoardNameOnAddr(pCmdItemHereUse->m_chTargetAddr, pCmdItemHereUse->m_chTargetPort).toLatin1().data(),
                                                        fVer);
                                        }
                                    }

                                    // 简略报告
                                    pCmdItemHereUse = pCmdItemHereUse->m_next;
                                    if((NULL != pCmdItemHereUse) && (en_Cmd_A_Is_Update == pCmdItemHereUse->m_enFrameAType))
                                    {
                                        char* ptr = pCmdItemHereUse->m_pSimpleReportBuf;
                                        if(NULL != ptr)
                                        {
                                            sprintf_s(ptr, SIZE_OF_AUTODOWN_REPORT, "查询版本: 板卡地址[%02X, %02X](%s), 查询失败", 
                                                                pCmdItemHereUse->m_chTargetAddr, pCmdItemHereUse->m_chTargetPort,
                                                                GetBoardNameOnAddr(pCmdItemHereUse->m_chTargetAddr, pCmdItemHereUse->m_chTargetPort).toLatin1().data());
                                        }
                                    }
                                }
                                else
                                {
                                    // 将查询到的结果保存到下一个命令的文件路径
                                    st_CommandItem* pCmdItemHere = m_pCmdItem_Cur;
                                    if(NULL != pCmdItemHere)
                                    {
                                        pCmdItemHere = pCmdItemHere->m_next;
                                        if((NULL != pCmdItemHere) && (en_Cmd_A_Is_Update == pCmdItemHere->m_enFrameAType))
                                        {
                                            char chFilePath[1024] = {0};
                                            sprintf_s(chFilePath, sizeof(chFilePath), "%s%s_V%0.1f.bin", m_strAutoPath.toLatin1().data(), chName, fVer_here);
                                            pCmdItemHere->m_strFilePath = chFilePath;

                                            // 报告
                                            char* ptr = m_pCmdItem_Cur->m_pReportBuf;
                                            if((en_Op_Is_Auto_Download == m_enOpType) && (ptr != NULL))
                                            {
                                                sprintf_s(ptr, SIZE_OF_AUTODOWN_REPORT, "查询版本: 板卡地址[%02X, %02X](%s), 版本[%0.1f], 文件[%s], 查询成功", 
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
                if(true == m_pCmdItem_Cur->m_bTimeOut)                  // 超时
                {
                    m_pCmdItem_Cur->m_bTimeOut = false;

                    if(m_pCmdItem_Cur->m_iReDownCount >= m_iReDownCount_Save)
                    {
                        bCmdFinish = true;
                        m_pCmdItem_Cur->m_iReDownCount = 0;
                        ShowTipInfoFunc("重发完毕");
                    }
                    else
                    {
                        char chTip[1024] = {0};
                        sprintf_s(chTip, sizeof(chTip), "MCU 响应超时，重发次数[%d]", m_pCmdItem_Cur->m_iReDownCount+1);
                        ShowTipInfoFunc(chTip);

                        SendOneFrame(0, NULL, 0, true);     // 重发本帧
                    }
                }
                else
                {
                    if(false == g_FrameData_2.m_bDataIsOK)              // 数据不正确
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
        else if(en_Cmd_A_Is_Update == m_pCmdItem_Cur->m_enFrameAType)   // 升级下载(当前)
        {
            if(en_Cmd_B_Down_Is_One == m_pCmdItem_Cur->m_enFrameBType)
            {
                if(true == m_pCmdItem_Cur->m_bTimeOut)                  // 超时
                {
                    m_pCmdItem_Cur->m_bTimeOut = false;

                    if(true == m_bSingleDebug)
                    {
                        m_iSingleNum = 2;
                        if(0 == m_pCmdItem_Cur->m_iReDownCount)
                        {
                            ++m_pCmdItem_Cur->m_iReDownCount;
                            ShowTipInfoFunc("超时，单击重发本帧");
                        }
                    }
                    else
                    {
                        if(m_pCmdItem_Cur->m_iReDownCount >= m_iReDownCount_Save)
                        {
                            m_pCmdItem_Cur->m_iReDownCount = 0;
                            ExitDownload();
                            ShowTipInfoFunc("重发完毕，退出下载");
                        }
                        else
                        {
                            char chTip[1024] = {0};
                            sprintf_s(chTip, sizeof(chTip), "MCU 响应超时，重发本帧[%d], 重发次数[%d]", m_pCmdItem_Cur->m_iPaksIndex+1, m_pCmdItem_Cur->m_iReDownCount+1);
                            ShowTipInfoFunc(chTip);

                            SendOneFrame(0, NULL, 0, true);     // 重发本帧
                        }
                    }
                }
                else
                {
                    if(false == g_FrameData_2.m_bDataIsOK)              // 数据不正确
                    {
                        //ShowTipInfoFunc("数据不正确");
                        //return;
                    }
                    else
                    {
                        unsigned char chCommand = g_FrameData_2.m_chFrameCmd;
                        unsigned char chHighPak = (g_FrameData_2.m_pFrameDataBuf)[0];     // 回复的包号
                        unsigned char chLowPak = (g_FrameData_2.m_pFrameDataBuf)[1];
                        unsigned char chParam = (g_FrameData_2.m_pFrameDataBuf)[2];       // 回复的参数

                        if(true == m_pCmdItem_Cur->m_bManual)           // 手动
                        {
                            HandleRecvCommand(chCommand, chHighPak, chLowPak, chParam);
                        }
                        else                                            // 自动
                        {
                            ;
                        }
                    }
                }

                // 只处理一个命令，所以如果下一个命令为空，则结束本次命令
                if(NULL == m_pCmdItem_Cur)
                {
                    // 点击“发送”按钮，重新设置数据是否是16进制; 点击升级类按钮，强制设置为16进制(恢复设置)
                    SetFrameDataStyle(true, true, true, true);
                    CmdFinished_Func();
                }
            }
            else if(en_Cmd_B_Down_Is_More == m_pCmdItem_Cur->m_enFrameBType)
            {
                if(true == m_pCmdItem_Cur->m_bTimeOut)                  // 超时
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
                        sprintf_s(chTip, sizeof(chTip), "MCU 响应超时，重发本帧[%d], 重发次数[%d]", m_pCmdItem_Cur->m_iPaksIndex+1, m_pCmdItem_Cur->m_iReDownCount+1);
                        ShowTipInfoFunc(chTip);

                        SendOneFrame(0, NULL, 0, true);     // 重发本帧
                    }
                }
                else
                {
                    if(false == g_FrameData_2.m_bDataIsOK)              // 数据不正确
                    {
                        //ShowTipInfoFunc("数据不正确");
                        
                    }
                    else
                    {
                        unsigned char chCommand = g_FrameData_2.m_chFrameCmd;
                        unsigned char chHighPak = (g_FrameData_2.m_pFrameDataBuf)[0];     // 回复的包号
                        unsigned char chLowPak = (g_FrameData_2.m_pFrameDataBuf)[1];
                        unsigned char chParam = (g_FrameData_2.m_pFrameDataBuf)[2];       // 回复的参数

                        if(true == m_pCmdItem_Cur->m_bManual)           // 手动
                        {
                            ;
                        }
                        else                                            // 自动
                        {
                            HandleRecvCommand(chCommand, chHighPak, chLowPak, chParam);
                            if(0x75 == chCommand)      // 回复 升级结束
                            {
                                m_pCmdItem_Cur->ExitDownload();         // 关闭文件
                                bCmdFinish = true;

                                // 报告
                                char* ptr = m_pCmdItem_Cur->m_pReportBuf;
                                if((en_Op_Is_Auto_Download == m_enOpType) && (ptr != NULL))
                                {
                                    sprintf_s(ptr, SIZE_OF_AUTODOWN_REPORT, "下载文件: 板卡地址[%02X, %02X](%s), 升级成功\n", 
                                        m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort,
                                        GetBoardNameOnAddr(m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort).toLatin1().data());
                                }

                                // 简略报告
                                ptr = m_pCmdItem_Cur->m_pSimpleReportBuf;
                                if(NULL != ptr)
                                {
                                    sprintf_s(ptr, SIZE_OF_AUTODOWN_REPORT, "下载文件: 板卡地址[%02X, %02X](%s), 升级成功", 
                                                        m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort,
                                                        GetBoardNameOnAddr(m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort).toLatin1().data());
                                }
                            }
                        }
                    }
                }
            }
        }
        else if(en_Cmd_A_Is_Delay == m_pCmdItem_Cur->m_enFrameAType)        // 延时(当前)
        {
            if(en_Cmd_B_Delay_Is_One == m_pCmdItem_Cur->m_enFrameBType)
            {
                ;
            }
            else if(en_Cmd_B_Delay_Is_More == m_pCmdItem_Cur->m_enFrameBType)
            {
                if(true == m_pCmdItem_Cur->m_bTimeOut)                  // 超时
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

        if(true == bCmdFinish)  // 如果当前命令已经处理完毕，则继续下一条
        {
            if((en_Cmd_A_Is_Version == m_pCmdItem_Cur->m_enFrameAType) && 
               (en_Cmd_B_Ver_Is_More == m_pCmdItem_Cur->m_enFrameBType) &&
               (true == m_pCmdItem_Cur->m_bManual))
            {
                // 如果所有的命令都已结束，则填充控件
                if(NULL == m_pCmdItem_Cur->m_next) BoardSelectFill();
            }

            GetNextCmd();
            if(NULL != m_pCmdItem_Cur)
            {
                m_iTimeOut = 0;
                if(en_Cmd_A_Is_Version == m_pCmdItem_Cur->m_enFrameAType)  // 版本查询(next)
                {
                    // 设置源地址
                    m_pMyComBasic->SetSourceAddr(m_pCmdItem_Cur->m_chSourceAddr, m_pCmdItem_Cur->m_chSourcePort, m_pCmdItem_Cur->m_chSourceUse);

                    // 设置目的地址
                    m_pMyComBasic->SetTargetAddr(m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort, m_pCmdItem_Cur->m_chTargetUse);

                    // 设置命令字
                    int iFrameNum = GetFrameNo();
                    m_pMyComBasic->SetCommand(iFrameNum, m_pCmdItem_Cur->m_chFrameCmd);

                    FrameNoInc(iFrameNum);

                    // 发送
                    SendOneFrame(m_pCmdItem_Cur->m_chFrameCmd, m_pCmdItem_Cur->m_chATCmd, sizeof(m_pCmdItem_Cur->m_chATCmd));

                    ShowTipInfoFunc("发送下一条版本查询命令");

                    // 显示信息
                    char chInfo[1024] = {0};
                    sprintf_s(chInfo, sizeof(chInfo), "查询目标板卡[%02X, %02X]版本", m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort);
                    ShowTipInfoFunc(chInfo);
                }
                else if(en_Cmd_A_Is_Update == m_pCmdItem_Cur->m_enFrameAType)   // 升级下载(next)
                {
                    errno_t err = fopen_s(&(m_pCmdItem_Cur->m_pFile), (m_pCmdItem_Cur->m_strFilePath).toLatin1(), "rb");
                    if(0 != err)
                    {
                        m_pCmdItem_Cur->m_pFile = NULL;
                        ShowTipInfoFunc("打开文件失败");
                    }
                    else
                    {
                        // 发送当前命令头包
                        DownloadSendCurCmdFirstPack();
                    }
                }
                else if(en_Cmd_A_Is_Delay == m_pCmdItem_Cur->m_enFrameAType)    // 延时(next)
                {
                    ShowTipInfoFunc("等待 5 秒");
                }
            }
            else            // 结束
            {
                ShowTipInfoFunc("命令发送完毕");
                CmdFinished_Func();
                ExitDownload();

                // 点击“发送”按钮，重新设置数据是否是16进制; 点击升级类按钮，强制设置为16进制(恢复设置)
                SetFrameDataStyle(true, true, true, true);

                // 如果是自动下载，显示报告
                if(en_Op_Is_Auto_Download == m_enOpType)
                {
                    ShowTipInfoFunc("\n自动升级报告: ");     // 详细
                    char* ptr = NULL;
                    for(int i = 0; i < NUM_OF_AUTODOWN_REPORT; ++i)
                    {
                        ptr = m_pAutoDownloadReport[i];
                        ShowTipInfoFunc(ptr, QColor(0, 0, 250));
                    }

                    // 暂时不使用简略报告
                    /*ShowTipInfoFunc("自动升级简略报告: ");
                    for(int i = 0; i < COUNT_OF_AUTODOWN_CMD; ++i)
                    {
                        ptr = m_pAutoUpdateSimpleReport[i];
                        ShowTipInfoFunc(ptr, QColor(28, 147, 148));
                    }*/
                    ShowTipInfoFunc(" \n");
                }

                // 提示：等待设备重启 8 秒，重新点击“板卡查询”获取“全部板卡”的最新版本
                if((en_Op_Is_Update_One == m_enOpType) || (en_Op_Is_Auto_Download == m_enOpType))
                {
                    ShowTipInfoFunc("\n升级结束，板卡正在重启，请等候 8 秒\n", QColor(0, 0, 250));
                    ShowTipInfoFunc("8 秒后，请选择“全部板卡”，点击“板卡查询”更新板卡的版本号\n", QColor(0, 0, 250));
                }

                m_enOpType = en_Op_Is_NULL;     // 清
            }
        }
    }
    else                    // 无当前命令
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

        // 单命令有多帧返回
        if(m_iMultiFrame > 1)
        {
            CheckSendedOneCmd(g_FrameData_2.m_pFrameDataBuf, m_iMultiFrame);
        }

    }
}

// 重发本帧
void CUpdateTab::ReDownloadFunc(QString strError)
{
    if(true == m_bSingleDebug)          // 单步
    {
        ShowTipInfoFunc("请重发");
        m_iSingleNum = 2;
        return;
    }

    {
        if(m_pCmdItem_Cur->m_iReDownCount >= m_iReDownCount_Save)
        {
            m_pCmdItem_Cur->m_iReDownCount = 0;
            ExitDownload();
            ShowTipInfoFunc("重发完毕，退出下载");
        }
        else
        {
            char chTip[1024] = {0};
            sprintf_s(chTip, sizeof(chTip), "%s 重发本帧[%d], 重发次数[%d]", strError.toLatin1().data(),m_pCmdItem_Cur->m_iPaksIndex+1, m_pCmdItem_Cur->m_iReDownCount+1);
            ShowTipInfoFunc(chTip);

            SendOneFrame(0, NULL, 0, true); // 重发本帧
        }
    }
}

// 处理返回的命令
void CUpdateTab::HandleRecvCommand(unsigned char chCommand, unsigned char chHighPak, unsigned char chLowPak, unsigned char chParam)
{
    static int stiOKRetPak = 0;     // 已经处理过的正确的包号

    int iRetPak = chHighPak * 256 + chLowPak;

    if(0x71 == chCommand)           // 回复 准备升级
    {
        // 下位机有可能重复上传，则忽略
        if(0x70 != m_pCmdItem_Cur->m_chCurCommand)
        {
            return;
        }

        stiOKRetPak = -1;
        m_iTimeOut = 0;

        if(iRetPak != m_pCmdItem_Cur->m_iTotalPaks) // 总包数不对
        {
            ReDownloadFunc(tr("MCU 回复总包数 错误。"));           // 重发本帧
            return;
        }

        if(0x00 == chParam)         // 不响应
        {
            ReDownloadFunc(tr("MCU 不响应 准备升级命令。"));           // 重发本帧
        }
        else if(0x01 == chParam)    // 响应命令
        {
            char chTip[1024] = {0};
            if((true == m_bIsEncrypt)) 
            {
                sprintf_s(chTip, sizeof(chTip), "MCU 响应 准备升级命令, 发送加密升级命令");
            }
            else if(true == m_bUseFileName)
            {
                sprintf_s(chTip, sizeof(chTip), "MCU 响应 准备升级命令, 发送文件名升级命令");
            }
            else 
            {
                sprintf_s(chTip, sizeof(chTip), "MCU 响应 准备升级命令, 下载第一包[%d/%d]", m_pCmdItem_Cur->m_iPaksIndex+1, m_pCmdItem_Cur->m_iTotalPaks+1);
            }
            ShowTipInfoFunc(chTip);

            if(false == m_bSingleDebug)
            {
                // 开始下载
                SendNextOnePak();
            }
            else                    // 单步调试
            {
                m_iSingleNum = 1;
            }
            m_pCmdItem_Cur->m_iReDownCount = 0;
        }
        else if(0x02 == chParam)    // 文件名与板卡不一致
        {
            ExitDownload();
            ShowTipInfoFunc("升级的文件名与单板不一致，退出本次下载。请确认文件名是否正确");
        }
        else
        {
            ReDownloadFunc(tr("MCU 响应 准备升级命令: 未知参数。"));           // 重发本帧
        }
    }
    else if(0x77 == chCommand)      // 是“加密”回复
    {
        // 下位机有可能重复上传，则忽略
        if(0x76 != m_pCmdItem_Cur->m_chCurCommand)
        {
            return;
        }

        stiOKRetPak = -1;
        m_iTimeOut = 0;

        if(0x00 == chParam)         // 不响应
        {
            ReDownloadFunc(tr("MCU 不响应 加密升级命令。"));           // 重发本帧
        }
        else if(0x01 == chParam)    // 响应命令
        {
            char chTip[1024] = {0};
            if(false == m_bUseFileName) sprintf_s(chTip, sizeof(chTip), "MCU 响应 加密升级命令, 下载第一包[%d/%d]", m_pCmdItem_Cur->m_iPaksIndex+1, m_pCmdItem_Cur->m_iTotalPaks+1);
            else sprintf_s(chTip, sizeof(chTip), "MCU 响应 加密升级命令, 下发文件名[%s]", m_strFileNameUse.toLatin1().data());
            ShowTipInfoFunc(chTip);

            // 开始下载
            SendNextOnePak();
            m_pCmdItem_Cur->m_iReDownCount = 0;
        }
        else
        {
            ReDownloadFunc(tr("MCU 响应 加密升级命令: 未知参数[%1]。").arg(QString::number(chParam)));           // 重发本帧
        }
    }
    else if(0x79 == chCommand)      // 是“文件名”回复
    {
        // 下位机有可能重复上传，则忽略
        if(0x78 != m_pCmdItem_Cur->m_chCurCommand)
        {
            return;
        }

        stiOKRetPak = -1;
        m_iTimeOut = 0;

        if(0x00 == chParam)         // 不响应
        {
            ReDownloadFunc(tr("MCU 不响应 文件名升级命令，可能选错了升级文件。"));           // 重发本帧
        }
        else if(0x01 == chParam)    // 响应命令
        {
            char chTip[1024] = {0};
            sprintf_s(chTip, sizeof(chTip), "MCU 响应 加密升级命令, 下载第一包[%d/%d]", m_pCmdItem_Cur->m_iPaksIndex+1, m_pCmdItem_Cur->m_iTotalPaks+1);
            ShowTipInfoFunc(chTip);

            if(false == m_bSingleDebug)
            {
                // 开始下载
                SendNextOnePak();
            }
            else                    // 单步调试
            {
                m_iSingleNum = 1;
            }
            m_pCmdItem_Cur->m_iReDownCount = 0;
        }
        else
        {
            ReDownloadFunc(tr("MCU 响应 加密升级命令: 未知参数[%1]。").arg(QString::number(chParam)));           // 重发本帧
        }
    }
    else if(0x73 == chCommand)      // 回复 数据下发
    {
        // 下位机有可能重复上传，则忽略
        if(0x72 != m_pCmdItem_Cur->m_chCurCommand)
        {
            return;
        }

        m_iTimeOut = 0;

        if(iRetPak != m_pCmdItem_Cur->m_iPaksIndex) // 包号不对
        {
            // 如果是已经接收过的包号，则不重发
            if(iRetPak == stiOKRetPak) return;

            ReDownloadFunc(tr("MCU 回复包号 错误。"));           // 重发本帧
            return;
        }

        if(0x00 == chParam)         // 不响应，写入失败，上位机要重发本帧数据，如果隔一段时间什么数据都没收到，上位机也要重发本帧
        {
            // new : (退出，提示从新开始升级)
            ExitDownload();
            ShowTipInfoFunc("MCU 响应异常，退出本次下载。请重新开始升级操作");
        }
        else if((0x01 == chParam) || (0x04 == chParam))    // 正确响应命令  // 本帧数据己成功写入，请发下一帧
        {
            stiOKRetPak = iRetPak;          // 保存正确响应的包号
            char chTip[1024] = {0};
            sprintf_s(chTip, sizeof(chTip), "MCU 响应 数据下发: 正确响应命令，下载下一包[%d/%d]", m_pCmdItem_Cur->m_iPaksIndex+1, m_pCmdItem_Cur->m_iTotalPaks+1);
            ShowTipInfoFunc(chTip);
            if(false == m_bSingleDebug)
            {
                // 下载 下一包
                SendNextOnePak();
            }
            else            // 单步调试
            {
                m_iSingleNum = 3;
            }
            m_pCmdItem_Cur->m_iReDownCount = 0;
        }
        else if(0x02 == chParam)    // 包号不正确
        {
            ReDownloadFunc(tr("MCU 响应 数据下发: 包号不正确。"));           // 重发本帧
        }
        else if(0x03 == chParam)    // 写入FLASH失败
        {
            ReDownloadFunc(tr("MCU 响应 数据下发: 写入 FLASH 失败。"));           // 重发本帧
        }
        else
        {
            ReDownloadFunc(tr("MCU 响应 数据下发: 未知参数。"));           // 重发本帧
        }
    }
    else if(0x75 == chCommand)      // 回复 升级结束
    {
        // 下位机有可能重复上传，则忽略
        if(0x74 != m_pCmdItem_Cur->m_chCurCommand)
        {
            return;
        }

        m_iTimeOut = 0;

        if(iRetPak != m_pCmdItem_Cur->m_iTotalPaks) // 总包数不对
        {
            // 退出下载
            ShowTipInfoFunc("MCU 响应 升级结束: 回复总包数 错误, 退出下载");
        }

        if(0x00 == chParam)         // 没全正确
        {
            // 退出下载
            ShowTipInfoFunc("MCU 响应 升级结束: 没全正确，退出下载");
        }
        else if(0x01 == chParam)    // 正确收到全部数据
        {
            ShowTipInfoFunc("MCU 响应 升级结束: 正确收到全部数据，退出下载");
        }
        else
        {
            ShowTipInfoFunc("MCU 响应 升级结束: 未知参数，退出下载");
        }

        // 如果是自动下载，则不调用下面的函数
        if(en_Cmd_B_Down_Is_More != m_pCmdItem_Cur->m_enFrameBType) ExitDownload();
    }
    else
    {
        //ReDownloadFunc(tr("MCU 回复未知命令。"));           // 重发本帧

        // 不认识的命令，跳过
        //m_pCmdItem_Cur->m_iReDownCount = 0;
    }
}

// 发送一帧(或者是重发)
void CUpdateTab::SendOneFrame(unsigned char chCommand, unsigned char* pchData, int iDataSize, bool isReSend)
{
    // 超时计时复位
    m_iTimeOut = 0;

    if(true == isReSend)        // 重发本帧
    {
        // 发送
        m_pMyComBasic->ReSendCurComNetData();

        ++ m_pCmdItem_Cur->m_iReDownCount;
    }
    else
    {
        // 发送
        SendComNetData(chCommand, (const char*)pchData, iDataSize);
    }
}

// 发送 准备升级 命令
void CUpdateTab::SendPrepareUpdate()
{
    m_pCmdItem_Cur->DownloadReset();

    // 设置源地址
    m_pMyComBasic->SetSourceAddr(m_pCmdItem_Cur->m_chSourceAddr, m_pCmdItem_Cur->m_chSourcePort, m_pCmdItem_Cur->m_chSourceUse);

    // 设置目的地址
    m_pMyComBasic->SetTargetAddr(m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort, m_pCmdItem_Cur->m_chTargetUse);

    // 设置命令字
    int iFrameNum = GetFrameNo();
    m_pMyComBasic->SetCommand(iFrameNum, m_pCmdItem_Cur->m_chFrameCmd);

    FrameNoInc(iFrameNum);

    // 准备升级
    fseek(m_pCmdItem_Cur->m_pFile, 0, SEEK_END);
    unsigned int iTotalSize = ftell(m_pCmdItem_Cur->m_pFile);
    m_pCmdItem_Cur->m_iTotalPaks = iTotalSize / 256 + 1;

    // 解析短文件名
    char chFileName[MAX_PATH] = {0};
    strcpy_s(chFileName, sizeof(chFileName), m_pCmdItem_Cur->m_strFilePath.toLatin1().data());
    char* ptr = chFileName + strlen(chFileName) - 1;
    while(('V' != *ptr) && ('v' != *ptr)) --ptr;
    if('_' == *(ptr-1)) --ptr;
    *ptr = '\0';
    --ptr;
    while(('/' != *ptr) && ('\\' != *ptr)) --ptr;
    ++ptr;

    // 总包数(2)
    unsigned int iHighLength = (m_pCmdItem_Cur->m_iTotalPaks)/256;
    unsigned int iLowLength = (m_pCmdItem_Cur->m_iTotalPaks)%256;

    unsigned char chData[MAX_PATH] = {0};
    chData[0] = iHighLength;
    chData[1] = iLowLength;
    chData[2] = '@';
    memcpy(chData+3, ptr, strlen(ptr));
    iLowLength = strlen((char*)chData+2) + 2;
    chData[iLowLength] = '#';
    iLowLength = strlen((char*)chData+2) + 2;       // 最后的长度

    // 发送
    m_pCmdItem_Cur->m_chCurCommand = 0x70;
    SendOneFrame(m_pCmdItem_Cur->m_chCurCommand, chData, iLowLength);

    ShowTipInfoFunc("发送 准备升级 命令");

    m_iTimeOut = 0;

    rewind(m_pCmdItem_Cur->m_pFile);                // 复位文件指针

    // 显示信息
    char chInfo[1024] = {0};
    sprintf_s(chInfo, sizeof(chInfo), "升级目标板卡[%02X, %02X][%s]软件", 
                            m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort, 
                            m_pCmdItem_Cur->m_strFilePath.toLatin1().data());
    ShowTipInfoFunc(chInfo);

    // 设置目标地址
    if(true == g_oPageCheck.IsAdmin())
    {
        sprintf_s(chInfo, sizeof(chInfo), "%X",m_pCmdItem_Cur->m_chTargetAddr);
        m_pTargetAddr_LineEdit->setText(chInfo);
        sprintf_s(chInfo, sizeof(chInfo), "%X", m_pCmdItem_Cur->m_chTargetPort);
        m_pTargetPort_LineEdit->setText(chInfo);
    }
}

// 下载一包数据
void CUpdateTab::SendNextOnePak()
{
    if(feof(m_pCmdItem_Cur->m_pFile))           // 文件结束
    {
        // 总包数(2)
        unsigned int iHighLength = (m_pCmdItem_Cur->m_iTotalPaks)/256;
        unsigned int iLowLength = (m_pCmdItem_Cur->m_iTotalPaks)%256;

        unsigned char chDataH[2] = {0};
        chDataH[0] = iHighLength;
        chDataH[1] = iLowLength;

        // 发送
        m_pCmdItem_Cur->m_chCurCommand = 0x74;
        SendOneFrame(m_pCmdItem_Cur->m_chCurCommand, chDataH, 2);

        return;
    }

    unsigned char chData[256+2];
    memset(chData, 0xFF, sizeof(chData));

    // 是“加密”
    if((true == m_bIsEncrypt) && (0x70 == m_pCmdItem_Cur->m_chCurCommand))
    {
        // 总包数(2)
        unsigned int iHighLength = (m_pCmdItem_Cur->m_iTotalPaks)/256;
        unsigned int iLowLength = (m_pCmdItem_Cur->m_iTotalPaks)%256;

        chData[0] = iHighLength;
        chData[1] = iLowLength;

        // 发送
        m_pCmdItem_Cur->m_chCurCommand = 0x76;
        SendOneFrame(m_pCmdItem_Cur->m_chCurCommand, chData, 2);
    }
   else if((true == m_bUseFileName) && ((0x70 == m_pCmdItem_Cur->m_chCurCommand) || (0x76 == m_pCmdItem_Cur->m_chCurCommand)))
    {
        // 文件名
        int iLength = m_strFileNameUse.length();
        memcpy(chData, m_strFileNameUse.toLatin1().data(), iLength);
        chData[iLength] = '\0';

        // 发送
        m_pCmdItem_Cur->m_chCurCommand = 0x78;
        SendOneFrame(m_pCmdItem_Cur->m_chCurCommand, chData, iLength+1);
    }
    else            // 正常发送
    {
        // 包号(2)
        ++m_pCmdItem_Cur->m_iPaksIndex;
        unsigned int iHighLength = (m_pCmdItem_Cur->m_iPaksIndex)/256;
        unsigned int iLowLength = (m_pCmdItem_Cur->m_iPaksIndex)%256;

        chData[0] = iHighLength;
        chData[1] = iLowLength;

        int iReadSize = 256;
        fread_s(chData+2, iReadSize, iReadSize, 1, m_pCmdItem_Cur->m_pFile);

        // 发送
        m_pCmdItem_Cur->m_chCurCommand = 0x72;
        SendOneFrame(m_pCmdItem_Cur->m_chCurCommand, chData, sizeof(chData));
    }
    
}

// 复位 升级变量
void CUpdateTab::DownloadReset()
{
    m_iTimeOut = 0;

    m_bSingleDebug = false;
    m_iSingleNum = 0;
}

// 退出 下载
void CUpdateTab::ExitDownload()
{
    if(NULL != m_pCmdItem_Cur) m_pCmdItem_Cur->ExitDownload();
    GetNextCmd();                   // 下一个命令

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
    // Rem_Control NavtfVerx.y (Vx.y前后有空格，小数点后面只有一位数)
    char chInput[1024] = {0};
    strcpy_s(chInput, sizeof(chInput), pszInput);
    char* ptr = ::strstr(chInput, "NavtfVer");
    char* ptrValue = ptr;
    if(NULL != ptr)
    {
        // 1. 名称，去后面的空格
        ptr -= 1;
        while(' ' == *ptr) --ptr;
        ++ptr;
        *ptr = '\0';
        strcpy_s(pszName, iNamelength, chInput);

        // 2. 版本，去前后的空格
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
    // 参考输入板_Ref_Input_P0102_Vx.y
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

// 由板卡地址获取板卡名称
QString CUpdateTab::GetBoardNameOnAddr(unsigned int iAddr, unsigned int iPort)
{
    QString strName = tr("名称未定");
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

// 下发命令，检测连接是否成功
void CUpdateTab::CheckConnect()
{
    SetFrameDataStyle(false, false, false, true);
    DestroyAllCmd();
    m_enOpType = en_Op_Is_Ver_One;

    st_CommandItem* pCmdItem = new st_CommandItem(en_Cmd_A_Is_Version, en_Cmd_B_Send_One_Cmd);
    AddOneCmd(pCmdItem);

    pCmdItem->m_chSourceAddr = 0x60;     // 源、目的和命令
    pCmdItem->m_chSourcePort = 0x00;
    pCmdItem->m_chSourceUse = 0x00;
    pCmdItem->m_chTargetAddr = 0x80;
    pCmdItem->m_chTargetPort = 0x00;
    pCmdItem->m_chTargetUse = 0x00;
    pCmdItem->m_chFrameCmd = COMMAND_IS_AT;
    pCmdItem->m_bManual = true;

    GetFirstCmd();

    // 发送当前命令头包
    {
        // 设置源地址
        m_pMyComBasic->SetSourceAddr(m_pCmdItem_Cur->m_chSourceAddr, m_pCmdItem_Cur->m_chSourcePort, m_pCmdItem_Cur->m_chSourceUse);

        // 设置目的地址
        m_pMyComBasic->SetTargetAddr(m_pCmdItem_Cur->m_chTargetAddr, m_pCmdItem_Cur->m_chTargetPort, m_pCmdItem_Cur->m_chTargetUse);

        // 设置命令字
        int iFrameNum = GetFrameNo();
        m_pMyComBasic->SetCommand(iFrameNum, m_pCmdItem_Cur->m_chFrameCmd);

        FrameNoInc(iFrameNum);

        m_bCmdIsNavtfIsOne = false;
        m_bCmdIsNavtfFinished = false;

        // 发送
        char chData[64] = "navtf2000";
        SendOneFrame(m_pCmdItem_Cur->m_chFrameCmd, (unsigned char*)chData, strlen(chData));

        ShowTipInfoFunc("发送 navtf2000 命令");

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

// 检测下发的命令的返回结果
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

    // 指示是否是多帧
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

                // 可能多个: PLL_Universal V2.5 0400
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
                
                // 填充控件
                m_bCmdIsNavtfFinished = true;
                m_bCmdIsNavtfIsOne = false;
                BoardSelectFill();

                ShowTipInfoFunc("\n设备连接成功\n", QColor(0, 0, 250));

                // 手工执行一次
                CmdFinished_Func();
            }
            else
            {
                ;
            }
        }
    }

    // 清
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
            i = 0;          // 置 错误
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

// 增加一个板卡
void CUpdateTab::AddOneBoard(char* chName, int iNameLength, unsigned char chAddr, unsigned char chPort, float fVer)
{
    // 注意：为区别相同的板卡，名称后面加上地址信息作为后缀 "_P0101" : Ref_Input_P0102
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

// 强制设命令字=1，勾选“自动成帧”，不选“自动重发”，“发16进制”，“收16进制”
void CUpdateTab::SetCmdAndOptionManu()
{
    m_pFrameCmd_LineEdit->setText("1");
    m_pAutoMakeFrame_CheckBox->setChecked(true);

    m_pAutoReSend_CheckBox->setChecked(false);
    m_pSend16Data_CheckBox->setChecked(false);
    m_pRecv16Data_CheckBox->setChecked(false);
}


// 选择导出文件
void CUpdateTab::SelectExportFilePath()
{
    QFileDialog::Options options;
    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(this, tr("文件保存"), tr("导出文件"), 
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

// 测试网口连接
void CUpdateTab::NetConnectTestFunc()
{
    m_pNetConnectTest_Button->setEnabled(false);

    QString strData = tr("ping %1 -l 2000").arg(m_pNetIPAddr_LineEdit->text());
    CFuncTool::ExecCommand(strData.toLatin1().data());
}

// 打开和关闭导出文件
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
        //ShowTipInfoFunc("打开导出文件失败");
    }
    else
    {
        // 指针移动到文件尾
        SetFilePointer(hFile , 0, NULL, FILE_END);
        m_hExportFile = hFile;
    }
}

// 选择单板还是系统(多板)
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

// 命令下发 保存日志
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
                ShowTipInfoFunc("创建 Log 文件夹失败");
                return;
            }
        }

        QString strTime = QDate::currentDate().toString("yyyy_MM_dd");
        sprintf_s(chPath, sizeof(chPath), "./Log/%s.txt", strTime.toLatin1().data());
        HANDLE hFile = CreateFile(chPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, NULL, NULL);
        if(INVALID_HANDLE_VALUE == hFile)
        {
            ShowTipInfoFunc("打开 Log 文件夹失败");
            return;
        }

        // 指针移动到文件尾
        SetFilePointer(hFile , 0, NULL, FILE_END);
        m_hCmdLogFile = hFile;
    }

    // 增加时间
    QString strCur = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    strCur += tr("  [%1, %2] ").arg(QString::number(chAddr, 16)).arg(QString::number(chPort)) + strLog;

    DWORD dwWrites;
    WriteFile((HANDLE)m_hCmdLogFile, strCur.toLatin1().data(), strCur.toLatin1().length(), &dwWrites, NULL);
    char chEnter[2] = {0x0D, 0x0A};
    WriteFile((HANDLE)m_hCmdLogFile, chEnter, 2, &dwWrites, NULL);
}

// 某个地址的板卡是否已经查询到
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

// 根据地址获取板卡的原始名称
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
    全局函数
*****************************************************************************************/
// 显示接收数据
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

// 显示发送数据
void ShowSendDataFunc(unsigned char chAddr, unsigned char chPort, const char* pszShowData, int iLength)
{
    if(NULL == g_pUpdateTab) return;
    g_pUpdateTab->ShowSendDataFunc(chAddr, chPort, pszShowData, iLength);
}


// 升级下载超时函数
void DownloadTimeoutFunc()
{
    if(NULL == g_pUpdateTab) return;
    g_pUpdateTab->DownloadTimeoutFunc();
}

// 帧数据处理函数
void FrameHanleFunc(const st_FrameData* pFrameData)
{
    if(NULL == g_pUpdateTab) return;
    g_pUpdateTab->FrameHanleFunc(pFrameData);
}
