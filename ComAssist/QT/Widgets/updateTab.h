#ifndef _UPDATETAB_H_
#define _UPDATETAB_H_

#include <QWidget>

#include <string>
#include <map>

// --------------------------------------------------------------------------------------
// ͨ��/���� tab (�豸ά��)
// --------------------------------------------------------------------------------------
#define SIZE_WIDTH_OF_LABEL             50      // ��ǩ������ֵ
#define SIZE_WIDTH_OF_EDIT              100     // ����������ֵ
#define SIZE_WIDTH_OF_BUTTON_RIGHT      300     // �ұ߰����Ŀ��
#define SIZE_HEIGHT_OF_BUTTON_RIGHT     40      // �ұ߰����ĸ߶�

// ����Ҫ������������
#define ID_OF_IP_ADDRESS            0       // IP��ַ
#define ID_OF_IP_PORT               1       // IP�˿�

#define ID_OF_SOURCE_ADDR           2       // Դ��ַ
#define ID_OF_SOURCE_PORT           3       // Դ�˿�
#define ID_OF_SOURCE_USE            4       // ����1

#define ID_OF_TARGET_ADDR           5       // Ŀ�ĵ�ַ
#define ID_OF_TARGET_PORT           6       // Ŀ�Ķ˿�
#define ID_OF_TARGET_USE            7       // ����2

#define ID_OF_FRAME_NUM             8       // ֡ ��
#define ID_OF_FRAME_COMMAND         9       // ������

#define ID_OF_RESEND_CIRCLE         10      // �Զ��ط�����

#define ID_OF_TIMEOUT_MS            11      // ��ʱms
#define ID_OF_TIMEOUT_COUNT         12      // ��ʱ����


// ѡ�����ڻ��Ǵ���
#define COMNET_SELECT_IS_COM    0
#define COMNET_SELECT_IS_NET    1

// ѡ�񵥰廹��ϵͳ(���)
#define UPDATE_SELECT_IS_ALONE  0
#define UPDATE_SELECT_IS_SYSTEM 1

// �Զ������ļ�Ŀ¼
#define NAME_OF_AUTO_UPDATE_DIR     "Updata_Soft"

// �汾��ѯ
#define NAME_OF_VERSION_ONLYONE     "�����忨"
#define NAME_OF_VERSION_ALL         "ȫ���忨"

// �Զ����ر��浥������
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
//��ǰִ�еĲ���
enum en_OpType
{
    en_Op_Is_NULL = 0,          // �޲���
    en_Op_Is_Ver_One,           // ��ѯĳ���汾
    en_Op_Is_Ver_More,          // ��ѯ����汾
    en_Op_Is_Update_One,        // ����ĳ���汾
    en_Op_Is_Auto_Download,     // �Զ�����
};



// --------------------------------------------------------------------------------------
enum en_CmdAType         // ����
{
    en_Cmd_A_Is_Version = 0,        // �汾��ѯ
    en_Cmd_A_Is_Update,             // ��������
    en_Cmd_A_Is_Delay,              // ��ʱ
};
enum en_CmdBTyep        // С��
{
    en_Cmd_B_Ver_Is_One = 0,        // ��ѯһ���汾
    en_Cmd_B_Ver_Is_More,           // ��ѯ����汾

    en_Cmd_B_Down_Is_One,           // ����һ���忨
    en_Cmd_B_Down_Is_More,          // ��������忨

    en_Cmd_B_Delay_Is_One,          // ��ʱһ��
    en_Cmd_B_Delay_Is_More,         // ��ʱ���

    en_Cmd_B_Send_One_Cmd,          // �·�һ������
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
    en_CmdAType m_enFrameAType;     // ����
    en_CmdBTyep m_enFrameBType;     // С��

    unsigned char m_chSourceAddr;   // Դ��Ŀ�ĺ�����
    unsigned char m_chSourcePort;
    unsigned char m_chSourceUse;
    unsigned char m_chTargetAddr;
    unsigned char m_chTargetPort;
    unsigned char m_chTargetUse;
    unsigned char m_chFrameCmd;

