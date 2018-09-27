#include "Function.h"

#include <WinSock2.h>               // for WSAStartup
#include <Windows.h>
#include <stdio.h>

#include <QApplication>
#include "..//Define/DefineFile.h"

#include "simplecrypt.h"

// --------------------------------------------------------------------------------------
#pragma comment(lib, "ws2_32.lib")

// --------------------------------------------------------------------------------------
double CFuncTool::m_stdSysFreq = 0.0;
LONGLONG CFuncTool::m_stllCounterBegin = 0;
LONGLONG CFuncTool::m_stllCounterEnd = 0;

char CFuncTool::m_stchMoudlePath[MAX_PATH] = {0};

static SimpleCrypt g_SimpleCrypt;

// --------------------------------------------------------------------------------------
// 获取功能页名称
QString GetPageNameOnIndex(int iIndex)
{
    if(PAGE_INDEX_OF_UPDATE == iIndex)
    {
        return QApplication::tr("%1_通信/升级").arg(iIndex);
    }
    if(PAGE_INDEX_OF_SIGNAL == iIndex)
    {
        return QApplication::tr("%1_信号选择").arg(iIndex);
    }
    if(PAGE_INDEX_OF_STATUS == iIndex)
    {
        return QApplication::tr("%1_状态监控").arg(iIndex);
    }
    if(PAGE_INDEX_OF_ADJUST == iIndex)
    {
        return QApplication::tr("%1_设备调整").arg(iIndex);
    }
    if(PAGE_INDEX_OF_GNSS == iIndex)
    {
        return QApplication::tr("%1_GNSS").arg(iIndex);
    }
    return QApplication::tr("未知名称");
}

// 获取功能项名称
QString GetPageItemNameOnIndex(int iIndex)
{
    if(PAGE_UPDATE_ENTER_CHECK == iIndex)
    {
        return QApplication::tr("%1_回车").arg(iIndex);
    }
    //if(PAGE_UPDATE_ALONE_OR_SYSTEM == iIndex)
    //{
    //    return QApplication::tr("%1_单板或者系统").arg(iIndex);
    //}
    if(PAGE_UPDATE_REF_STATUS == iIndex)
    {
        return QApplication::tr("%1_参考输入").arg(iIndex);
    }
    if(PAGE_UPDATE_EXPORT_FILE == iIndex)
    {
        return QApplication::tr("%1_导出文件(管理权限)").arg(iIndex);
    }
    if(PAGE_SIGNAL_INPUT_CTRL == iIndex)
    {
        return QApplication::tr("%1_数值修改和显示").arg(iIndex);
    }
    if(PAGE_SIGNAL_PRIORITY == iIndex)
    {
        return QApplication::tr("%1_优先级配置和显示").arg(iIndex);
    }
    return QApplication::tr("未知名称");
}

// --------------------------------------------------------------------------------------
// Function
// --------------------------------------------------------------------------------------
CFuncTool::CFuncTool()
{

}
CFuncTool::~CFuncTool()
{

}

void CFuncTool::Init()
{
    // Time
    LARGE_INTEGER liTime;
    ::QueryPerformanceFrequency(&liTime);
    m_stdSysFreq = (double)liTime.QuadPart;

    // Path
    char szCurPath[MAX_PATH] = {0};
    ::GetModuleFileName(NULL,szCurPath,sizeof(szCurPath));

    char* ptr2 = NULL;
    char* ptr = ::strstr(szCurPath, "\\");
    while('\0' != *ptr && NULL != ptr)
    {
        ptr2 = ptr + 1;
        ptr2 = ::strstr(ptr2, "\\");
        if(NULL == ptr2)        // 最后一个 "\\"
        {
            *ptr = '/';
            *(++ptr) = '\0';
            break;
        }

        *ptr = '/';
        ptr = ::strstr(szCurPath, "\\");
    }
    
    strcpy_s(m_stchMoudlePath, sizeof(m_stchMoudlePath), szCurPath);

    // Socket Init
    WSADATA wsaData;
    int err = WSAStartup(WINSOCK_VERSION, &wsaData);
    if(0 != err)
    {
        //"WSAStartup error!";
    }

    // SimpleCrypt
    QString str = QLatin1String("735e8f2fbc14fe96");        // 16位
    quint64 key64 = str.toULongLong(NULL, 16);
    g_SimpleCrypt.setKey(key64);
    g_SimpleCrypt.setCompressionMode(SimpleCrypt::CompressionAlways);
    g_SimpleCrypt.setIntegrityProtectionMode(SimpleCrypt::ProtectionHash);
}
void CFuncTool::UnInit()
{
    // Socket UnInit
    WSACleanup();
}

