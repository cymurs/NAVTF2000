#ifndef _LIN_COMMUNICATE_H_
#define _LIN_COMMUNICATE_H_

#include <WinSock2.h>
#include <Windows.h>
#define    WIN32_LEAN_AND_MEAN
#include "Libase.h"

/***************************************************************************************/
class CMyProt;
class CMyComm;

/****************************************************************************************
    通信模块
*****************************************************************************************/
// 线程参数
namespace Lin_Com_Use{
typedef struct _ComWorker_Params
{
    _ComWorker_Params()
    {
        m_pComm = NULL;
        m_bComThreadExit = false;
        m_bThreadWorking = false;
    }

    CMyComm* m_pComm;                       // 指针

    volatile bool m_bComThreadExit;         // 退出标志
    volatile bool m_bThreadWorking;         // 避免重入

}COMWORKER_PARAMS, *PCOMWORKER_PARAMS;
};

/***************************************************************************************/
class CMyComm : public CMyComBasic
{
public:
    CMyComm();
    ~CMyComm();

public:
    // 当前使用网口还是串口
    void SetCurrentIsCom(bool bIsCom);

    // 收发数据是否是16进制
    void SetSendDataIs16Hex(bool bSendIs16Hex);
    void SetRecvDataIs16Hex(bool bRecvIs16Hex);

    // 收发数据是否有帧结构
    void SetDataHasFrame(bool bHasFrame);

    // 是否接收所有的数据包
    void SetRecvAllData(bool bRecvAll);

    // 是否显示调试信息
    void SetShowDebug(bool bDebug);

    // 设置源地址
    void SetSourceAddr(int iAddr, int iPort, int iResv);

    // 设置目的地址
    void SetTargetAddr(int iAddr, int iPort, int iResv);

    // 设置命令字
    void SetCommand(int iFrameNo, int iCmd);

    // 设置显示接收包的函数
    void SetShowRecvDataFunc(SHOWRECVDATAFUNC pShowFunc);
    virtual void SetRecvShowFunc(SETRECVSHOWFUNC pSetShowFunc);

    // 设置显示发送包的函数
    void SetShowSendDataFunc(SHOWSENDDATAFUNC pShowFunc);

    // 设置升级下载超时功能函数
    void SetDownloadTimeoutFunc(DOWNLOADTIMEOUTFUNC pDownloadTimeoutFunc);

    // 设置帧数据处理函数
    void SetFrameHandleFunc(FRAMEHANDLEFUNC pFrameHandleFunc);

public:                     // 串口
    // 关闭串口
    void CloseComPort();

    // 配置串口
    void SetComPort(char* pszComName, int iComBaud, int iComDataBits, int iComStopBits, int iComParity);

    // 打开串口
    bool OpenComPort();

    bool TaskIsRunning();

public:                     // 网口
    // 关闭网口
    void CloseNetPort();

    // 配置网口
    void SetNetPort(char* pszIPAddr, int iPort);

    // 打开网口
    bool OpenNetPort();

    // 写串口/网口。pszDataBuf仅仅是数据内容，不包括源地址等其他信息；
    //              chCommand = 0，使用输入框的命令；iDataLength_in = 0，表示是字符串形的数据
    void SendComNetData(unsigned char chCommand, const char* pszDataBuf, int iDataLength_in);

    // 重发当前帧
    void ReSendCurComNetData();

public:
    // 读串口/网口
    void ReadComNetData();

    // 升级下载功能超时函数
    void DownLoadTimeoutFunc();

private:
    HANDLE m_hCom;          // 串口
    SOCKET m_hUDP;          // 网口

    // 串口 配置
    char m_chComName[32];   // 串口号
    int m_iComBaud;         // 波特率
    int m_iComDataBits;     // 数据位
    int m_iComStopBits;     // 停止位
    int m_iComParity;       // 校验位

    // 网口配置
    char m_chNetIPAddr[32]; // IP
    int m_iNetPort;         // 端口
    SOCKADDR_IN m_ServerSockAddrIN;

    bool m_bCurrentIsCom;

private:
    DOWNLOADTIMEOUTFUNC m_pDownloadTimeoutFunc;

private:
    Lin_Com_Use::COMWORKER_PARAMS m_ComWorkerParams;

    CMyProt* m_pMyProt;

private:    // Com 缓冲
    unsigned char* m_pSaveBuf;
    unsigned char* m_pRecvBuf;
    unsigned char* m_pSendBuf;

    int m_iSaveLength;
    int m_iSendDataLength;
};


#endif