    unsigned char m_chATCmd[3];     // �汾��ѯ����

    bool m_bTimeOut;                // �Ƿ��ǳ�ʱ
    bool m_bManual;                 // �Ƿ����ֶ����������ֶ���ѯ�汾���ֶ�������

    int m_iDelayCount;              // ��ʱ����ʹ��(������)
    char* m_pReportBuf;             // �Զ�����ʹ�ã���ϸ����
    char* m_pSimpleReportBuf;       // �Զ�����ʹ�ã����Ա���

    QString m_strFilePath;          // �������أ�Ŀ¼�ļ���
    FILE* m_pFile;                  // �ļ����
    int m_iTotalPaks;               // �ܰ����Ͱ���(���Ŵ�1��ʼ)
    int m_iPaksIndex;
    unsigned char m_chCurCommand;   // ��ǰ�·�����
    volatile int m_iReDownCount;    // �ط�����
};

// �忨�汾��Ϣ
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

    std::string m_strName;          // ����
    std::string m_strOriName;       // ԭʼ���ƣ�û�о����޸�
    unsigned char m_chAddr;         // ��ַ
    unsigned char m_chPort;
    unsigned char m_chUse;
    float m_fVer;                   // �汾
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

    // �رմ���ʱ���õĺ���
    void CloseFunction();

    void Set_MyImageTab(CMyImageTab* pMyImageTab);
    void Set_StatusTab(CStatusTab* pStatusTab);
    void Set_MyAdjustTab(CMyAdjustTab* pMyAdjustTab);
    void Set_GNSSTab(CGNSSTab* pGNSSTab);

    bool ComNetIsOpen();

public:     // ���ʹ��ͨ�Ź���
    // ����������Դ��ַ����Ϣ
    void SetSourceTargetInfo(int iCmd);

    // ����Դ��ַ
    void SetSourceAddr(int iAddr, int iPort, int iResv);

    // ����Ŀ�ĵ�ַ
    void SetTargetAddr(int iAddr, int iPort, int iResv);

    // ����������
    void SetCommand(int iCmd);

    // ��������͡���ť���������������Ƿ���16����; ��������ఴť��ǿ������Ϊ16����
    // ��������͡���ť���������������Ƿ���֡ͷ֡β; ��������ఴť��ǿ������Ϊ��֡ͷ֡β
    void SetFrameDataStyle(bool bManualSet, bool bSendIs16Hex, bool bRecvIs16Hex, bool bHasFrame);

    // ��������
    void SendComNetData(unsigned char chCommand, const char* pszDataBuf, int iDataLength_in);

    // ȫ���忨 �汾��ѯ
    void VersionQueryAllBoard();

    // ǿ����������=1����ѡ���Զ���֡������ѡ���Զ��ط���������16���ơ�������16���ơ�
    void SetCmdAndOptionManu();

    //  �� �ο������ �� ���ذ� ���� getsta ����
    void Send_getsta_To_RInputAndMain();

    // ѡ��Ƶ������
    void SendFreqSelect(int iIndex);

    // ѡ����������
    void SendPlusSelect(int iIndex);

    // ���� Ŀ���ַ�ַ������� ����
    void SendChangeData(unsigned char chAddr, unsigned char chPort, char* pchData);

    // ��ȡ������Ŀ�ĵ�ַ
    void GetTargetForGNSS(QString&, QString&);
    void SetTargetForGNSS(QString, QString);

    // �����豸���ź�״̬
    void SetDeviceSignalStatus(int iValue);

    // ĳ����ַ�İ忨�Ƿ��Ѿ���ѯ��
    bool BoardIsValid(unsigned char chAddr, unsigned char chPort);

    // ���ݵ�ַ��ȡ�忨��ԭʼ����
    std::string GetBoardOriName(unsigned char chAddr, unsigned char chPort);

