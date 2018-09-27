#ifndef _FUNCTION_H_
#define _FUNCTION_H_

#include <vector>
#include <string>

#include <qglobal.h>
#include <QDate>
#include <QStringList>

// --------------------------------------------------------------------------------------
// Function
// --------------------------------------------------------------------------------------
struct st_PageCheck;
struct st_PageData;


// --------------------------------------------------------------------------------------
struct stTimeData
{
    unsigned short wYear;
    unsigned short wMonth;
    unsigned short wDayOfWeek;
    unsigned short wDay;
    unsigned short wHour;
    unsigned short wMinute;
    unsigned short wSecond;
    unsigned short wMilliseconds;
};

#ifndef LONGLONG
typedef __int64 LONGLONG;
#endif


#ifndef MAX_PATH
#define MAX_PATH 260
#endif

// --------------------------------------------------------------------------------------
class CFuncTool
{
public:
    CFuncTool();
    ~CFuncTool();

public:
    static void Init();
    static void UnInit();

public:
    static void GetLocalCurTime(stTimeData& timeData);

public:
    // ��һ��16���Ƶ�ֵת��ASCII������һ���ո�
    static void Change16ToAscii(int iData, unsigned char* pszResult, int iSize);

public:     // Sys INI Function ==> ��������
    // ���ں�
    static void SetComIndex(int iIndex);
    static int GetComIndex();

    // ������
    static void SetComBaud(int iBaud);
    static int GetComBaud();

    // ����λ
    static void SetComDataBit(int iData);
    static int GetComDataBit();

    // ֹͣλ
    static void SetComStopBit(int iStop);
    static int GetComStopBit();

    // У��λ
    static void SetComCheckBit(int iCheck);
    static int GetComCheckBit();

    // ������
    static void SetComStreamCtrl(int iCtrl);
    static int GetComStreamCtrl();

    // Դ��ַ
    static void SetComSvrAddr(int iAddr);
    static int GetComSvrAddr();

    // Դ�˿�
    static void SetComSvrPort(int iPort);
    static int GetComSvrPort();

    // ����1
    static void SetComBackup1(int iBackup);
    static int GetComBackup1();

    // Ŀ�ĵ�ַ
    static void SetComDestAddr(int iAddr);
    static int GetComDestAddr();

    // Ŀ�Ķ˿�
    static void SetComDestPort(int iPort);
    static int GetComDestPort();

    // ����2
    static void SetComBackup2(int iBackup);
    static int GetComBackup2();

    // ������
    static void SetComCommand(int iCommand);
    static int GetComCommand();

    // ֡��
    static void SetComNumber(int iNum);
    static int GetComNumber();

    // ����
    static void SetComCircle(int iCircle);
    static int GetComCircle();

    // �Զ���֡
    static void SetAutoMakeFrame(int iAuto);
    static int GetAutoMakeFrame();

    // ��16����
    static void SetSend16Hex(int i16Hex);
    static int GetSend16Hex();

    // ��16����
    static void SetRecv16Hex(int i16Hex);
    static int GetRecv16Hex();

    // �����ַ���
    static void SetComSendData(const char* pchSendData);
    static void GetComSendData(char* pchSendData, int iLength);

    // ��ʱms
    static void SetDownloadTimeoutMS(int iMS);
    static int GetDownloadTimeoutMS();

    // ��ʱ����
    static void SetDownloadTimeoutCount(int iMS);
    static int GetDownloadTimeoutCount();

    // �Ƿ��ǡ����ܡ�
    static void SetEncryptData(int iEncrypt);
    static int GetEncryptData();

    // ��������ʱ�Ƿ�׷�ӡ��س���
    static void SetEnterForSend(int iEnter);
    static int GetEnterForSend();

    // UDP ��ַ
    static void SetUDPAddr(const char* pchSendData);
    static void GetUDPAddr(char* pchSendData, int iLength);

    // UDP �˿�
    static void SetUDPPort(int iPort);
    static int GetUDPPort();

    // ���ڡ�����ѡ��
    static void SetComNetSelect(int iSelect);
    static int GetComNetSelect();

    // �����ļ���·��
    static void SetDownloadFilePath(const char* pchFilePath);
    static void GetDownloadFilePath(char* pchFilePath, int iLength);

