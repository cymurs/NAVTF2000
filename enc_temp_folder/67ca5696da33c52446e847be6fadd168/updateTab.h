#ifndef _UPDATETAB_H_
#define _UPDATETAB_H_

#include <QWidget>

#include <string>
#include <map>

// --------------------------------------------------------------------------------------
// 通信/升级 tab (设备维护)
// --------------------------------------------------------------------------------------
#define SIZE_WIDTH_OF_LABEL             50      // 标签宽度最大值
#define SIZE_WIDTH_OF_EDIT              100     // 输入框宽度最大值
#define SIZE_WIDTH_OF_BUTTON_RIGHT      300     // 右边按键的宽度
#define SIZE_HEIGHT_OF_BUTTON_RIGHT     40      // 右边按键的高度

// 给需要保存的输入框编号
#define ID_OF_IP_ADDRESS            0       // IP地址
#define ID_OF_IP_PORT               1       // IP端口

#define ID_OF_SOURCE_ADDR           2       // 源地址
#define ID_OF_SOURCE_PORT           3       // 源端口
#define ID_OF_SOURCE_USE            4       // 备用1

#define ID_OF_TARGET_ADDR           5       // 目的地址
#define ID_OF_TARGET_PORT           6       // 目的端口
#define ID_OF_TARGET_USE            7       // 备用2

#define ID_OF_FRAME_NUM             8       // 帧 号
#define ID_OF_FRAME_COMMAND         9       // 命令字

#define ID_OF_RESEND_CIRCLE         10      // 自动重发周期

#define ID_OF_TIMEOUT_MS            11      // 超时ms
#define ID_OF_TIMEOUT_COUNT         12      // 超时次数


// 选择网口还是串口
#define COMNET_SELECT_IS_COM    0
#define COMNET_SELECT_IS_NET    1

// 选择单板还是系统(多板)
#define UPDATE_SELECT_IS_ALONE  0
#define UPDATE_SELECT_IS_SYSTEM 1

// 自动升级文件目录
#define NAME_OF_AUTO_UPDATE_DIR     "Updata_Soft"

// 版本查询
#define NAME_OF_VERSION_ONLYONE     "单个板卡"
#define NAME_OF_VERSION_ALL         "全部板卡"

// 自动下载报告单个长度
#define SIZE_OF_AUTODOWN_REPORT     (1024*2)
#define COUNT_OF_AUTODOWN_CMD       15
#define NUM_OF_AUTODOWN_REPORT      (COUNT_OF_AUTODOWN_CMD*2)

// --------------------------------------------------------------------------------------
class QGroupBox;
class QVBoxLayout;
class QHBoxLayout;

class QTextEdit;
class QComboBox;
class QLabel;
class QPushButton;
class QLineEdit;
class QRadioButton;
class QCheckBox;

class QRegExpValidator;
class QSignalMapper;
class QTimer;

class CMyComBasic;
struct st_FrameData;

class CMyImageTab;
class CStatusTab;
class CMyAdjustTab;
class CGNSSTab;

// --------------------------------------------------------------------------------------
//当前执行的操作
enum en_OpType
{
    en_Op_Is_NULL = 0,          // 无操作
    en_Op_Is_Ver_One,           // 查询某个版本
    en_Op_Is_Ver_More,          // 查询多个版本
    en_Op_Is_Update_One,        // 下载某个版本
    en_Op_Is_Auto_Download,     // 自动下载
};



// --------------------------------------------------------------------------------------
enum en_CmdAType         // 大类
{
    en_Cmd_A_Is_Version = 0,        // 版本查询
    en_Cmd_A_Is_Update,             // 升级下载
    en_Cmd_A_Is_Delay,              // 延时
};
enum en_CmdBTyep        // 小类
{
    en_Cmd_B_Ver_Is_One = 0,        // 查询一个版本
    en_Cmd_B_Ver_Is_More,           // 查询多个版本

