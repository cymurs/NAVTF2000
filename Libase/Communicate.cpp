#include "Communicate.h"

#include "Protocol.h"

#include <stdio.h>      // for sprintf_s()

#pragma comment(lib, "ws2_32.lib")

/****************************************************************************************/
DWORD WINAPI ComWorkerThread(LPVOID lpParam);

/****************************************************************************************/
static CRITICAL_SECTION g_WriteLock_CS;

/****************************************************************************************/
class CMyLock
{
public:
    CMyLock()
    {
        ::EnterCriticalSection(&g_WriteLock_CS);
    }
    ~CMyLock()
    {
        ::LeaveCriticalSection(&g_WriteLock_CS);
    }
private:
};

/****************************************************************************************
    ͨ��ģ��
*****************************************************************************************/
CMyComm::CMyComm()
{
    m_hCom = INVALID_HANDLE_VALUE;
    m_hUDP = INVALID_SOCKET;

    strcpy_s(m_chComName, sizeof(m_chComName), "COM1");
    m_iComBaud = CBR_9600;
    m_iComDataBits = 8;
    m_iComStopBits = ONESTOPBIT;
    m_iComParity = NOPARITY;

    strcpy_s(m_chNetIPAddr, sizeof(m_chNetIPAddr), "127.0.0.1");
    m_iNetPort = 0;

    m_ComWorkerParams.m_pComm = this;

    m_pDownloadTimeoutFunc = NULL;

    // COM Buf
    m_pSaveBuf = new unsigned char[MAX_SIZE_OF_SAVE_BUF];
    m_pRecvBuf = new unsigned char[MAX_SIZE_OF_RECV_BUF];
    m_pSendBuf = new unsigned char[MAX_SIZE_OF_SEND_BUF];
    m_iSaveLength = 0;
    m_iSendDataLength = 0;

    m_bCurrentIsCom = true;

    m_pMyProt = new CMyProt();

    ::InitializeCriticalSection(&g_WriteLock_CS);
}
CMyComm::~CMyComm()
{
    delete[] m_pSaveBuf;
    delete[] m_pRecvBuf;
    delete[] m_pSendBuf;

    ::DeleteCriticalSection(&g_WriteLock_CS);
}

// ��ǰʹ�����ڻ��Ǵ���
void CMyComm::SetCurrentIsCom(bool bIsCom)
{
    m_bCurrentIsCom = bIsCom;
}

// �շ������Ƿ���16����
void CMyComm::SetSendDataIs16Hex(bool bSendIs16Hex)
{
    m_pMyProt->SetSendDataIs16Hex(bSendIs16Hex);
}
void CMyComm::SetRecvDataIs16Hex(bool bRecvIs16Hex)
{
    m_pMyProt->SetRecvDataIs16Hex(bRecvIs16Hex);
}

// �շ������Ƿ���֡�ṹ
void CMyComm::SetDataHasFrame(bool bHasFrame)
{
    m_pMyProt->SetDataHasFrame(bHasFrame);
}

// �Ƿ�������е����ݰ�
void CMyComm::SetRecvAllData(bool bRecvAll)
{
    m_pMyProt->SetRecvAllData(bRecvAll);
}

void CMyComm::SetShowDebug(bool bDebug)
{
    m_pMyProt->SetShowDebug(bDebug);
}

void CMyComm::SetSourceAddr(int iAddr, int iPort, int iResv)
{
    m_pMyProt->SetSourceAddr(iAddr, iPort, iResv);
}

void CMyComm::SetTargetAddr(int iAddr, int iPort, int iResv)
{
    m_pMyProt->SetTargetAddr(iAddr, iPort, iResv);
}

void CMyComm::SetCommand(int iFrameNo, int iCmd)
{
    m_pMyProt->SetCommand(iFrameNo, iCmd);
}

void CMyComm::SetShowRecvDataFunc(SHOWRECVDATAFUNC pShowFunc)
{
    m_pMyProt->SetShowRecvDataFunc(pShowFunc);
}
void CMyComm::SetRecvShowFunc(SETRECVSHOWFUNC pSetShowFunc)
{
    m_pMyProt->SetRecvShowFunc(pSetShowFunc);
}

void CMyComm::SetShowSendDataFunc(SHOWSENDDATAFUNC pShowFunc)
{
    m_pMyProt->SetShowSendDataFunc(pShowFunc);
}