    // �����ļ���·��
    static void SetExportFilePath(const char* pchFilePath);
    static void GetExportFilePath(char* pchFilePath, int iLength);

    // �������ļ�
    static void SetAllowExportFile(int iAllow);
    static int GetAllowExportFile();

    // �����ļ�����ʱ��
    static void SetExportUseTime(int iUse);
    static int GetExportUseTime();

    // ѡ�񵥰廹��ϵͳ(���)
    static void SetUpdateSelect(int iSelect);
    static int GetUpdateSelect();

public:     // INI ==> CNAU
    // �ļ��汾
    static void SetCNAUFileVersion(float fVer);
    static float GetCNAUFileVersion();

    // �ļ�·��
    static void SetCNAUFilePath(char* pszFilePath);
    static void GetCNAUFilePath(char* pszFilePath, int iSize);

public:     // Sys INI Function ==> Common
    // window
    static void SetWindowWidth_SysINI(int iWidth);
    static int GetWindowWidth_SysINI();
    static void SetWindowHeight_SysINI(int iHeight);
    static int GetWindowHeight_SysINI();
    static void SetWindowMax_SysINI(bool bMaximized);
    static bool GetWindowMax_SysINI();


public:     // ϵͳ�ļ�ʱ
    // ���߳�
    static void HighTimeBegin();
    static void HighTimeEnd();
    static double GetHighTime();

    // ���߳�
    static LONGLONG GetHighTimeCounter();
    static double GetHighTime(LONGLONG llBegin, LONGLONG llEnd);

public:
    // ��ȡ��������·��: ��ʽ d:/work/mms/
    static void GetMoudlePath(char* pszPath, int iSize);

    // ��ȡ·�����ҵ��ĵ�һ���ļ����ƣ������ƣ�ָ����չ��
    static bool GetFirstFileOnFolder(char* pszFilePath_in, char* pszFilePath_out, int iSize);

    static bool FolderExist(const char* pszFolder);
    static bool CreateFolder(const char* pszFolder);

    static unsigned short GetCRC16(unsigned char* cp,unsigned int leng);

    // �Զ���������ȡ Ŀ¼������ Ref_Input(_Vx.y.bin) �ļ��İ汾�š�pszFolder ����� '/'
    static bool GetVerOnDirAndShortName(const char* pszFolder, const char* pszShortName, float& fVer);

public:     // Crypt
    static void SetCryptKey(quint64 key);

    static QString EncryptToString(const QString& plaintext);
    static QString EncryptToString(QByteArray plaintext);
    static QByteArray EncryptToByteArray(const QString& plaintext);
    static QByteArray EncryptToByteArray(QByteArray plaintext);

    static QString DecryptToString(const QString& cyphertext);
    static QString DecryptToString(QByteArray cypher);
    static QByteArray DecryptToByteArray(const QString& cyphertext);
    static QByteArray DecryptToByteArray(QByteArray cypher);

    static int GetCryptLastError();

public:     // ����ѡ���ļ� ����
    static bool ParseFileToPageCheck(char* pszFilePath, st_PageCheck& oPageCheck);
    static char* ReadOneLine(char* ptrBegin, char* ptrOut, int iOutSize);
    static void ReadTwoNumOnLine(char* ptrData, int& iNum0, int& iNum1);
    static void ReadThreeNumOnLine(char* ptrData, int& iNum0, int& iNum1, int& iNum2);
    static char* ReadOneNum(char* ptrData, int& iNum);

    // ���һҳ
    static void AddOnePage(st_PageCheck& oPageCheck, int iPageIndex);

    // ���һҳһ��
    static void AddOnePageItem(st_PageCheck& oPageCheck, int iPageIndex, int iPageItemIndex, bool bUse);

    // ��ĳҳ�����еļ�������ӽ�ȥ
    static void AddItemDataOfPage(st_PageData& oPD);


public:
    static void ExecCommand(char* pszCmd);

private:
    static double m_stdSysFreq;     // ϵͳ�ļ�ʱ����Ƶ��
    static LONGLONG m_stllCounterBegin;
    static LONGLONG m_stllCounterEnd;

    // ����·��
    static char m_stchMoudlePath[MAX_PATH];

};


#endif