    en_Cmd_B_Down_Is_One,           // 升级一个板卡
    en_Cmd_B_Down_Is_More,          // 升级多个板卡

    en_Cmd_B_Delay_Is_One,          // 延时一个
    en_Cmd_B_Delay_Is_More,         // 延时多个

    en_Cmd_B_Send_One_Cmd,          // 下发一个命令
};

struct st_CommandItem
{
    st_CommandItem(en_CmdAType enFrameAType, en_CmdBTyep enFrameBType)
    {
        m_enFrameAType = enFrameAType;
        m_enFrameBType = enFrameBType;

        m_next = NULL;
        m_chSourceAddr = 0;
        m_chSourcePort = 0;
        m_chSourceUse = 0;
        m_chTargetAddr = 0;
        m_chTargetPort = 0;
        m_chTargetUse = 0;
        m_chFrameCmd = 0;

        m_chATCmd[0] = 'v';
        m_chATCmd[1] = 'e';
        m_chATCmd[2] = 'r';

        m_strFilePath = "";
        m_pFile = NULL;
        DownloadReset();

        m_bTimeOut = false;
        m_bManual = false;

        m_iDelayCount = 0;
        m_pReportBuf = NULL;
        m_pSimpleReportBuf = NULL;
    }

    void DownloadReset()
    {
        m_iTotalPaks = 0;
        m_iPaksIndex = 0;
        m_chCurCommand = 0;
        m_iReDownCount = 0;
    }
    void ExitDownload()
    {
        if(NULL != m_pFile)
        {
            fclose(m_pFile);
            m_pFile = NULL;
        }
    }

    st_CommandItem* m_next;
    en_CmdAType m_enFrameAType;     // 大类
    en_CmdBTyep m_enFrameBType;     // 小类

    unsigned char m_chSourceAddr;   // 源、目的和命令
    unsigned char m_chSourcePort;
    unsigned char m_chSourceUse;
    unsigned char m_chTargetAddr;
    unsigned char m_chTargetPort;
    unsigned char m_chTargetUse;
    unsigned char m_chFrameCmd;

    unsigned char m_chATCmd[3];     // 版本查询命令

    bool m_bTimeOut;                // 是否是超时
    bool m_bManual;                 // 是否是手动操作，如手动查询版本，手动升级等

    int m_iDelayCount;              // 延时命令使用(多少秒)
    char* m_pReportBuf;             // 自动升级使用：详细报告
    char* m_pSimpleReportBuf;       // 自动升级使用：简略报告

    QString m_strFilePath;          // 升级下载，目录文件名
    FILE* m_pFile;                  // 文件句柄
    int m_iTotalPaks;               // 总包数和包号(包号从1开始)
    int m_iPaksIndex;
    unsigned char m_chCurCommand;   // 当前下发命令
    volatile int m_iReDownCount;    // 重发次数
};

// 板卡版本信息
struct st_BoardVerInfo
{
    st_BoardVerInfo()
    {
        m_strName = "";
        m_strOriName = "";
        m_chAddr = 0;
        m_chPort = 0;
        m_chUse = 0;
        m_fVer = 0.0f;
    }

    std::string m_strName;          // 名称
    std::string m_strOriName;       // 原始名称，没有经过修改
    unsigned char m_chAddr;         // 地址
    unsigned char m_chPort;
    unsigned char m_chUse;
    float m_fVer;                   // 版本
};

// --------------------------------------------------------------------------------------
class CUpdateTab : public QWidget
{
    Q_OBJECT

public:
    CUpdateTab(QWidget* parent = 0);
    ~CUpdateTab();

public:
    void SetComBasic(CMyComBasic* pComBasic);

    // 关闭窗口时调用的函数
    void CloseFunction();

    void Set_MyImageTab(CMyImageTab* pMyImageTab);
    void Set_StatusTab(CStatusTab* pStatusTab);
    void Set_MyAdjustTab(CMyAdjustTab* pMyAdjustTab);
    void Set_GNSSTab(CGNSSTab* pGNSSTab);