// �����������س�ʱ���ܺ���
void CMyComm::SetDownloadTimeoutFunc(DOWNLOADTIMEOUTFUNC pDownloadTimeoutFunc)
{
    m_pDownloadTimeoutFunc = pDownloadTimeoutFunc;
}

// ����֡���ݴ�����
void CMyComm::SetFrameHandleFunc(FRAMEHANDLEFUNC pFrameHandleFunc)
{
    m_pMyProt->SetFrameHandleFunc(pFrameHandleFunc);
}

// �رմ���
void CMyComm::CloseComPort()
{
    m_ComWorkerParams.m_bComThreadExit = true;      // ���߳��˳�
    if(INVALID_HANDLE_VALUE != m_hCom) CloseHandle(m_hCom);
    m_hCom = INVALID_HANDLE_VALUE;
}

bool CMyComm::TaskIsRunning()
{
    return (m_ComWorkerParams.m_bThreadWorking);
}

// �������س���
enum enum_FlowControl
{
    en_NoFlowControl = 0,
    en_CtsRtsFlowControl,
    en_CtsDtrFlowControl,
    en_DsrRtsFlowControl,
    en_DsrDtrFlowControl,
    en_XonXoffFlowControl
};

// ���ô���
void CMyComm::SetComPort(char* pszComName, int iComBaud, int iComDataBits, int iComStopBits, int iComParity)
{
    strcpy_s(m_chComName, sizeof(m_chComName), pszComName); // ���ں�
    m_iComBaud = iComBaud;                                  // ������
    m_iComDataBits = iComDataBits;                          // ����λ
    m_iComStopBits = iComStopBits;                          // ֹͣλ
    m_iComParity = iComParity;                              // У��λ
}

// �򿪴���
bool CMyComm::OpenComPort()
{
    if(INVALID_HANDLE_VALUE != m_hCom)
    {
        CloseComPort();
    }

    // ��������
    if(true == m_ComWorkerParams.m_bThreadWorking)
    {
        //("������æ�����Ժ�����");
        return false;
    }

    // �򿪴���
    m_hCom = CreateFile(m_chComName,                // COM��
                        GENERIC_READ|GENERIC_WRITE, // �������д
                        0,                          // ��ռ��ʽ
                        NULL,
                        OPEN_EXISTING,              // �򿪶����Ǵ���
                        0,                          // ͬ����ʽ
                        NULL);
    if(INVALID_HANDLE_VALUE == m_hCom)
    {
        //("�򿪴���ʧ��");
        return false;
    }

    // ���ô���

    SetupComm(m_hCom, MAX_SIZE_OF_RECV_BUF, MAX_SIZE_OF_SEND_BUF);  // ���뻺����������������Ĵ�С

    COMMTIMEOUTS TimeOuts;
    TimeOuts.ReadIntervalTimeout=MAXDWORD;      // �趨����ʱ
    TimeOuts.ReadTotalTimeoutMultiplier=0;
    TimeOuts.ReadTotalTimeoutConstant=0;        // �ڶ�һ�����뻺���������ݺ���������������أ�// �������Ƿ������Ҫ����ַ���

    TimeOuts.WriteTotalTimeoutMultiplier=500;   // �趨д��ʱ
    TimeOuts.WriteTotalTimeoutConstant=2000;
    SetCommTimeouts(m_hCom,&TimeOuts);          // ���ó�ʱ

    DCB dcb;
    GetCommState(m_hCom,&dcb);
    dcb.DCBlength = sizeof(dcb);
    dcb.BaudRate = m_iComBaud;          // ������ 
    dcb.ByteSize = m_iComDataBits;      // ����λ
    dcb.Parity = m_iComParity;          // У��λ 
    dcb.StopBits = m_iComStopBits;      // ֹͣλ
    dcb.fBinary = TRUE;
    dcb.fParity = FALSE;

    // ��������
    dcb.fDsrSensitivity = FALSE;          

    dcb.fTXContinueOnXoff = FALSE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;

    enum_FlowControl oFlowCtl = en_NoFlowControl;
    switch (oFlowCtl)
    {
    case en_NoFlowControl:
        {
            dcb.fOutxCtsFlow = FALSE;
            dcb.fOutxDsrFlow = FALSE;
            dcb.fOutX = FALSE;
            dcb.fInX = FALSE;
            break;
        }
    case en_CtsRtsFlowControl:
        {
            dcb.fOutxCtsFlow = TRUE;
            dcb.fOutxDsrFlow = FALSE;
            dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
            dcb.fOutX = FALSE;
            dcb.fInX = FALSE;
            break;
        }
    case en_CtsDtrFlowControl:
        {
            dcb.fOutxCtsFlow = TRUE;
            dcb.fOutxDsrFlow = FALSE;
            dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
            dcb.fOutX = FALSE;
            dcb.fInX = FALSE;
            break;
        }
    case en_DsrRtsFlowControl:
        {
            dcb.fOutxCtsFlow = FALSE;
            dcb.fOutxDsrFlow = TRUE;
            dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
            dcb.fOutX = FALSE;
            dcb.fInX = FALSE;
            break;
        }
    case en_DsrDtrFlowControl:
        {
            dcb.fOutxCtsFlow = FALSE;
            dcb.fOutxDsrFlow = TRUE;
            dcb.fDtrControl = DTR_CONTROL_HANDSHAKE;
            dcb.fOutX = FALSE;
            dcb.fInX = FALSE;
            break;
        }
    case en_XonXoffFlowControl:
        {
            dcb.fOutxCtsFlow = FALSE;
            dcb.fOutxDsrFlow = FALSE;
            dcb.fOutX = TRUE;
            dcb.fInX = TRUE;
            dcb.XonChar = 0x11;
            dcb.XoffChar = 0x13;
            dcb.XoffLim = 100;
            dcb.XonLim = 100;
            break;
        }
    }

    SetCommState(m_hCom,&dcb);

    PurgeComm(m_hCom,PURGE_TXCLEAR|PURGE_RXCLEAR);


    // �����߳�
    DWORD dwThreadID;
    m_ComWorkerParams.m_bComThreadExit = false;
    m_ComWorkerParams.m_bThreadWorking = true;
    CreateThread(NULL, 0, ComWorkerThread, &m_ComWorkerParams, 0, &dwThreadID);

    return true;
}