public:
    // ��ʾ��������
    void ShowRecvDataFunc(unsigned char chAddr, unsigned char chPort, const char* pszShowData, bool, int iR, int iG, int iB);
    void SetRecvShowFunc(int iShowType);

    // ��ʾ��������
    void ShowSendDataFunc(unsigned char chAddr, unsigned char chPort, const char* pszShowData, int iLength);

    // ��ʾ��ʾ��Ϣ
    void ShowTipInfoFunc(const char* pszShowData, QColor rgb = QColor(0, 0, 0));

    // �����·� ������־
    void CmdLog_Func(unsigned char, unsigned char, QString&);

    // �ɰ忨��ַ��ȡ�忨����
    QString GetBoardNameOnAddr(unsigned int iAddr, unsigned int iPort);

public:     // ����
    // �������س�ʱ����
    void DownloadTimeoutFunc();

    // ֡���ݴ�����
    void FrameHanleFunc(const st_FrameData* pFrameData);

    // ֡���ݴ����̵߳��ú���
    void HandleFrameData();

    // �ط���֡
    void ReDownloadFunc(QString strError);

private:     // ����
    // �����ص�����
    void HandleRecvCommand(unsigned char chCommand, unsigned char chHighPak, unsigned char chLowPak, unsigned char chParam);

    // ���� ׼������ ����
    void SendPrepareUpdate();

    // ����һ֡(�������ط�)
    void SendOneFrame(unsigned char chCommand, unsigned char* pchData, int iDataSize, bool isReSend = false);

    // ����һ������
    void SendNextOnePak();

    // ��λ ��������
    void DownloadReset();

    // �˳� ����
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

    // �������
    void CmdFinished_Func();

    // ���͵�ǰ����ͷ��
    void DownloadSendCurCmdFirstPack();

    // �·������������Ƿ�ɹ�
    void CheckConnect();

    // ����·�������ķ��ؽ��
    void CheckSendedOneCmd(const char* pszInput, int iMultiFrame);
    bool ParseInputString(const char* pszInput, int iCheckNum, char* pszOut1, char* pszOut2, char* pszOut3, char* pszOut4);

    // ����һ���忨
    void AddOneBoard(char* chName, int iNameLength, unsigned char chAddr, unsigned char chPort, float fVer);

private slots:
    // ��/�رմ���(�����봮�ڻ���)
    void OpenCloseCom();

    // ��/�ر�����(�����봮�ڻ���)
    void OpenCloseNet();

    // ���ڸı�
    void ComNameChanged(const QString& text);

    // ��������������
    void SaveInput(int iID);

    // ѡ�����ڻ��Ǵ���
    void ComNetChanged(int);
    void SelectCom_Slot();
    void SelectNet_Slot();

    // �� ��
    void SendData();

    // �� ��
    void ClearShowInfo();

    // ��ʾ
    void ShowRecvText_Slot(unsigned char, unsigned char, const QString, bool, int, int, int);
    void ShowSendText_Slot(unsigned char chAddr, unsigned char chPort, const QString);
    void ShowTipText_Slot(const QString, QColor);

    // Scroll
    void ShowRecvAutoScroll_Slot();
    void ShowTipAutoScroll_Slot(); 

    // �ؼ�
    void FrameNoInc_Slot(int);
    void BoardSelectFill_Slot();

    // �������
    void CmdFinished_Slot();

    // ѡ��Զ��ط����Զ���֡��16���ơ�����
    void AutoReSendClicked();
    void RecvAllClicked();
    void AutoMakeFrameClicked();
    void CheckSend16DataClicked();
    void CheckRecv16DataClicked();
    void CheckDebugClicked();

    // �����Ƿ�׷�ӡ��س���
    void EnterClicked();

    // �ط���ʱ��
    void ReSendTimeReach();

    // ���ļ�
    void OpenFilePath();

    // �汾��ѯ
    void VersionQuery();

    // ��ʼ����
    void StartUpdate();

    // ֹͣ����
    void StopUpdate();

    // �Զ�����
    void AutoUpdate();

    // ѡ�񵼳��ļ�
    void SelectExportFilePath();

    // ������������
    void NetConnectTestFunc();

    // ѡ�񵥰廹��ϵͳ(���)
    void BoardTypeChanged(int);

    // ѡ��ĳ���忨
    void BoardSelect_Slot(const QString&);

    // ��ȡ�豸�ź�״̬
    void DeviceSignalStatus_Slot(int);

    // �����·� ������־
    void CmdLog_Slot(unsigned char, unsigned char, QString);

    void CheckSendedOneCmd_Slot(const QString, int);