    bool ComNetIsOpen();

public:     // 外界使用通信功能
    // 设置输入框的源地址等信息
    void SetSourceTargetInfo(int iCmd);

    // 设置源地址
    void SetSourceAddr(int iAddr, int iPort, int iResv);

    // 设置目的地址
    void SetTargetAddr(int iAddr, int iPort, int iResv);

    // 设置命令字
    void SetCommand(int iCmd);

    // 点击“发送”按钮，重新设置数据是否是16进制; 点击升级类按钮，强制设置为16进制
    // 点击“发送”按钮，重新设置数据是否有帧头帧尾; 点击升级类按钮，强制设置为有帧头帧尾
    void SetFrameDataStyle(bool bManualSet, bool bSendIs16Hex, bool bRecvIs16Hex, bool bHasFrame);

    // 发送数据
    void SendComNetData(unsigned char chCommand, const char* pszDataBuf, int iDataLength_in);

    // 全部板卡 版本查询
    void VersionQueryAllBoard();

    // 强制设命令字=1，勾选“自动成帧”，不选“自动重发”，“发16进制”，“收16进制”
    void SetCmdAndOptionManu();

    //  给 参考输入板 和 主控板 发送 getsta 命令
    void Send_getsta_To_RInputAndMain();

    // 选择频率输入
    void SendFreqSelect(int iIndex);

    // 选择脉冲输入
    void SendPlusSelect(int iIndex);

    // 发送 目标地址字符串内容 功能
    void SendChangeData(unsigned char chAddr, unsigned char chPort, char* pchData);

    // 读取和设置目的地址
    void GetTargetForGNSS(QString&, QString&);
    void SetTargetForGNSS(QString, QString);

    // 设置设备的信号状态
    void SetDeviceSignalStatus(int iValue);

    // 某个地址的板卡是否已经查询到
    bool BoardIsValid(unsigned char chAddr, unsigned char chPort);

    // 根据地址获取板卡的原始名称
    std::string GetBoardOriName(unsigned char chAddr, unsigned char chPort);

public:
    // 显示接收数据
    void ShowRecvDataFunc(unsigned char chAddr, unsigned char chPort, const char* pszShowData, bool, int iR, int iG, int iB);
    void SetRecvShowFunc(int iShowType);

    // 显示发送数据
    void ShowSendDataFunc(unsigned char chAddr, unsigned char chPort, const char* pszShowData, int iLength);

    // 显示提示信息
    void ShowTipInfoFunc(const char* pszShowData, QColor rgb = QColor(0, 0, 0));

    // 命令下发 保存日志
    void CmdLog_Func(unsigned char, unsigned char, QString&);

    // 由板卡地址获取板卡名称
    QString GetBoardNameOnAddr(unsigned int iAddr, unsigned int iPort);

public:     // 升级
    // 升级下载超时函数
    void DownloadTimeoutFunc();

    // 帧数据处理函数
    void FrameHanleFunc(const st_FrameData* pFrameData);

    // 帧数据处理线程调用函数
    void HandleFrameData();

    // 重发本帧
    void ReDownloadFunc(QString strError);

private:     // 升级
    // 处理返回的命令
    void HandleRecvCommand(unsigned char chCommand, unsigned char chHighPak, unsigned char chLowPak, unsigned char chParam);

    // 发送 准备升级 命令
    void SendPrepareUpdate();

    // 发送一帧(或者是重发)
    void SendOneFrame(unsigned char chCommand, unsigned char* pchData, int iDataSize, bool isReSend = false);

    // 下载一包数据
    void SendNextOnePak();

    // 复位 升级变量
    void DownloadReset();

    // 退出 下载
    void ExitDownload();

protected:
    void paintEvent(QPaintEvent*);
    void keyPressEvent(QKeyEvent*);

private:        // Command
    void DestroyAllCmd();
    void AddOneCmd(st_CommandItem* pOneItem);
    void GetFirstCmd();
    void GetNextCmd();

