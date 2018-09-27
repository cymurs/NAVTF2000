#ifndef _LIN_LIBASE_H_
#define _LIN_LIBASE_H_

#include <memory.h>

/****************************************************************************************/
// 帧数据
struct st_FrameData
{
    st_FrameData()
    {
        m_chSourceAddr = 0;
        m_chSourcePort = 0;
        m_chSourceUse = 0;
        m_chTargetAddr = 0;
        m_chTargetPort = 0;
        m_chTargetUse = 0;
        m_chFrameNo = 0;
        m_chFrameCmd = 0;
        m_pFrameDataBuf = 0;
        m_iFrameDataLength = 0;
        m_bDataIsOK = false;
        m_bDataIs16Hex = false;
    }

    void Clone(st_FrameData* pOther)
    {
        pOther->m_chSourceAddr = m_chSourceAddr;
        pOther->m_chSourcePort = m_chSourcePort;
        pOther->m_chSourceUse = m_chSourceUse;
        pOther->m_chTargetAddr = m_chTargetAddr;
        pOther->m_chTargetPort = m_chTargetPort;
        pOther->m_chTargetUse = m_chTargetUse;
        pOther->m_chFrameNo = m_chFrameNo;
        pOther->m_chFrameCmd = m_chFrameCmd;
        //m_pFrameDataBuf = 0;
        pOther->m_iFrameDataLength = m_iFrameDataLength;
        pOther->m_bDataIsOK = m_bDataIsOK;
        pOther->m_bDataIs16Hex = m_bDataIs16Hex;
        memcpy((void*)(pOther->m_pFrameDataBuf), m_pFrameDataBuf, m_iFrameDataLength);
        (pOther->m_pFrameDataBuf)[m_iFrameDataLength] = '\0';
    }

    unsigned char m_chSourceAddr;       // 源 地址、端口和备用
    unsigned char m_chSourcePort;
    unsigned char m_chSourceUse;

    unsigned char m_chTargetAddr;       // 目的 地址、端口和备用
    unsigned char m_chTargetPort;
    unsigned char m_chTargetUse;

    unsigned char m_chFrameNo;          // 帧号和命令
    unsigned char m_chFrameCmd;

    char* m_pFrameDataBuf;              // 帧数据和长度
    int m_iFrameDataLength;

    bool m_bDataIsOK;                   // 数据是否接收正确
    bool m_bDataIs16Hex;                // 数据是否是16进制
};


/****************************************************************************************/
typedef void (*SHOWRECVDATAFUNC)(const char* pszShowData);                  // 显示接收数据
typedef void (*SETRECVSHOWFUNC)(int iShowType);                             // 显示接收的格式设置
typedef void (*SHOWSENDDATAFUNC)(const char* pszShowData, int iLength);     // 显示发送数据
typedef void (*SHOWTWOCOLORFUNC)(bool bShowColor);                          // 交替显示两种颜色(设置颜色)
typedef void (*SHOWERRORCOLORFUNC)();                                       // 发生错误显示的颜色(设置颜色)

typedef void (*DOWNLOADTIMEOUTFUNC)();                                      // 升级下载超时功能函数
typedef void (*FRAMEHANDLEFUNC)(const st_FrameData* pFrameData);            // 帧数据处理函数

/****************************************************************************************
    动态库头文件
*****************************************************************************************/
class _declspec(dllexport) CMyComBasic
{
public:
    CMyComBasic();
    virtual ~CMyComBasic();

public:
    // 当前使用网口还是串口
    virtual void SetCurrentIsCom(bool bIsCom) = 0;

    // 收发数据是否是16进制
    virtual void SetSendDataIs16Hex(bool bSendIs16Hex) = 0;
    virtual void SetRecvDataIs16Hex(bool bRecvIs16Hex) = 0;

    // 收发数据是否有帧结构
    virtual void SetDataHasFrame(bool bHasFrame) = 0;

    // 是否显示调试信息
    virtual void SetShowDebug(bool bDebug) = 0;

    // 是否接收所有的数据包
    virtual void SetRecvAllData(bool bRecvAll) = 0;

    // 设置源地址
    virtual void SetSourceAddr(int iAddr, int iPort, int iResv) = 0;

    // 设置目的地址
    virtual void SetTargetAddr(int iAddr, int iPort, int iResv) = 0;

    // 设置命令字
    virtual void SetCommand(int iFrameNo, int iCmd) = 0;

    // 设置显示接收包的函数
    virtual void SetShowRecvDataFunc(SHOWRECVDATAFUNC pShowFunc) = 0;
    virtual void SetRecvShowFunc(SETRECVSHOWFUNC pSetShowFunc) = 0;

    // 设置显示发送包的函数
    virtual void SetShowSendDataFunc(SHOWSENDDATAFUNC pShowFunc) = 0;

    // 设置修改显示颜色的函数
    virtual void SetShowTwoColorFunc(SHOWTWOCOLORFUNC pShowTwoColorFunc) = 0;

    // 设置错误显示的颜色
    virtual void SetShowErrorColorFunc(SHOWERRORCOLORFUNC pShowErrorColorFunc) = 0;

    // 设置升级下载超时功能函数
    virtual void SetDownloadTimeoutFunc(DOWNLOADTIMEOUTFUNC pDownloadTimeoutFunc) = 0;

    // 设置帧数据处理函数
    virtual void SetFrameHandleFunc(FRAMEHANDLEFUNC pFrameHandleFunc) = 0;

public:                     // 串口
    // 关闭串口
    virtual void CloseComPort() = 0;

    // 配置串口
    // pszComName = "COM1", "COM2", ...
    // iComBaud = CBR_9600, CBR_14400, ...
    // iComDataBits = 5, 6, 7, 8
    // iComStopBits = ONESTOPBIT, ONE5STOPBITS, TWOSTOPBITS
    // iComParity = NOPARITY, ODDPARITY, EVENPARITY, MARKPARITY, SPACEPARITY
    virtual void SetComPort(char* pszComName, int iComBaud, int iComDataBits, int iComStopBits, int iComParity) = 0;

    // 打开串口
    virtual bool OpenComPort() = 0;

    virtual bool TaskIsRunning() = 0;

public:                     // 网口
    // 关闭网口
    virtual void CloseNetPort() = 0;

    // 配置网口
    virtual void SetNetPort(char* pszIPAddr, int iPort) = 0;

    // 打开网口
    virtual bool OpenNetPort() = 0;

    // 写串口/网口。pszDataBuf仅仅是数据内容，不包括源地址等其他信息；
    //              chCommand = 0，使用输入框的命令；iDataLength_in = 0，表示是字符串形的数据
    virtual void SendComNetData(unsigned char chCommand, const char* pszDataBuf, int iDataLength_in) = 0;

    // 重发当前帧
    virtual void ReSendCurComNetData() = 0;

};

/****************************************************************************************
    全局函数
*****************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

    // 使用这个函数创建对象；指针由调用者释放
    _declspec(dllexport) CMyComBasic* CreateComBasic();

#ifdef __cplusplus
}
#endif


#endif