private:
    void CreateWidgets();

    // ͨ������
    QVBoxLayout* CreateCommSet();
    QGroupBox* CreateComInfo();
    QGroupBox* CreateNetInfo();

    // Դ��ַ��Ŀ�ĵ�ַ��֡�ź�������
    QGroupBox* CreateSourceAddr();
    QGroupBox* CreateFrameCmd();

    // �����ļ�
    QGroupBox* CreateExportGroup();

    // ѡ��ͷ��͵�
    QGroupBox* CreateClear();
    QGroupBox* CreateOption();
    QGroupBox* CreateSend();

    // �������
    QVBoxLayout* CreateUpdate();
    QGroupBox* CreateManuUpdate();

    // �忨ѡ��
    QGroupBox* CreateBoardSelectGroup();

    // ��ʾ��
    QVBoxLayout* CreateShow();


    // �ο�����
    QVBoxLayout* CreateRefStatus();

    // ������ǹ���Ա�汾����Ҫ���볬�������������ĳЩ����
    void SetSomeFuncEnabled(bool bEnable);

private:
    // ��ʼ����������
    void InitComInfo();

    // �رմ���
    void CloseCom();

    // �ر�����
    void CloseNet();

    // ���ð�ť״̬(��Զ˿��Ƿ��)
    void SetFuncButtonEnable(bool bEnable);

    // ��ʾ�����ļ�������
    void ShowUpdateFile();
    void Change16ToAscii8Bits(int iData, QString& strResult);

    // ��ȡ·�����ļ�����
    QString GetShortFileOnPath(const char* pszPath);

    // ��ȡ Դ��Ŀ�ĺ������
    unsigned char GetSourceAddr();
    unsigned char GetSourcePort();
    unsigned char GetSourceUse();
    unsigned char GetTargetAddr();
    unsigned char GetTargetPort();
    unsigned char GetTargetUse();
    unsigned char GetFrameNo();

    void FrameNoInc(unsigned char chCurFrameNo);

    void BoardSelectFill();

    // ִ������ʱ��ֹ��������
    void SetAllEnabled(bool bEnable);

    // ֹͣ �Զ��ط�
    void StopAutoReSend();

    // �򿪺͹رյ����ļ�
    void CloseExportFile();
    void OpenExportFile(char* pszFilePath);


signals:
    // ��ʾ
    void ShowRecvText_Signal(unsigned char, unsigned char, const QString, bool, int, int, int);
    void ShowSendText_Signal(unsigned char chAddr, unsigned char chPort, const QString);
    void ShowTipText_Signal(const QString, QColor);

    // �ؼ�
    void FrameNoInc_Signal(int);
    void BoardSelectFill_Signal();

    // �������
    void CmdFinished_Signal();

    // ��ȡ�豸�ź�״̬
    void DeviceSignalStatus_Signal(int);

    // �����·� ������־
    void CmdLog_Signal(unsigned char, unsigned char, QString);

    void CheckSendedOneCmd_Signal(const QString, int);