    void DestroyAllBoardVerInfo();
    bool GetBoardNameAndVerOnInput(const char* pszInput, char* pszName, int iNamelength, float& fVersion);
    bool GetBoardNameOnComBox(const char* pszInput, char* pszName, int iNameLength);

    // 命令结束
    void CmdFinished_Func();

    // 发送当前命令头包
    void DownloadSendCurCmdFirstPack();

    // 下发命令，检测连接是否成功
    void CheckConnect();

    // 检测下发的命令的返回结果
    void CheckSendedOneCmd(const char* pszInput, int iMultiFrame);
    bool ParseInputString(const char* pszInput, int iCheckNum, char* pszOut1, char* pszOut2, char* pszOut3, char* pszOut4);

    // 增加一个板卡
    void AddOneBoard(char* chName, int iNameLength, unsigned char chAddr, unsigned char chPort, float fVer);

private slots:
    // 打开/关闭串口(网口与串口互斥)
    void OpenCloseCom();

    // 打开/关闭网口(网口与串口互斥)
    void OpenCloseNet();

    // 串口改变
    void ComNameChanged(const QString& text);

    // 保存输入框的内容
    void SaveInput(int iID);

    // 选择网口还是串口
    void ComNetChanged(int);
    void SelectCom_Slot();
    void SelectNet_Slot();

    // 发 送
    void SendData();

    // 清 屏
    void ClearShowInfo();

    // 显示
    void ShowRecvText_Slot(unsigned char, unsigned char, const QString, bool, int, int, int);
    void ShowSendText_Slot(unsigned char chAddr, unsigned char chPort, const QString);
    void ShowTipText_Slot(const QString, QColor);

    // Scroll
    void ShowRecvAutoScroll_Slot();
    void ShowTipAutoScroll_Slot(); 

    // 控件
    void FrameNoInc_Slot(int);
    void BoardSelectFill_Slot();

    // 命令结束
    void CmdFinished_Slot();

    // 选项：自动重发、自动成帧、16进制、调试
    void AutoReSendClicked();
    void RecvAllClicked();
    void AutoMakeFrameClicked();
    void CheckSend16DataClicked();
    void CheckRecv16DataClicked();
    void CheckDebugClicked();

    // 发送是否追加“回车”
    void EnterClicked();

    // 重发定时到
    void ReSendTimeReach();

    // 打开文件
    void OpenFilePath();

    // 版本查询
    void VersionQuery();

    // 开始升级
    void StartUpdate();

    // 停止升级
    void StopUpdate();

    // 自动升级
    void AutoUpdate();

    // 选择导出文件
    void SelectExportFilePath();

    // 测试网口连接
    void NetConnectTestFunc();

    // 选择单板还是系统(多板)
    void BoardTypeChanged(int);

    // 选择某个板卡
    void BoardSelect_Slot(const QString&);

    // 获取设备信号状态
    void DeviceSignalStatus_Slot(int);

    // 命令下发 保存日志
    void CmdLog_Slot(unsigned char, unsigned char, QString);

    void CheckSendedOneCmd_Slot(const QString, int);

private:
    void CreateWidgets();

    // 通信设置
    QVBoxLayout* CreateCommSet();
    QGroupBox* CreateComInfo();
    QGroupBox* CreateNetInfo();

    // 源地址、目的地址、帧号和命令字
    QGroupBox* CreateSourceAddr();
    QGroupBox* CreateFrameCmd();

    // 导出文件
    QGroupBox* CreateExportGroup();

    // 选项和发送等
    QGroupBox* CreateClear();
    QGroupBox* CreateOption();
    QGroupBox* CreateSend();

    // 软件升级
    QVBoxLayout* CreateUpdate();
    QGroupBox* CreateManuUpdate();

    // 板卡选择
    QGroupBox* CreateBoardSelectGroup();

    // 显示区
    QVBoxLayout* CreateShow();