// �ر�����
void CMyComm::CloseNetPort()
{
    m_ComWorkerParams.m_bComThreadExit = true;      // ���߳��˳�
    if(INVALID_SOCKET != m_hUDP) closesocket(m_hUDP);
    m_hUDP = INVALID_SOCKET;
}

// ��������
void CMyComm::SetNetPort(char* pszIPAddr, int iPort)
{
    strcpy_s(m_chNetIPAddr, sizeof(m_chNetIPAddr), pszIPAddr);
    m_iNetPort = iPort;
}

// ������
bool CMyComm::OpenNetPort()
{
    if(INVALID_SOCKET != m_hUDP)
    {
        CloseNetPort();
    }

    // ��������
    if(true == m_ComWorkerParams.m_bThreadWorking)
    {
        //("������æ�����Ժ�����");
        return false;
    }

    // ������
    m_ServerSockAddrIN.sin_addr.S_un.S_addr = inet_addr(m_chNetIPAddr);
    m_ServerSockAddrIN.sin_family = AF_INET;
    m_ServerSockAddrIN.sin_port = htons(m_iNetPort);

    m_hUDP = socket(AF_INET, SOCK_DGRAM, 0);

    int iMode = 1;//0 == block�� 1 == non-block
    ioctlsocket(m_hUDP, FIONBIO, (u_long FAR*) &iMode);

    // �����߳�
    DWORD dwThreadID;
    m_ComWorkerParams.m_bComThreadExit = false;
    m_ComWorkerParams.m_bThreadWorking = true;
    CreateThread(NULL, 0, ComWorkerThread, &m_ComWorkerParams, 0, &dwThreadID);

    return true;
}

