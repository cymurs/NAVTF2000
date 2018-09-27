#ifndef _LIN_PROTOCOL_H_
#define _LIN_PROTOCOL_H_

#include "Libase.h"

/****************************************************************************************/
// Com 缓冲区 大小
#define MAX_SIZE_OF_SAVE_BUF        (1024*8)
#define MAX_SIZE_OF_TEMP_BUF        (1024*8)
#define MAX_SIZE_OF_RECV_BUF        (1024*4)
#define MAX_SIZE_OF_SEND_BUF        (1024*4)
#define MAX_SIZE_OF_SHOW_BUF        (1024*5*2)

/****************************************************************************************
    协议模块
*****************************************************************************************/
class CMyProt
{
public:
    CMyProt();
    ~CMyProt();

public:
    // 处理数据(读)
    void HandleData(unsigned char* pszDataBuf, int& iDataLength);

    // 组帧: chCommand = 0，使用输入框的命令；iDataLength_in = 0，表示是字符串形的数据
    void WriteOneFrame(unsigned char chCommand, const char* pszData_in, int iDataLength_in, char* pszDataBuf, int iBufSize, int& iLength);

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
    void SetRecvShowFunc(SETRECVSHOWFUNC pSetShowFunc);

    // 设置显示发送包的函数
    void SetShowSendDataFunc(SHOWSENDDATAFUNC pShowFunc);

    // 设置帧数据处理函数
    void SetFrameHandleFunc(FRAMEHANDLEFUNC pFrameHandleFunc);
    
public:
    

private:
    // 将一个16进制的值转成ASCII，并带一个空格
    void Change16ToAscii(int iData, unsigned char* pszResult, int iSize);

    unsigned short GetCRC16(unsigned char* cp,unsigned int leng);

    // 计算字符串中以空格分割的字符的个数
    int StringNumOnSplit(const char* pszBuf);

private:
    int m_iSaveIndex_Start;
    int m_iSaveIndex_End;

    unsigned char* m_pCheckBuf;
    unsigned char* m_pDataUseBuf;
    unsigned char* m_pTempBuf;

    char* m_pShowBuf;
    char* m_pShowTempBuf;
    unsigned char m_chOne[64];

    unsigned char m_chWriteUseOne[64];

private:
    bool m_bDataHasFrame;
    bool m_bSendDataIs16Hex;
    bool m_bRecvDataIs16Hex;
    bool m_bShowDebug;
    bool m_bShowColor;
    bool m_bRecvAll;

    SHOWRECVDATAFUNC m_pShowRecvFunc;
    SETRECVSHOWFUNC m_pSetRecvShowFunc;
    SHOWSENDDATAFUNC m_pShowSendFunc;
    FRAMEHANDLEFUNC m_pFrameHandleFunc;

    // 源地址
    int m_iSourceAddr;
    int m_iSourcePort;
    int m_iSourceResv;

    // 目的地址
    int m_iTargetAddr;
    int m_iTargetPort;
    int m_iTargetResv;

    // 命令字
    int m_iFrameNo;
    int m_iCmd;

private:
    int m_iCountUse;        // 用于处理帧大小为270的时候的显示格式问题: 0, 1, 2
};


#endif