private:
    // ��������
    QComboBox* m_pComName_ComboBox;
    QComboBox* m_pComBaud_ComboBox;
    QComboBox* m_pComDataBit_ComboBox;
    QComboBox* m_pComStopBit_ComboBox;
    QComboBox* m_pComCheck_ComboBox;
    QLabel* m_pComStatus_Label;
    QPushButton* m_pComOpen_Button;
    QGroupBox* m_pSelectCom_GroupBox;

    // ��������
    QLineEdit* m_pNetIPAddr_LineEdit;
    QLineEdit* m_pNetIPPort_LineEdit;
    QLabel* m_pNetStatus_Label;
    QPushButton* m_pNetOpen_Button;
    QGroupBox* m_pSelectNet_GroupBox;

    QPushButton* m_pNetConnectTest_Button;

    // Դ��ַ��Ŀ�ĵ�ַ��֡�ź�������
    QLineEdit* m_pSourceAddr_LineEdit;
    QLineEdit* m_pSourcePort_LineEdit;
    QLineEdit* m_pSourceResv_LineEdit;

    QLineEdit* m_pTargetAddr_LineEdit;
    QLineEdit* m_pTargetPort_LineEdit;
    QLineEdit* m_pTargetResv_LineEdit;

    QLineEdit* m_pFrameNo_LineEdit;
    QLineEdit* m_pFrameCmd_LineEdit;

    // ��Ϣ��ʾ
    QTextEdit* m_pRecvShow_TextEdit;
    QTextEdit* m_pTipShow_TextEdit;

    // �ֶ�����
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

    // �ײ�
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

    // �����ļ�
    QPushButton* m_pExportPath_Button;
    QLineEdit* m_pExportFileShow_LineEdit;
    QCheckBox* m_pAllowExport_CheckBox;
    QCheckBox* m_pExportUseTime_CheckBox;

    // �ο�����
    QGroupBox* m_pRefInput_Group;
    QLabel* m_p1PPSLVDS_Label;

private:
    // ������ʽ
    QRegExpValidator* m_pIP_RegExpVal;
    QRegExpValidator* m_pNum_RegExpVal;
    QRegExpValidator* m_p0x16_RegExpVal;

    QSignalMapper* m_pSignalMapper;
    QTimer* m_pTimer;

    void* m_hExportFile;                // �����ļ�
    void* m_hCmdLogFile;                // �����·� ��־�ļ�

private:
    CMyComBasic* m_pMyComBasic;

    bool m_bComIsOpened;
    bool m_bNetIsOpened;
    bool m_bUseIsCom;

    int m_iRecvShowType;                // �������ڽ������ݵ�ĳ�������������ʾ�仯

    bool m_bIsEncrypt;                  // �Ƿ��ǡ����ܡ�
    bool m_bUseFileName;                // �Ƿ�ʹ�á��ļ������߼�
    QString m_strFileNameUse;
    
    int m_iMultiFrame;                  // ��ָ֡ʾ���� 0x21, 0x22����ʾ�ܹ�����֡����ǰ�ǵڼ�֡
    bool m_bCmdIsNavtfIsOne;            // navtf2000����Ƿ���Ч
    bool m_bCmdIsNavtfFinished;

    bool m_bSomeFuncEnabled;            // ĳЩ�����Ƿ���Ч

private:
    CMyImageTab* m_pMyImageTab;
    CStatusTab* m_pStatusTab;
    CMyAdjustTab* m_pMyAdjustTab;
    CGNSSTab* m_pGNSSTab;

private:            // ����/����
    volatile int m_iTimeOut;
    int m_iTimeOut_Save;
    int m_iReDownCount_Save;

    volatile bool m_bStopDownload;      // ����ֹͣ����

    QString m_strFile;                  // �����ļ���
    QString m_strAutoPath;              // �Զ�����Ŀ¼

    bool m_bSingleDebug;                // �Ƿ��ǵ�������
    int m_iSingleNum;                   // ��������ָʾ

private:        // Command
    st_CommandItem* m_pCmdItem_Head;    // ͷָ��
    st_CommandItem* m_pCmdItem_Cur;     // ��ǰָ��

    typedef std::map<std::string, st_BoardVerInfo*> BOARDVERINFOMAPTYPE;    // key = name
    BOARDVERINFOMAPTYPE m_BoardVerInfo_map;

    typedef std::map<std::string, std::string> BOARDNAMETYPE;               // key = name, �忨ʶ�����ƶ�Ӧ�ɶ���������
    BOARDNAMETYPE m_BoardName_map;

    char* m_pAutoDownloadReport[NUM_OF_AUTODOWN_REPORT];                    // �Զ�����ʹ�ã���ϸ����
    char* m_pAutoUpdateSimpleReport[NUM_OF_AUTODOWN_REPORT];                // �Զ�����ʹ�ã����Ա���
    en_OpType m_enOpType;
};

#endif