    // 参考输入
    QVBoxLayout* CreateRefStatus();

    // 如果不是管理员版本，需要输入超级命令才能允许某些功能
    void SetSomeFuncEnabled(bool bEnable);

private:
    // 初始化串口设置
    void InitComInfo();

    // 关闭串口
    void CloseCom();

    // 关闭网口
    void CloseNet();

    // 设置按钮状态(针对端口是否打开)
    void SetFuncButtonEnable(bool bEnable);

    // 显示下载文件的内容
    void ShowUpdateFile();
    void Change16ToAscii8Bits(int iData, QString& strResult);

    // 获取路径的文件名称
    QString GetShortFileOnPath(const char* pszPath);

    // 获取 源、目的和命令等
    unsigned char GetSourceAddr();
    unsigned char GetSourcePort();
    unsigned char GetSourceUse();
    unsigned char GetTargetAddr();
    unsigned char GetTargetPort();
    unsigned char GetTargetUse();
    unsigned char GetFrameNo();

    void FrameNoInc(unsigned char chCurFrameNo);

    void BoardSelectFill();

    // 执行命令时禁止其他操作
    void SetAllEnabled(bool bEnable);

    // 停止 自动重发
    void StopAutoReSend();

    // 打开和关闭导出文件
    void CloseExportFile();
    void OpenExportFile(char* pszFilePath);


signals:
    // 显示
    void ShowRecvText_Signal(unsigned char, unsigned char, const QString, bool, int, int, int);
    void ShowSendText_Signal(unsigned char chAddr, unsigned char chPort, const QString);
    void ShowTipText_Signal(const QString, QColor);

    // 控件
    void FrameNoInc_Signal(int);
    void BoardSelectFill_Signal();

    // 命令结束
    void CmdFinished_Signal();

    // 获取设备信号状态
    void DeviceSignalStatus_Signal(int);

    // 命令下发 保存日志
    void CmdLog_Signal(unsigned char, unsigned char, QString);

    void CheckSendedOneCmd_Signal(const QString, int);

private:
    // 串口设置
    QComboBox* m_pComName_ComboBox;
    QComboBox* m_pComBaud_ComboBox;
    QComboBox* m_pComDataBit_ComboBox;
    QComboBox* m_pComStopBit_ComboBox;
    QComboBox* m_pComCheck_ComboBox;
    QLabel* m_pComStatus_Label;
    QPushButton* m_pComOpen_Button;
    QGroupBox* m_pSelectCom_GroupBox;

    // 网口设置
    QLineEdit* m_pNetIPAddr_LineEdit;
    QLineEdit* m_pNetIPPort_LineEdit;
    QLabel* m_pNetStatus_Label;
    QPushButton* m_pNetOpen_Button;
    QGroupBox* m_pSelectNet_GroupBox;

    QPushButton* m_pNetConnectTest_Button;

    // 源地址、目的地址、帧号和命令字
    QLineEdit* m_pSourceAddr_LineEdit;
    QLineEdit* m_pSourcePort_LineEdit;
    QLineEdit* m_pSourceResv_LineEdit;

    QLineEdit* m_pTargetAddr_LineEdit;
    QLineEdit* m_pTargetPort_LineEdit;
    QLineEdit* m_pTargetResv_LineEdit;

    QLineEdit* m_pFrameNo_LineEdit;
    QLineEdit* m_pFrameCmd_LineEdit;

    // 信息显示
    QTextEdit* m_pRecvShow_TextEdit;
    QTextEdit* m_pTipShow_TextEdit;

    // 手动升级
    QPushButton* m_pOpenFile_Button;
    QLineEdit* m_pFileShow_LineEdit;
    QPushButton* m_pVerQuary_Button;
    QComboBox* m_pBoardSelect_ComboBox;

    QRadioButton* m_pSelectAlone_Radio;
    QRadioButton* m_pSelectSystem_Radio;

    QCheckBox* m_pEnter_CheckBox;