void CFuncTool::GetLocalCurTime(stTimeData& timeData)
{
    SYSTEMTIME sysTime;
    GetLocalTime(&sysTime);

    timeData.wYear         = sysTime.wYear;
    timeData.wMonth        = sysTime.wMonth;
    timeData.wDayOfWeek    = sysTime.wDayOfWeek;
    timeData.wDay          = sysTime.wDay;
    timeData.wHour         = sysTime.wHour;
    timeData.wMinute       = sysTime.wMinute;
    timeData.wSecond       = sysTime.wSecond;
    timeData.wMilliseconds = sysTime.wMilliseconds;
}

// 将一个16进制的值转成ASCII，并带一个空格
void CFuncTool::Change16ToAscii(int iData, unsigned char* pszResult, int iSize)
{
    sprintf_s((char*)pszResult, iSize, "%02X ", iData);
}

// 串口号
void CFuncTool::SetComIndex(int iIndex)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iIndex);
    ::WritePrivateProfileString("com", "index", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetComIndex()
{
    return (::GetPrivateProfileInt("com", "index", 0, SYS_INI_FILE_NAME));
}

// 波特率
void CFuncTool::SetComBaud(int iBaud)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iBaud);
    ::WritePrivateProfileString("com", "baud", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetComBaud()
{
    return (::GetPrivateProfileInt("com", "baud", 0, SYS_INI_FILE_NAME));
}

// 数据位
void CFuncTool::SetComDataBit(int iData)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iData);
    ::WritePrivateProfileString("com", "databit", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetComDataBit()
{
    return (::GetPrivateProfileInt("com", "databit", 3, SYS_INI_FILE_NAME));
}

// 停止位
void CFuncTool::SetComStopBit(int iStop)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iStop);
    ::WritePrivateProfileString("com", "stopbit", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetComStopBit()
{
    return (::GetPrivateProfileInt("com", "stopbit", 0, SYS_INI_FILE_NAME));
}

// 校验位
void CFuncTool::SetComCheckBit(int iCheck)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iCheck);
    ::WritePrivateProfileString("com", "checkbit", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetComCheckBit()
{
    return (::GetPrivateProfileInt("com", "checkbit", 0, SYS_INI_FILE_NAME));
}

// 流控制
void CFuncTool::SetComStreamCtrl(int iCtrl)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iCtrl);
    ::WritePrivateProfileString("com", "streamctrl", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetComStreamCtrl()
{
    return (::GetPrivateProfileInt("com", "streamctrl", 2, SYS_INI_FILE_NAME));
}

// 源地址
void CFuncTool::SetComSvrAddr(int iAddr)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iAddr);
    ::WritePrivateProfileString("comsource", "addr", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetComSvrAddr()
{
    return (::GetPrivateProfileInt("comsource", "addr", 0, SYS_INI_FILE_NAME));
}

// 源端口
void CFuncTool::SetComSvrPort(int iPort)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iPort);
    ::WritePrivateProfileString("comsource", "port", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetComSvrPort()
{
    return (::GetPrivateProfileInt("comsource", "port", 0, SYS_INI_FILE_NAME));
}

// 备用1
void CFuncTool::SetComBackup1(int iBackup)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iBackup);
    ::WritePrivateProfileString("comsource", "use", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetComBackup1()
{
    return (::GetPrivateProfileInt("comsource", "use", 0, SYS_INI_FILE_NAME));
}

// 目的地址
void CFuncTool::SetComDestAddr(int iAddr)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iAddr);
    ::WritePrivateProfileString("comtarget", "addr", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetComDestAddr()
{
    return (::GetPrivateProfileInt("comtarget", "addr", 0, SYS_INI_FILE_NAME));
}

// 目的端口
void CFuncTool::SetComDestPort(int iPort)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iPort);
    ::WritePrivateProfileString("comtarget", "port", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetComDestPort()
{
    return (::GetPrivateProfileInt("comtarget", "port", 0, SYS_INI_FILE_NAME));
}

// 备用2
void CFuncTool::SetComBackup2(int iBackup)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iBackup);
    ::WritePrivateProfileString("comtarget", "use", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetComBackup2()
{
    return (::GetPrivateProfileInt("comtarget", "use", 0, SYS_INI_FILE_NAME));
}

// 命令字
void CFuncTool::SetComCommand(int iCommand)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iCommand);
    ::WritePrivateProfileString("comtarget", "command", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetComCommand()
{
    return (::GetPrivateProfileInt("comtarget", "command", 0, SYS_INI_FILE_NAME));
}

// 帧号
void CFuncTool::SetComNumber(int iNum)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iNum);
    ::WritePrivateProfileString("comtarget", "num", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetComNumber()
{
    return (::GetPrivateProfileInt("comtarget", "num", 0, SYS_INI_FILE_NAME));
}

// 周期
void CFuncTool::SetComCircle(int iCircle)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iCircle);
    ::WritePrivateProfileString("option", "circle", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetComCircle()
{
    return (::GetPrivateProfileInt("option", "circle", 1000, SYS_INI_FILE_NAME));
}

// 自动成帧
void CFuncTool::SetAutoMakeFrame(int iAuto)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iAuto);
    ::WritePrivateProfileString("option", "autoframe", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetAutoMakeFrame()
{
    int iAuto = (::GetPrivateProfileInt("option", "autoframe", 1, SYS_INI_FILE_NAME));
    if(iAuto <= 0) iAuto = 0;
    else iAuto = 1;
    return iAuto;
}

// 发16进制
void CFuncTool::SetSend16Hex(int i16Hex)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", i16Hex);
    ::WritePrivateProfileString("option", "send16hex", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetSend16Hex()
{
    int i16Hex = (::GetPrivateProfileInt("option", "send16hex", 0, SYS_INI_FILE_NAME));
    if(i16Hex <= 0) i16Hex = 0;
    else i16Hex = 1;
    return i16Hex;
}

// 收16进制
void CFuncTool::SetRecv16Hex(int i16Hex)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", i16Hex);
    ::WritePrivateProfileString("option", "recv16hex", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetRecv16Hex()
{
    int i16Hex = (::GetPrivateProfileInt("option", "recv16hex", 0, SYS_INI_FILE_NAME));
    if(i16Hex <= 0) i16Hex = 0;
    else i16Hex = 1;
    return i16Hex;
}

// 发送字符串
void CFuncTool::SetComSendData(const char* pchSendData)
{
    ::WritePrivateProfileString("senddata", "text", pchSendData, SYS_INI_FILE_NAME);
}
void CFuncTool::GetComSendData(char* pchSendData, int iLength)
{
    ::GetPrivateProfileString("senddata", "text", "", pchSendData, iLength, SYS_INI_FILE_NAME);
}

// 超时ms
void CFuncTool::SetDownloadTimeoutMS(int iMS)
{
    // 屏蔽掉：重启就使用默认值
    //char chData[128] = {0};
    //sprintf_s(chData, sizeof(chData), "%d", iMS);
    //::WritePrivateProfileString("timeout", "ms", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetDownloadTimeoutMS()
{
    return (::GetPrivateProfileInt("timeout", "ms", 1000, SYS_INI_FILE_NAME));
}

// 超时次数
void CFuncTool::SetDownloadTimeoutCount(int iMS)
{
    // 屏蔽掉：重启就使用默认值
    //char chData[128] = {0};
    //sprintf_s(chData, sizeof(chData), "%d", iMS);
    //::WritePrivateProfileString("timeout", "count", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetDownloadTimeoutCount()
{
    return (::GetPrivateProfileInt("timeout", "count", 5, SYS_INI_FILE_NAME));
}

// 是否是“加密”
void CFuncTool::SetEncryptData(int iEncrypt)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iEncrypt);
    ::WritePrivateProfileString("option", "encrypt", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetEncryptData()
{
    return (::GetPrivateProfileInt("option", "encrypt", 0, SYS_INI_FILE_NAME));
}

// 发送数据时是否追加“回车”
void CFuncTool::SetEnterForSend(int iEnter)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iEnter);
    ::WritePrivateProfileString("option", "enter", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetEnterForSend()
{
    return (::GetPrivateProfileInt("option", "enter", 0, SYS_INI_FILE_NAME));
}

// UDP 地址
void CFuncTool::SetUDPAddr(const char* pchSendData)
{
    ::WritePrivateProfileString("net", "addr", pchSendData, SYS_INI_FILE_NAME);
}
void CFuncTool::GetUDPAddr(char* pchSendData, int iLength)
{
    ::GetPrivateProfileString("net", "addr", "127.0.0.1", pchSendData, iLength, SYS_INI_FILE_NAME);
}

// UDP 端口
void CFuncTool::SetUDPPort(int iPort)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iPort);
    ::WritePrivateProfileString("net", "port", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetUDPPort()
{
    return (::GetPrivateProfileInt("net", "port", 8087, SYS_INI_FILE_NAME));
}

// 网口、串口选择
void CFuncTool::SetComNetSelect(int iSelect)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iSelect);
    ::WritePrivateProfileString("net", "select", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetComNetSelect()
{
    return (::GetPrivateProfileInt("net", "select", 0, SYS_INI_FILE_NAME));
}

// 下载文件的路径
void CFuncTool::SetDownloadFilePath(const char* pchFilePath)
{
    ::WritePrivateProfileString("download", "file", pchFilePath, SYS_INI_FILE_NAME);
}
void CFuncTool::GetDownloadFilePath(char* pchFilePath, int iLength)
{
    ::GetPrivateProfileString("download", "file", "", pchFilePath, iLength, SYS_INI_FILE_NAME);
}

// 导出文件的路径
void CFuncTool::SetExportFilePath(const char* pchFilePath)
{
    ::WritePrivateProfileString("export", "file", pchFilePath, SYS_INI_FILE_NAME);
}
void CFuncTool::GetExportFilePath(char* pchFilePath, int iLength)
{
    ::GetPrivateProfileString("export", "file", "", pchFilePath, iLength, SYS_INI_FILE_NAME);
}

// 允许导出文件
void CFuncTool::SetAllowExportFile(int iAllow)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iAllow);
    ::WritePrivateProfileString("export", "allow", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetAllowExportFile()
{
    return (::GetPrivateProfileInt("export", "allow", 0, SYS_INI_FILE_NAME));
}

// 导出文件加入时间
void CFuncTool::SetExportUseTime(int iUse)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iUse);
    ::WritePrivateProfileString("export", "usetime", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetExportUseTime()
{
    return (::GetPrivateProfileInt("export", "usetime", 0, SYS_INI_FILE_NAME));
}

// 选择单板还是系统(多板)
void CFuncTool::SetUpdateSelect(int iSelect)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iSelect);
    ::WritePrivateProfileString("update", "select", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetUpdateSelect()
{
    return (::GetPrivateProfileInt("update", "select", 0, SYS_INI_FILE_NAME));
}

// CNAU
// 文件版本
void CFuncTool::SetCNAUFileVersion(float fVer)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%0.1f", fVer);
    ::WritePrivateProfileString("cnaufile", "version", chData, SYS_INI_FILE_NAME);
}
float CFuncTool::GetCNAUFileVersion()
{
    char chData[128] = {0};
    ::GetPrivateProfileString("cnaufile", "version", "1.0", chData, sizeof(chData), SYS_INI_FILE_NAME);
    return atof(chData);
}

// 文件路径
void CFuncTool::SetCNAUFilePath(char* pszFilePath)
{
    ::WritePrivateProfileString("cnaufile", "path", pszFilePath, SYS_INI_FILE_NAME);
}
void CFuncTool::GetCNAUFilePath(char* pszFilePath, int iSize)
{
    ::GetPrivateProfileString("cnaufile", "path", "", pszFilePath, iSize, SYS_INI_FILE_NAME);
}

// window
void CFuncTool::SetWindowWidth_SysINI(int iWidth)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iWidth);
    ::WritePrivateProfileString("window", "width", chData, SYS_INI_FILE_NAME);
    
}
int CFuncTool::GetWindowWidth_SysINI()
{
    return ::GetPrivateProfileInt("window", "width", 1100, SYS_INI_FILE_NAME);
}
void CFuncTool::SetWindowHeight_SysINI(int iHeight)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", iHeight);
    ::WritePrivateProfileString("window", "height", chData, SYS_INI_FILE_NAME);
}
int CFuncTool::GetWindowHeight_SysINI()
{
    return ::GetPrivateProfileInt("window", "height", 630, SYS_INI_FILE_NAME);
}
void CFuncTool::SetWindowMax_SysINI(bool bMaximized)
{
    char chData[128] = {0};
    sprintf_s(chData, sizeof(chData), "%d", bMaximized);
    ::WritePrivateProfileString("window", "maximized", chData, SYS_INI_FILE_NAME);
}
bool CFuncTool::GetWindowMax_SysINI()
{
    bool bMaximized = false;
    int iValue = ::GetPrivateProfileInt("window", "maximized", 0, SYS_INI_FILE_NAME);
    if(iValue > 0)
    {
        bMaximized = true;
    }
    return bMaximized;
}

void CFuncTool::HighTimeBegin()
{
    LARGE_INTEGER liTime;
    ::QueryPerformanceCounter(&liTime);
    m_stllCounterBegin = liTime.QuadPart;
}
void CFuncTool::HighTimeEnd()
{
    LARGE_INTEGER liTime;
    ::QueryPerformanceCounter(&liTime);
    m_stllCounterEnd = liTime.QuadPart;
}
double CFuncTool::GetHighTime()
{
    return ((double)(m_stllCounterEnd-m_stllCounterBegin)/m_stdSysFreq);
}

LONGLONG CFuncTool::GetHighTimeCounter()
{
    LARGE_INTEGER liTime;
    ::QueryPerformanceCounter(&liTime);
    return (liTime.QuadPart);
}
double CFuncTool::GetHighTime(LONGLONG llBegin, LONGLONG llEnd)
{
    return ((double)(llEnd-llBegin)/m_stdSysFreq);
}

void CFuncTool::GetMoudlePath(char* pszPath, int iSize)
{
    strcpy_s(pszPath, iSize, m_stchMoudlePath);
}

bool CFuncTool::FolderExist(const char* pszFolder)
{
    bool bRes = false;
    WIN32_FIND_DATA wfd;
    HANDLE fFind = FindFirstFile(pszFolder, &wfd);
    if((INVALID_HANDLE_VALUE != fFind) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        bRes = true;
    }
    FindClose(fFind);
    return bRes;
}
bool CFuncTool::CreateFolder(const char* pszFolder)
{
    if(FALSE == CreateDirectory(pszFolder, NULL))
    {
        return false;
    }
    return true;
}

// 获取路径下找到的第一个文件名称，短名称，指定扩展名
bool CFuncTool::GetFirstFileOnFolder(char* pszFilePath_in, char* pszFilePath_out, int iSize)
{
    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFile(pszFilePath_in, &wfd);
    if(INVALID_HANDLE_VALUE == hFind) return false;
    bool bFound = false;
    int iLength0 = strlen(pszFilePath_in);
    int iLength1 = 0;
    char* ptr0 = NULL;
    char* ptr1 = NULL;
    bool bIsSame = false;
    do
    {
        if(lstrcmp(wfd.cFileName,".")==0 || lstrcmp(wfd.cFileName,"..")==0) continue;
        if((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) continue;

        // 扩展名是否一致
        bIsSame = true;
        iLength1 = strlen(wfd.cFileName);
        ptr0 = pszFilePath_in + iLength0;
        ptr1 = wfd.cFileName + iLength1;
        do
        {
            --ptr0;
            --ptr1;
            if(*ptr0 != *ptr1) 
            {
                bIsSame = false;
                break;
            }
        }while(('.' != *ptr0) && ('.' != *ptr1));
        if(*ptr0 != *ptr1) bIsSame = false;
        if(false == bIsSame) continue;

        strcpy_s(pszFilePath_out, iSize, wfd.cFileName);
        bFound = true;
        break;
        
    }while(FindNextFile(hFind, &wfd));
    FindClose(hFind);

    return bFound;
}

unsigned short CFuncTool::GetCRC16(unsigned char* cp,unsigned int leng)
{
    unsigned short crc = 0;
    unsigned int i,j;
    if (leng <= 0 ) 
    {
        return (0);
    }
    for (j = 0; j < leng; j++) 
    {
        crc = crc ^ (int)*cp++ << 8;
        for(i = 0; i < 8; ++i) 
        {
            if(crc & 0x8000) 
            {
                crc = crc << 1 ^ 0x1021;
            }
            else 
            {
                crc = crc << 1;
            }
        }
    }
    return (unsigned short )(crc & 0xFFFF);
}

// 自动升级：获取 目录下形如 Ref_Input_Vx.y.bin 文件的版本号。pszFolder 后面带 '/'
bool CFuncTool::GetVerOnDirAndShortName(const char* pszFolder, const char* pszShortName, float& fVer)
{
    char chPath[MAX_PATH];
    strcpy_s(chPath, sizeof(chPath), pszFolder);
    strcat_s(chPath, sizeof(chPath), pszShortName);
    strcat_s(chPath, sizeof(chPath), "*.*");

    bool bFound = false;
    char chData[32] = {0};

    WIN32_FIND_DATA wfd;
    HANDLE hFind = FindFirstFile(chPath, &wfd);
    if(INVALID_HANDLE_VALUE == hFind) return bFound;
    do
    {
        if(lstrcmp(wfd.cFileName,".")==0 || lstrcmp(wfd.cFileName,"..")==0) continue;
        if((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) continue;
        
        // 找到，获取版本
        char* ptrValue = strstr(wfd.cFileName, "_V");
        if(NULL != ptrValue)
        {
            ptrValue += 2;
            char* ptr = strstr(ptrValue, ".bin");
            if(NULL != ptr)
            {
                *ptr = '\0';
                strcpy_s(chData, sizeof(chData), ptrValue);
                fVer = (float)atof(chData);
                if(fVer > 0.0f)
                {
                    bFound = true;
                    break;
                }
            }
        }

    }while(FindNextFile(hFind, &wfd));
    FindClose(hFind);

    return bFound;
}

void CFuncTool::SetCryptKey(quint64 key)
{
    g_SimpleCrypt.setKey(key);
}

QString CFuncTool::EncryptToString(const QString& plaintext)
{
    return g_SimpleCrypt.encryptToString(plaintext);
}
QString CFuncTool::EncryptToString(QByteArray plaintext)
{
    return g_SimpleCrypt.encryptToString(plaintext);
}
QByteArray CFuncTool::EncryptToByteArray(const QString& plaintext)
{
    return g_SimpleCrypt.encryptToByteArray(plaintext);
}
QByteArray CFuncTool::EncryptToByteArray(QByteArray plaintext)
{
    return g_SimpleCrypt.encryptToByteArray(plaintext);
}

QString CFuncTool::DecryptToString(const QString& cyphertext)
{
    return g_SimpleCrypt.decryptToString(cyphertext);
}
QString CFuncTool::DecryptToString(QByteArray cypher)
{
    return g_SimpleCrypt.decryptToString(cypher);
}
QByteArray CFuncTool::DecryptToByteArray(const QString& cyphertext)
{
    return g_SimpleCrypt.decryptToByteArray(cyphertext);
}
QByteArray CFuncTool::DecryptToByteArray(QByteArray cypher)
{
    return g_SimpleCrypt.decryptToByteArray(cypher);
}

int CFuncTool::GetCryptLastError()
{
    return g_SimpleCrypt.lastError();
}

void CFuncTool::ExecCommand(char* pszCmd)
{
    //控制命令行窗口信息
    STARTUPINFO si;

    //返回进程信息
    PROCESS_INFORMATION pi;

    si.cb = sizeof(STARTUPINFO);
    GetStartupInfo(&si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;           // 隐藏命令行窗口

    //创建获取命令行进程
    BOOL bret = CreateProcess (NULL, pszCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi );

    //关闭所有的句柄
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

bool CFuncTool::ParseFileToPageCheck(char* pszFilePath, st_PageCheck& oPageCheck)
{
    // 读取
    HANDLE hFile = CreateFile(pszFilePath, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL); 
    if(INVALID_HANDLE_VALUE == hFile) return false;

    DWORD dwSize = GetFileSize(hFile, NULL);
    if(dwSize < 1) return false;

    char* pBuf = new char[dwSize+1];
    if(NULL == pBuf) return false;
    memset(pBuf, 0, dwSize+1);

    bool bReadStat = ReadFile(hFile, pBuf, dwSize, &dwSize, NULL);
    CloseHandle(hFile);
    if((!bReadStat) || (dwSize < 1))
    {
        delete[] pBuf;
        pBuf = NULL;
        return false;
    }

    QString strEncryty = pBuf;
    delete[] pBuf;
    pBuf = NULL;

    // 解密
    QString strDecptData = CFuncTool::DecryptToString(strEncryty);
    if(0 != CFuncTool::GetCryptLastError())
    {
        return false;
    }

    dwSize = strDecptData.size();
    pBuf = new char[dwSize+1];
    if(NULL == pBuf) return false;
    memset(pBuf, 0, dwSize+1);
    memcpy(pBuf, strDecptData.toLatin1().data(), dwSize);

    oPageCheck.Clear();

    // 解析
    char chData[1024] = {0};
    char* ptr = NULL;

    // 1. Num=5,Admin=0
    ptr = pBuf;
    memset(chData, 0, sizeof(chData));
    ptr = ReadOneLine(ptr, chData, sizeof(chData));

    int iNum = 0;
    int iAdmin = 0;
    ReadTwoNumOnLine(chData, iNum, iAdmin);

    oPageCheck.m_bAdmin = false;
    if(iAdmin > 0) oPageCheck.m_bAdmin = true;

    for(int i = 0; i < iNum; ++i)
    {
        // 2. PIndex=0,PUse=1,PItemNum=3
        memset(chData, 0, sizeof(chData));
        ptr = ReadOneLine(ptr, chData, sizeof(chData));

        int iPageIndex = 0;
        int iPageUse = 0;
        int iPageItemNum = 0;
        ReadThreeNumOnLine(chData, iPageIndex, iPageUse, iPageItemNum);
        AddOnePage(oPageCheck, iPageIndex);

        for(int j = 0; j < iPageItemNum; ++j)
        {
            // 3. PItemIndex=1,PItemUse=1
            memset(chData, 0, sizeof(chData));
            ptr = ReadOneLine(ptr, chData, sizeof(chData));

            int iPageItemIndex = 0;
            int iPageItemUse = 0;
            ReadTwoNumOnLine(chData, iPageItemIndex, iPageItemUse);
            AddOnePageItem(oPageCheck, iPageIndex, iPageItemIndex, iPageItemUse?true:false);
        }
    }

    delete[] pBuf;
    pBuf = NULL;
    return true;
}

char* CFuncTool::ReadOneLine(char* ptrBegin, char* ptrOut, int iOutSize)
{
    char* ptrEnd = ptrBegin;
    while(('\0' != *ptrEnd) && (0x0D != *ptrEnd) && (0x0A != *ptrEnd)) ++ptrEnd;
    memcpy(ptrOut, ptrBegin, ptrEnd-ptrBegin);
    while((0x0D == *ptrEnd) || (0x0A == *ptrEnd)) ++ptrEnd;
    return ptrEnd;
}
void CFuncTool::ReadTwoNumOnLine(char* ptrData, int& iNum0, int& iNum1)
{
    char* ptr = ReadOneNum(ptrData, iNum0);
    ptr = ReadOneNum(ptr, iNum1);
}
void CFuncTool::ReadThreeNumOnLine(char* ptrData, int& iNum0, int& iNum1, int& iNum2)
{
    char* ptr = ReadOneNum(ptrData, iNum0);
    ptr = ReadOneNum(ptr, iNum1);
    ptr = ReadOneNum(ptr, iNum2);
}
char* CFuncTool::ReadOneNum(char* ptrData, int& iNum)
{
    char* ptr = ptrData;
    while((*ptr < '0') || (*ptr > '9')) ++ptr;
    char* ptrValue = ptr;
    while((',' != *ptr) && ('\0' != *ptr)) ++ptr;
    *ptr = '\0';
    iNum = atoi(ptrValue);
    ++ptr;
    return ptr;
}

// 添加一页
void CFuncTool::AddOnePage(st_PageCheck& oPageCheck, int iPageIndex)
{
    // 避免重复添加
    bool bFound = false;
    int iItemIndex = 0;
    for(int i = 0; i < oPageCheck.GetPageNums(); ++i)
    {
        st_PageData& PD = oPageCheck.m_PageData_vector[i];
        if(iPageIndex == PD.m_iPageIndex)
        {
            bFound = true;
            break;
        }
    }

    // 没找到，添加
    if(false == bFound)
    {
        st_PageData oPage;
        oPage.m_iPageIndex = iPageIndex;
        oPage.m_bPageUse = true;

        AddItemDataOfPage(oPage);
        oPageCheck.AddOne(oPage);
    }
}

// 添加一页一项
void CFuncTool::AddOnePageItem(st_PageCheck& oPageCheck, int iPageIndex, int iPageItemIndex, bool bUse)
{
    // 避免重复添加
    for(int i = 0; i < oPageCheck.GetPageNums(); ++i)
    {
        st_PageData& PD = oPageCheck.m_PageData_vector[i];
        if(iPageIndex == PD.m_iPageIndex)
        {
            for(int j = 0; j < PD.GetPageItemNums(); ++j)
            {
                if(iPageItemIndex == PD.m_PageItem_vector[j].m_iPageItemIndex)  // 找到
                {
                    PD.m_PageItem_vector[j].m_bPageItemUse = bUse;
                    break;
                }
            }
            break;
        }
    }
}

// 将某页的所有的检测子项都添加进去
void CFuncTool::AddItemDataOfPage(st_PageData& oPD)
{
    if(PAGE_INDEX_OF_UPDATE == oPD.m_iPageIndex)
    {
        st_PageItemData oPage0_Item0;
        oPage0_Item0.m_iPageItemIndex = PAGE_UPDATE_ENTER_CHECK;
        oPage0_Item0.m_bPageItemUse = false;
        oPD.AddOne(oPage0_Item0);

        //st_PageItemData oPage0_Item1;
        //oPage0_Item1.m_iPageItemIndex = PAGE_UPDATE_ALONE_OR_SYSTEM;
        //oPage0_Item1.m_bPageItemUse = false;
        //oPD.AddOne(oPage0_Item1);

        st_PageItemData oPage0_Item2;
        oPage0_Item2.m_iPageItemIndex = PAGE_UPDATE_REF_STATUS;
        oPage0_Item2.m_bPageItemUse = false;
        oPD.AddOne(oPage0_Item2);

        st_PageItemData oPage0_Item3;
        oPage0_Item3.m_iPageItemIndex = PAGE_UPDATE_EXPORT_FILE;
        oPage0_Item3.m_bPageItemUse = false;
        oPD.AddOne(oPage0_Item3);
    }
    else if(PAGE_INDEX_OF_SIGNAL == oPD.m_iPageIndex)
    {
        st_PageItemData oPage1_Item0;
        oPage1_Item0.m_iPageItemIndex = PAGE_SIGNAL_INPUT_CTRL;
        oPage1_Item0.m_bPageItemUse = false;
        oPD.AddOne(oPage1_Item0);

        st_PageItemData oPage1_Item1;
        oPage1_Item1.m_iPageItemIndex = PAGE_SIGNAL_PRIORITY;
        oPage1_Item1.m_bPageItemUse = false;
        oPD.AddOne(oPage1_Item1);
    }
    else if(PAGE_INDEX_OF_STATUS == oPD.m_iPageIndex)
    {
        ;
    }
    else if(PAGE_INDEX_OF_ADJUST == oPD.m_iPageIndex)
    {
        ;
    }
    else if(PAGE_INDEX_OF_GNSS == oPD.m_iPageIndex)
    {
        ;
    }
}
