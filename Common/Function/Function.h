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
    // 将一个16进制的值转成ASCII，并带一个空格
    static void Change16ToAscii(int iData, unsigned char* pszResult, int iSize);

public:     // Sys INI Function ==> 串口设置
    // 串口号
    static void SetComIndex(int iIndex);
    static int GetComIndex();

    // 波特率
    static void SetComBaud(int iBaud);
    static int GetComBaud();

    // 数据位
    static void SetComDataBit(int iData);
    static int GetComDataBit();

    // 停止位
    static void SetComStopBit(int iStop);
    static int GetComStopBit();

    // 校验位
    static void SetComCheckBit(int iCheck);
    static int GetComCheckBit();

    // 流控制
    static void SetComStreamCtrl(int iCtrl);
    static int GetComStreamCtrl();

    // 源地址
    static void SetComSvrAddr(int iAddr);
    static int GetComSvrAddr();

    // 源端口
    static void SetComSvrPort(int iPort);
    static int GetComSvrPort();

    // 备用1
    static void SetComBackup1(int iBackup);
    static int GetComBackup1();

    // 目的地址
    static void SetComDestAddr(int iAddr);
    static int GetComDestAddr();

    // 目的端口
    static void SetComDestPort(int iPort);
    static int GetComDestPort();

    // 备用2
    static void SetComBackup2(int iBackup);
    static int GetComBackup2();

    // 命令字
    static void SetComCommand(int iCommand);
    static int GetComCommand();

    // 帧号
    static void SetComNumber(int iNum);
    static int GetComNumber();

    // 周期
    static void SetComCircle(int iCircle);
    static int GetComCircle();

    // 自动成帧
    static void SetAutoMakeFrame(int iAuto);
    static int GetAutoMakeFrame();

    // 发16进制
    static void SetSend16Hex(int i16Hex);
    static int GetSend16Hex();

    // 收16进制
    static void SetRecv16Hex(int i16Hex);
    static int GetRecv16Hex();

    // 发送字符串
    static void SetComSendData(const char* pchSendData);
    static void GetComSendData(char* pchSendData, int iLength);

    // 超时ms
    static void SetDownloadTimeoutMS(int iMS);
    static int GetDownloadTimeoutMS();

    // 超时次数
    static void SetDownloadTimeoutCount(int iMS);
    static int GetDownloadTimeoutCount();

    // 是否是“加密”
    static void SetEncryptData(int iEncrypt);
    static int GetEncryptData();

    // 发送数据时是否追加“回车”
    static void SetEnterForSend(int iEnter);
    static int GetEnterForSend();

    // UDP 地址
    static void SetUDPAddr(const char* pchSendData);
    static void GetUDPAddr(char* pchSendData, int iLength);

    // UDP 端口
    static void SetUDPPort(int iPort);
    static int GetUDPPort();

    // 网口、串口选择
    static void SetComNetSelect(int iSelect);
    static int GetComNetSelect();

    // 下载文件的路径
    static void SetDownloadFilePath(const char* pchFilePath);
    static void GetDownloadFilePath(char* pchFilePath, int iLength);

    // 导出文件的路径
    static void SetExportFilePath(const char* pchFilePath);
    static void GetExportFilePath(char* pchFilePath, int iLength);

    // 允许导出文件
    static void SetAllowExportFile(int iAllow);
    static int GetAllowExportFile();

    // 导出文件加入时间
    static void SetExportUseTime(int iUse);
    static int GetExportUseTime();

    // 选择单板还是系统(多板)
    static void SetUpdateSelect(int iSelect);
    static int GetUpdateSelect();

public:     // INI ==> CNAU
    // 文件版本
    static void SetCNAUFileVersion(float fVer);
    static float GetCNAUFileVersion();

    // 文件路径
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


public:     // 系统的计时
    // 单线程
    static void HighTimeBegin();
    static void HighTimeEnd();
    static double GetHighTime();

    // 多线程
    static LONGLONG GetHighTimeCounter();
    static double GetHighTime(LONGLONG llBegin, LONGLONG llEnd);

public:
    // 获取程序所在路径: 形式 d:/work/mms/
    static void GetMoudlePath(char* pszPath, int iSize);

    // 获取路径下找到的第一个文件名称，短名称，指定扩展名
    static bool GetFirstFileOnFolder(char* pszFilePath_in, char* pszFilePath_out, int iSize);

    static bool FolderExist(const char* pszFolder);
    static bool CreateFolder(const char* pszFolder);

    static unsigned short GetCRC16(unsigned char* cp,unsigned int leng);

    // 自动升级：获取 目录下形如 Ref_Input(_Vx.y.bin) 文件的版本号。pszFolder 后面带 '/'
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

public:     // 功能选择文件 解析
    static bool ParseFileToPageCheck(char* pszFilePath, st_PageCheck& oPageCheck);
    static char* ReadOneLine(char* ptrBegin, char* ptrOut, int iOutSize);
    static void ReadTwoNumOnLine(char* ptrData, int& iNum0, int& iNum1);
    static void ReadThreeNumOnLine(char* ptrData, int& iNum0, int& iNum1, int& iNum2);
    static char* ReadOneNum(char* ptrData, int& iNum);

    // 添加一页
    static void AddOnePage(st_PageCheck& oPageCheck, int iPageIndex);

    // 添加一页一项
    static void AddOnePageItem(st_PageCheck& oPageCheck, int iPageIndex, int iPageItemIndex, bool bUse);

    // 将某页的所有的检测子项都添加进去
    static void AddItemDataOfPage(st_PageData& oPD);


public:
    static void ExecCommand(char* pszCmd);

private:
    static double m_stdSysFreq;     // 系统的计时器的频率
    static LONGLONG m_stllCounterBegin;
    static LONGLONG m_stllCounterEnd;

    // 程序路径
    static char m_stchMoudlePath[MAX_PATH];

};


#endif