    QLineEdit* m_pTimeoutMs_LineEdit;
    QLineEdit* m_pTimeoutCount_LineEdit;

    QPushButton* m_pStartUpdate_Button;
    QPushButton* m_pStopUpdate_Button;

    // 底部
    QCheckBox* m_pAutoReSend_CheckBox;
    QLineEdit* m_pTimeReSend_LineEdit;

    QCheckBox* m_pRecvAll_CheckBox;
    QCheckBox* m_pAutoMakeFrame_CheckBox;
    QCheckBox* m_pSend16Data_CheckBox;
    QCheckBox* m_pRecv16Data_CheckBox;
    QCheckBox* m_pDebug_CheckBox;

    QPushButton* m_pClearShow_Button;

    QLineEdit* m_pSendText_LineEdit;
    QPushButton* m_pSendData_Button;

    // 导出文件
    QPushButton* m_pExportPath_Button;
    QLineEdit* m_pExportFileShow_LineEdit;
    QCheckBox* m_pAllowExport_CheckBox;
    QCheckBox* m_pExportUseTime_CheckBox;

    // 参考输入
    QGroupBox* m_pRefInput_Group;
    QLabel* m_p1PPSLVDS_Label;

private:
    // 正则表达式
    QRegExpValidator* m_pIP_RegExpVal;
    QRegExpValidator* m_pNum_RegExpVal;
    QRegExpValidator* m_p0x16_RegExpVal;

    QSignalMapper* m_pSignalMapper;
    QTimer* m_pTimer;

    void* m_hExportFile;                // 导出文件
    void* m_hCmdLogFile;                // 命令下发 日志文件

private:
    CMyComBasic* m_pMyComBasic;

    bool m_bComIsOpened;
    bool m_bNetIsOpened;
    bool m_bUseIsCom;

    int m_iRecvShowType;                // 处理由于接收数据的某个长度引起的显示变化

    bool m_bIsEncrypt;                  // 是否是“加密”
    bool m_bUseFileName;                // 是否使用“文件名”逻辑
    QString m_strFileNameUse;
    
    int m_iMultiFrame;                  // 多帧指示，如 0x21, 0x22：表示总共有两帧，当前是第几帧
    bool m_bCmdIsNavtfIsOne;            // navtf2000命令：是否有效
    bool m_bCmdIsNavtfFinished;

    bool m_bSomeFuncEnabled;            // 某些功能是否有效

private:
    CMyImageTab* m_pMyImageTab;
    CStatusTab* m_pStatusTab;
    CMyAdjustTab* m_pMyAdjustTab;
    CGNSSTab* m_pGNSSTab;

private:            // 升级/下载
    volatile int m_iTimeOut;
    int m_iTimeOut_Save;
    int m_iReDownCount_Save;

    volatile bool m_bStopDownload;      // 主动停止下载

    QString m_strFile;                  // 下载文件名
    QString m_strAutoPath;              // 自动升级目录

    bool m_bSingleDebug;                // 是否是单步下载
    int m_iSingleNum;                   // 单步下载指示

private:        // Command
    st_CommandItem* m_pCmdItem_Head;    // 头指针
    st_CommandItem* m_pCmdItem_Cur;     // 当前指针

    typedef std::map<std::string, st_BoardVerInfo*> BOARDVERINFOMAPTYPE;    // key = name
    BOARDVERINFOMAPTYPE m_BoardVerInfo_map;

    typedef std::map<std::string, std::string> BOARDNAMETYPE;               // key = name, 板卡识别名称对应可读中文名称
    BOARDNAMETYPE m_BoardName_map;

    char* m_pAutoDownloadReport[NUM_OF_AUTODOWN_REPORT];                    // 自动升级使用：详细报告
    char* m_pAutoUpdateSimpleReport[NUM_OF_AUTODOWN_REPORT];                // 自动升级使用：简略报告
    en_OpType m_enOpType;
};

#endif