// ������/����
void CMyComm::ReadComNetData()
{
    // Read
    int iReaded = 0;
    if(true == m_bCurrentIsCom)                 // ����
    {
        if(INVALID_HANDLE_VALUE == m_hCom) return;

        DWORD wCount = MAX_SIZE_OF_RECV_BUF/2;    // ��ȡ���ֽ���
        BOOL bReadStat;
        bReadStat = ReadFile(m_hCom, m_pRecvBuf, wCount, &wCount, NULL);
        if(!bReadStat)
        {
            //("������ʧ��!");
            return;
        }
        iReaded = wCount;
        if(iReaded <= 0) return;
    }
    else                                        // ����
    {
        if(INVALID_SOCKET == m_hUDP) return;

        SOCKADDR_IN addr_udp;
        int addrlen = sizeof(SOCKADDR); 
        iReaded = recvfrom(m_hUDP, (char*)m_pRecvBuf, MAX_SIZE_OF_RECV_BUF/2, 0,  (SOCKADDR*) &addr_udp, &addrlen);

        if(iReaded <= 0) return;
    }

    // Copy �������������
    if((m_iSaveLength+iReaded) >= MAX_SIZE_OF_SAVE_BUF)
    {
        m_iSaveLength -= iReaded+1;
        if(m_iSaveLength < 0) m_iSaveLength = 0;
    }
    memcpy(m_pSaveBuf+m_iSaveLength, m_pRecvBuf, iReaded);
    m_iSaveLength += iReaded;

    // ��������
    m_pMyProt->HandleData(m_pSaveBuf, m_iSaveLength);
}

// д����/����
void CMyComm::SendComNetData(unsigned char chCommand, const char* pszDataBuf, int iDataLength_in)
{
    CMyLock myLock;             // Lock

    // ��֡
    m_iSendDataLength = 0;
    m_pMyProt->WriteOneFrame(chCommand, pszDataBuf, iDataLength_in, (char*)m_pSendBuf, MAX_SIZE_OF_SEND_BUF, m_iSendDataLength);

    // ����
    DWORD dwBytesWrite = m_iSendDataLength;
    if(true == m_bCurrentIsCom)
    {
        COMSTAT ComStat;
        DWORD dwErrorFlags;
        BOOL bWriteStat;
        ClearCommError(m_hCom, &dwErrorFlags, &ComStat);
        bWriteStat=WriteFile(m_hCom, m_pSendBuf, dwBytesWrite, &dwBytesWrite, NULL);
        if(!bWriteStat)
        {
            //("д����ʧ��!");
        }
    }
    else
    {
        sendto(m_hUDP, (const char*)m_pSendBuf, dwBytesWrite, 0, (SOCKADDR*) &m_ServerSockAddrIN, sizeof(SOCKADDR));
    }
}

// �ط���ǰ֡
void CMyComm::ReSendCurComNetData()
{
    CMyLock myLock;             // Lock

    // ����
    DWORD dwBytesWrite = m_iSendDataLength;
    if(true == m_bCurrentIsCom)
    {
        COMSTAT ComStat;
        DWORD dwErrorFlags;
        BOOL bWriteStat;
        PurgeComm(m_hCom,PURGE_TXCLEAR|PURGE_RXCLEAR);      // ��
        ClearCommError(m_hCom, &dwErrorFlags, &ComStat);
        bWriteStat=WriteFile(m_hCom, m_pSendBuf, dwBytesWrite, &dwBytesWrite, NULL);
        if(!bWriteStat)
        {
            //("д����ʧ��!");
        }
    }
    else
    {
        sendto(m_hUDP, (const char*)m_pSendBuf, dwBytesWrite, 0, (SOCKADDR*) &m_ServerSockAddrIN, sizeof(SOCKADDR));
    }
}

// �������ع��ܳ�ʱ����
void CMyComm::DownLoadTimeoutFunc()
{
    if(NULL != m_pDownloadTimeoutFunc) m_pDownloadTimeoutFunc();
}


/****************************************************************************************
    ȫ�ֺ���
*****************************************************************************************/
// ʹ�����������������ָ���ɵ������ͷ�
CMyComBasic* CreateComBasic()
{
    return new CMyComm();
}

// �̺߳���
DWORD WINAPI ComWorkerThread(LPVOID lpParam)
{
    Lin_Com_Use::COMWORKER_PARAMS* pComWorkerParams = (Lin_Com_Use::COMWORKER_PARAMS*)lpParam;
    CMyComm* pCommunicate =pComWorkerParams->m_pComm;

    // Loop
    while(true)
    {
        if(true == pComWorkerParams->m_bComThreadExit)          // Exit while
        {
            break;
        }

        pCommunicate->ReadComNetData();
        pCommunicate->DownLoadTimeoutFunc();

        Sleep(100);
    }

    pComWorkerParams->m_bComThreadExit = false;
    pComWorkerParams->m_bThreadWorking = false;
    return 0;
}

