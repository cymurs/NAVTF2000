#include "Protocol.h"

#include <Windows.h>
#include <stdio.h>



/****************************************************************************************
    协议模块
*****************************************************************************************/
CMyProt::CMyProt()
{
    // COM Buf
    m_pTempBuf = new unsigned char[MAX_SIZE_OF_TEMP_BUF];
    m_pCheckBuf = new unsigned char[MAX_SIZE_OF_RECV_BUF];
    m_pDataUseBuf = new unsigned char[MAX_SIZE_OF_RECV_BUF];
    m_pShowBuf = new char[MAX_SIZE_OF_SHOW_BUF];
    m_pShowTempBuf = new char[MAX_SIZE_OF_SHOW_BUF];
    m_iSaveIndex_Start = 0;
    m_iSaveIndex_End = 0;

    m_bDataHasFrame = false;
    m_bSendDataIs16Hex = false;
    m_bRecvDataIs16Hex = false;
    m_bShowDebug = false;
    m_bShowColor = true;
    m_bRecvAll = false;

    m_iSourceAddr = 0;
    m_iSourcePort = 0;
    m_iSourceResv = 0;

    m_iTargetAddr = 0;
    m_iTargetPort = 0;
    m_iTargetResv = 0;

    m_iFrameNo = 0;
    m_iCmd = 0;

    m_pShowRecvFunc = NULL;
    m_pSetRecvShowFunc = NULL;
    m_pShowSendFunc = NULL;
    m_pFrameHandleFunc = NULL;

    m_iCountUse = 0;
}
CMyProt::~CMyProt()
{
    delete[] m_pTempBuf;
    delete[] m_pCheckBuf;
    delete[] m_pDataUseBuf;
    delete[] m_pShowBuf;
    delete[] m_pShowTempBuf;

}

void CMyProt::SetSendDataIs16Hex(bool bSendIs16Hex)
{
    m_bSendDataIs16Hex = bSendIs16Hex;
}
void CMyProt::SetRecvDataIs16Hex(bool bRecvIs16Hex)
{
    m_bRecvDataIs16Hex = bRecvIs16Hex;
}

// 收发数据是否有帧结构
void CMyProt::SetDataHasFrame(bool bHasFrame)
{
    m_bDataHasFrame = bHasFrame;
}

void CMyProt::SetRecvAllData(bool bRecvAll)
{
    m_bRecvAll = bRecvAll;
}

void CMyProt::SetShowDebug(bool bDebug)
{
    m_bShowDebug = bDebug;
}

void CMyProt::SetSourceAddr(int iAddr, int iPort, int iResv)
{
    m_iSourceAddr = iAddr;
    m_iSourcePort = iPort;
    m_iSourceResv = iResv;
}

void CMyProt::SetTargetAddr(int iAddr, int iPort, int iResv)
{
    m_iTargetAddr = iAddr;
    m_iTargetPort = iPort;
    m_iTargetResv = iResv;
}

void CMyProt::SetCommand(int iFrameNo, int iCmd)
{
    m_iFrameNo = iFrameNo;
    m_iCmd = iCmd;
}

void CMyProt::SetShowRecvDataFunc(SHOWRECVDATAFUNC pShowFunc)
{
    m_pShowRecvFunc = pShowFunc;
}
void CMyProt::SetRecvShowFunc(SETRECVSHOWFUNC pSetShowFunc)
{
    m_pSetRecvShowFunc = pSetShowFunc;
}

void CMyProt::SetShowSendDataFunc(SHOWSENDDATAFUNC pShowFunc)
{
    m_pShowSendFunc = pShowFunc;
}

void CMyProt::SetFrameHandleFunc(FRAMEHANDLEFUNC pFrameHandleFunc)
{
    m_pFrameHandleFunc = pFrameHandleFunc;
}

// 处理数据
void CMyProt::HandleData(unsigned char* pszDataBuf, int& iDataLength)
{
    // 接收的数据是否是16进制；是的话则将接收到的16进制数据转成ASCII数据
    bool bIs16Data = m_bRecvDataIs16Hex;

    // 如果是不需要帧头帧尾，则直接显示接收数据
    if(false == m_bDataHasFrame)
    {
        if(false == bIs16Data)
        {
            memcpy(m_pShowBuf, pszDataBuf, iDataLength);
            m_pShowBuf[iDataLength] = '\0';
        }
        else
        {
            m_pShowBuf[0] = '\0';
            unsigned char chOne = 0;
            for(int i = 0; i < iDataLength; ++i)
            {
                chOne = pszDataBuf[i];
                Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));
                strcat_s(m_pShowBuf, MAX_SIZE_OF_SHOW_BUF, (const char*)m_chOne);
            }
        }

        // 显示
         m_bShowColor = !m_bShowColor;
        if(NULL != m_pShowRecvFunc) m_pShowRecvFunc(0, 0, (const char*)m_pShowBuf, m_bShowColor, 0, 0, 0);

        memset(pszDataBuf, 0, MAX_SIZE_OF_SAVE_BUF);
        iDataLength = 0;
        return;
    }

    do{
        m_iSaveIndex_Start = -1;
        m_iSaveIndex_End = -1;

        unsigned char chOne = 0;
        bool bFound10 = false;
        if(-1 == m_iSaveIndex_Start)        // 找帧头
        {
            bFound10 = false;
            for(int i = 0; i < iDataLength; ++i)
            {
                chOne = pszDataBuf[i];
                if(false == bFound10)
                {
                    if(0x10 == chOne)
                    {
                        bFound10 = true;
                    }
                }
                else
                {
                    if(0x02 == chOne)
                    {
                        m_iSaveIndex_Start = i - 1;     // -1
                        break;
                    }
                    bFound10 = false;
                }
            }
        }

        if(-1 != m_iSaveIndex_Start)        // 找帧尾
        {
            bFound10 = false;
            for(int i = m_iSaveIndex_Start+2; i < iDataLength; ++i)
            {
                chOne = pszDataBuf[i];
                if(false == bFound10)
                {
                    if(0x10 == chOne)
                    {
                        bFound10 = true;
                    }
                }
                else
                {
                    if(0x03 == chOne)
                    {
                        m_iSaveIndex_End = i;
                        break;
                    }
                    bFound10 = false;
                }
            }

            if((-1 != m_iSaveIndex_End) && (m_iSaveIndex_End > m_iSaveIndex_Start)) // 一帧
            {
                // 余下的
                int iLeaveLength = iDataLength - (m_iSaveIndex_End+1);
                if(iLeaveLength > 0)
                {
                    memcpy(m_pTempBuf, pszDataBuf+(m_iSaveIndex_End+1), iLeaveLength);
                }

                // 校验一帧的正确性
                {
                    int iFrameLength = m_iSaveIndex_End - m_iSaveIndex_Start + 1;
                    unsigned char chOne = 0;
                    unsigned char* pchData0 = pszDataBuf + m_iSaveIndex_Start;

                    int iSendLength = 0;
                    int iIndexUse = 2;
                    chOne = *(pchData0 + (iIndexUse++));
                    if(0x10 == chOne)iIndexUse++;
                    iSendLength = chOne * 256;
                    Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));
                    chOne = *(pchData0 + (iIndexUse++));
                    iSendLength += chOne;

                    if(abs(iFrameLength-iSendLength) > 12)      // 长度不对，丢弃（因为有双“10”的问题，长度不能使用等于的比较方式）
                    {
                        // 因缓存叠加, 后续内容会覆盖前面内容的尾部, 导致两个帧都不正常
						// 但叠加后, 后续内容中的帧依然可用
						memset(m_pShowBuf, '\0', MAX_SIZE_OF_SHOW_BUF);
						if (iFrameLength > 16) {							
							int iLeftLength = iFrameLength - 2;
							memcpy(m_pShowBuf, pszDataBuf + m_iSaveIndex_Start + 2, iLeftLength);
							if (iLeaveLength > 0) {
								memcpy(m_pShowBuf + iLeftLength, m_pTempBuf, iLeaveLength);
								iLeftLength += iLeaveLength;
							}
							memcpy(m_pTempBuf, m_pShowBuf, iLeftLength);
							iLeaveLength = iLeftLength;
						}
						
						// 保存余下的   // 拷贝下面的逻辑
                        memset(pszDataBuf, 0, MAX_SIZE_OF_SAVE_BUF);
                        iDataLength = 0;
                        if(iLeaveLength > 0)
                        {
                            memcpy(pszDataBuf, m_pTempBuf, iLeaveLength);
                            iDataLength = iLeaveLength;
                            continue;   // 增加的语句
                        }
                        else
                        {
                            iDataLength = 0;
                        }

                        break;
                    }
                }

                unsigned char* pchData = pszDataBuf + m_iSaveIndex_Start;
                int iUseDataLength = m_iSaveIndex_End - m_iSaveIndex_Start + 1;
                m_pShowBuf[0] = '\0';
                unsigned char chOne = 0;
                m_chOne[0] = '\0';

                // 用于做CRC校验
                int iCheckIndex = 0;
                memset(m_pCheckBuf, 0, MAX_SIZE_OF_RECV_BUF);

                // 传出去的帧数据
                st_FrameData stFrameData;

                // 注意：除开帧头，帧尾，0x10须表示为0x10 0x10两个；但多加入的字节不算长度，同时也不参与CRC校验

                // ASCII数据的长度 = 总长度 - 前ASCII的长度 - 后ASCII的长度
                int iPreASCIILength = 0;        // 前ASCII的长度
                int iAftASCIILength = 0;        // 后ASCII的长度
                int iASCIILength = 0;           // ASCII数据的长度

                // 如果CRC校验正确，则长度应该就是 N + 10
                int iRecvLength = 0;

                // 帧头(2)
                int iIndexUse = 0;
                strcpy_s(m_pShowBuf, MAX_SIZE_OF_SHOW_BUF, " ");
                chOne = *(pchData + (iIndexUse++));
                Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));
                chOne = *(pchData + (iIndexUse++));
                Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));

                // 长度(2) = N + 10
                int iSendLength = 0;
                chOne = *(pchData + (iIndexUse++));
                if(0x10 == chOne)iIndexUse++;
                iSendLength = chOne * 256;
                Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));
                chOne = *(pchData + (iIndexUse++));
                if(0x10 == chOne)iIndexUse++;
                iSendLength += chOne;
                Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));

                int iHighLength = (iSendLength)/256;
                int iLowLength = (iSendLength)%256;
                m_pCheckBuf[iCheckIndex++] = iHighLength;
                m_pCheckBuf[iCheckIndex++] = iLowLength;

                // 处理由长度引起的显示问题
                if(iSendLength == 270)
                {
                    if(0 == m_iCountUse) m_iCountUse = 1;
                    else if(1 == m_iCountUse) m_iCountUse = 2;
                    else m_iCountUse = 2;
                }
                else
                {
                    if(0 == m_iCountUse) m_iCountUse = 0;
                    else if(1 == m_iCountUse) m_iCountUse = 3;
                    else if(2 == m_iCountUse) m_iCountUse = 3;
                    else if(3 == m_iCountUse) m_iCountUse = 0;
                    else m_iCountUse = 0;
                }

                if((0 == m_iCountUse) || (1 == m_iCountUse)) m_bShowColor = !m_bShowColor;

                // 源地址(1)
                chOne = *(pchData + (iIndexUse++));
                m_pCheckBuf[iCheckIndex++] = chOne;
                if(0x10 == chOne) iIndexUse++;
                Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));

                stFrameData.m_chSourceAddr = chOne;

                int iSendBackPackCount = 0;
                if(chOne == m_iSourceAddr) ++iSendBackPackCount;

                // 源端口(1)
                chOne = *(pchData + (iIndexUse++));
                m_pCheckBuf[iCheckIndex++] = chOne;
                if(0x10 == chOne) iIndexUse++;
                Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));

                stFrameData.m_chSourcePort = chOne;

                if(chOne == m_iSourcePort) ++iSendBackPackCount;

                // 变量定义
                char* ptrData = NULL;
                int iASCCalcCount = 0;
                int iASCIIMore = 0;
                bool bPreIs0x10 = false;
                bool bHadLost0x10 = false;
                m_pShowTempBuf[0] = '\0';
                int iSendCRC = 0;
                int iAddCRCLength = 0;
                unsigned short usCRC = 0;
                static int stiRecvAllCount = 0;

                // 如果是回发包(源端一样)，则直接退出
                bool bShouldHandled = true;
                if((iSendBackPackCount >= 2))
                {
                    bShouldHandled = false;
                }

                // 备用 1(1)
                chOne = *(pchData + (iIndexUse++));
                m_pCheckBuf[iCheckIndex++] = chOne;
                if(0x10 == chOne) iIndexUse++;
                Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));

                stFrameData.m_chSourceUse = chOne;

                // 目的地址(1)
                chOne = *(pchData + (iIndexUse++));
                m_pCheckBuf[iCheckIndex++] = chOne;
                if(0x10 == chOne) iIndexUse++;
                Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));

                stFrameData.m_chTargetAddr = chOne;

                iSendBackPackCount = 0;
                if(chOne == m_iSourceAddr || 0x00 == chOne) ++iSendBackPackCount; // 增加对地址0x00的处理

                // 目的端口(1)
                chOne = *(pchData + (iIndexUse++));
                m_pCheckBuf[iCheckIndex++] = chOne;
                if(0x10 == chOne) iIndexUse++;
                Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));

                stFrameData.m_chTargetPort = chOne;

                if(chOne == m_iSourcePort) ++iSendBackPackCount;

                // 如果是目的地址不是自己，则直接退出
                if((iSendBackPackCount < 2))
                {
                    bShouldHandled = false;
                }

                // 备用 2(1)
                chOne = *(pchData + (iIndexUse++));
                m_pCheckBuf[iCheckIndex++] = chOne;
                if(0x10 == chOne) iIndexUse++;
                Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));

                stFrameData.m_chTargetUse = chOne;

                // 帧号(1)
                chOne = *(pchData + (iIndexUse++));
                m_pCheckBuf[iCheckIndex++] = chOne;
                if(0x10 == chOne) iIndexUse++;
                Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));

                stFrameData.m_chFrameNo = chOne;

                // 命令字(1)
                chOne = *(pchData + (iIndexUse++));
                m_pCheckBuf[iCheckIndex++] = chOne;
                if(0x10 == chOne) iIndexUse++;
                Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));

                stFrameData.m_chFrameCmd = chOne;

                iPreASCIILength = iIndexUse;
                iASCIILength = iSendLength - 10;        // 这是不包括0x10的

                // 将数据全部当成16进制，0x10重复则丢弃一个
                ptrData = (char*)(pchData + iPreASCIILength);
                memset(m_pDataUseBuf, 0, MAX_SIZE_OF_RECV_BUF);

                for(int i = 0; ; ++i)
                {
                    if(iASCIILength <= 0) break;

                    bHadLost0x10 = false;
                    chOne = ptrData[i];
                    if(true == bPreIs0x10)
                    {
                        if(0x10 == chOne)
                        {
                            ++iASCIIMore;       // 丢弃数量
                            bHadLost0x10 = true;
                        }
                        else
                        {
                            m_pDataUseBuf[iASCCalcCount] = chOne;
                            ++iASCCalcCount;
                            if(iASCCalcCount >= iASCIILength) 
                            {
                                // 如果最后一个数是0x10，则后面必然跟着一个0x10(预测)
                                if(0x10 == chOne) ++iASCIIMore;   // 丢弃数量
                                break;
                            }
                        }
                    }
                    else
                    {
                        m_pDataUseBuf[iASCCalcCount] = chOne;
                        ++iASCCalcCount;
                        if(iASCCalcCount >= iASCIILength) 
                        {
                            // 如果最后一个数是0x10，则后面必然跟着一个0x10(预测)
                            if(0x10 == chOne) ++iASCIIMore;   // 丢弃数量
                            break;
                        }
                    }
                    bPreIs0x10 = false;
                    if(false == bHadLost0x10)
                    {
                        if(0x10 == chOne) bPreIs0x10 = true;
                    }
                    bHadLost0x10 = false;
                }

                if(false == bIs16Data)
                {
                    strcpy_s(m_pShowTempBuf, MAX_SIZE_OF_SHOW_BUF, (const char*)m_pDataUseBuf);
                }
                else
                {
                    for(int i = 0; i < iASCCalcCount; ++i)
                    {
                        chOne = m_pDataUseBuf[i];
                        Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));
                        strcat_s(m_pShowTempBuf, MAX_SIZE_OF_SHOW_BUF, (const char*)m_chOne);
                    }
                }

                memcpy(m_pCheckBuf+iCheckIndex, m_pDataUseBuf, iASCCalcCount);
                iCheckIndex += iASCCalcCount;

                stFrameData.m_iFrameDataLength = iASCCalcCount;
                stFrameData.m_pFrameDataBuf = (char*)m_pDataUseBuf;
                stFrameData.m_bDataIsOK = true;
                stFrameData.m_bDataIs16Hex = bIs16Data;

                // CRC(2)
                iIndexUse += iASCIILength + iASCIIMore;
                chOne = *(pchData + (iIndexUse++));
                if(0x10 == chOne) 
                {
                    iIndexUse++;
                    ++iAddCRCLength;
                }
                iSendCRC = chOne * 256;
                Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));
                chOne = *(pchData + (iIndexUse++));
                if(0x10 == chOne) 
                {
                    iIndexUse++;
                    ++iAddCRCLength;
                }
                iSendCRC += chOne;
                Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));

                // 帧尾(2)
                chOne = *(pchData + (iIndexUse++));
                Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));
                chOne = *(pchData + (iIndexUse++));
                Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));

                iAftASCIILength = 2 + 2 + iAddCRCLength;
                iRecvLength = iASCIILength + 10;

                // 数据(N)

                if(false == m_bShowDebug)                           // 是否显示调试内容
                {
                    strcpy_s(m_pShowBuf, MAX_SIZE_OF_SHOW_BUF, m_pShowTempBuf);
                    if(0 == m_iCountUse) strcat_s(m_pShowBuf, MAX_SIZE_OF_SHOW_BUF, "\n");
                }
                else
                {
                    strcat_s(m_pShowBuf, MAX_SIZE_OF_SHOW_BUF, m_pShowTempBuf);
                    if(0 == m_iCountUse) strcat_s(m_pShowBuf, MAX_SIZE_OF_SHOW_BUF, "\n");
                }

                // 计算CRC(2)(从长度开始，止于CRC)
                usCRC = GetCRC16(m_pCheckBuf, iCheckIndex);

                ++stiRecvAllCount;

                if((NULL != m_pSetRecvShowFunc) && (true == bShouldHandled)) m_pSetRecvShowFunc(m_iCountUse);
                if((NULL != m_pShowRecvFunc) && (true == bShouldHandled)) m_pShowRecvFunc(stFrameData.m_chSourceAddr, stFrameData.m_chSourcePort, (const char*)m_pShowBuf, m_bShowColor, 0, 0, 0);

                // 将接收的数据按16进制显示出来
                if(true == m_bShowDebug)
                {
                    strcpy_s(m_pShowBuf, MAX_SIZE_OF_SHOW_BUF, "10 02 ");

                    for(int i = 0; i < iCheckIndex; ++i)
                    {
                        chOne = m_pCheckBuf[i];
                        Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));
                        strcat_s(m_pShowBuf, MAX_SIZE_OF_SHOW_BUF, (const char*)m_chOne);
                    }
                    chOne = (usCRC)/256;
                    Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));
                    strcat_s(m_pShowBuf, MAX_SIZE_OF_SHOW_BUF, (const char*)m_chOne);
                    chOne = (usCRC)%256;
                    Change16ToAscii(chOne, m_chOne, sizeof(m_chOne));
                    strcat_s(m_pShowBuf, MAX_SIZE_OF_SHOW_BUF, (const char*)m_chOne);
                    strcat_s(m_pShowBuf, MAX_SIZE_OF_SHOW_BUF, "10 03\n");
                    if((NULL != m_pShowRecvFunc) && (true == bShouldHandled)) m_pShowRecvFunc(stFrameData.m_chSourceAddr, stFrameData.m_chSourcePort, (const char*)m_pShowBuf, m_bShowColor, 0, 0, 0);
                }

                // 校验
                if(usCRC != iSendCRC)
                {
                    strcpy_s(m_pShowBuf, MAX_SIZE_OF_SHOW_BUF, "\n错误：CRC 不一致。\n");
                    if((NULL != m_pShowRecvFunc) && (true == bShouldHandled)) m_pShowRecvFunc(stFrameData.m_chSourceAddr, stFrameData.m_chSourcePort, (const char*)m_pShowBuf, false, 250, 0, 0);

                    stFrameData.m_bDataIsOK = false;
                }
                if(iSendLength != iRecvLength)
                {
                    strcpy_s(m_pShowBuf, MAX_SIZE_OF_SHOW_BUF, "\n错误：长度 不一致。\n");
                    if((NULL != m_pShowRecvFunc) && (true == bShouldHandled)) m_pShowRecvFunc(stFrameData.m_chSourceAddr, stFrameData.m_chSourcePort, (const char*)m_pShowBuf, false, 250, 0, 0);

                    stFrameData.m_bDataIsOK = false;
                }

                if((NULL != m_pFrameHandleFunc) && (true == bShouldHandled)) m_pFrameHandleFunc(&stFrameData);
				// 增加了自定义析构, 置为0, 避免释放全局内存
				stFrameData.m_pFrameDataBuf = 0;

                // 保存余下的
                memset(pszDataBuf, 0, MAX_SIZE_OF_SAVE_BUF);
                iDataLength = 0;
                if(iLeaveLength > 0)
                {
                    memcpy(pszDataBuf, m_pTempBuf, iLeaveLength);
                    iDataLength = iLeaveLength;
                }
                else
                {
                    iDataLength = 0;
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }while(iDataLength > 0);
}

// 组帧: chCommand = 0，使用输入框的命令；iDataLength_in = 0，表示是字符串形的数据
void CMyProt::WriteOneFrame(unsigned char chCommand, const char* pszData_in, int iDataLength_in, char* pszDataBuf, int iBufSize, int& iLength)
{
    // 显示发送的内容
    if(NULL != m_pShowSendFunc) 
    {
        int iLength = iDataLength_in;
        if(0 == iDataLength_in) 
        {
            iLength = strlen(pszData_in);
        }

        memset(pszDataBuf, 0, iBufSize);
        memcpy(pszDataBuf, pszData_in, iLength);
        pszDataBuf[iLength] = '\0';
        m_pShowSendFunc(m_iTargetAddr, m_iTargetPort, pszDataBuf, iLength);
    }

    // 发送的数据是否是16进制；是的话则将输入ASCII数据转成16进制数据
    bool bIs16Data = m_bSendDataIs16Hex;

    // 发送内容(由调用端 清两端空格)
    memset(pszDataBuf, 0, iBufSize);
    unsigned char* chSendData = (unsigned char*)pszDataBuf;

    // 如果是不需帧头帧尾，则只发送数据内容
    if(false == m_bDataHasFrame)
    {
        // 长度
        int iStrLength = 0;
        if(0 == iDataLength_in)
        {
            iStrLength = strlen(pszData_in);
            if(true == bIs16Data)
            {
                iStrLength = StringNumOnSplit(pszData_in);
            }
        }
        else
        {
            iStrLength = iDataLength_in;
        }

        // 数据
        int iDataUseLength = 0;
        if(0 == iDataLength_in)
        {
            if(false == bIs16Data)
            {
                for(int i = 0; i < iStrLength; ++i)
                {
                    chSendData[iDataUseLength + i] = pszData_in[i];
                }
                iDataUseLength += iStrLength;
            }
            else
            {
                const char* ptr = pszData_in;
                const char* ptrValue = ptr;
                int iSize = 0;
                int iValue = 0;
                for(int i = 0; i < iStrLength; ++i)
                {
                    while(' ' == *ptr) ++ptr;
                    ptrValue = ptr;
                    if('\0' == *ptr) break;
                    while((' ' != *ptr) && ('\0' != *ptr)) ++ptr;
                    iSize = 2;  //ptr-ptrValue; // 固定为2
                    memcpy(m_chWriteUseOne, ptrValue, iSize);
                    m_chWriteUseOne[iSize] = '\0';
                    sscanf_s((const char*)m_chWriteUseOne, "%x", &iValue);
                    chSendData[iDataUseLength++] = iValue;
                    ++ptr;
                }
            }
        }
        else
        {
            memcpy(chSendData+iDataUseLength, pszData_in, iDataLength_in);
            iDataUseLength += iDataLength_in;
        }

        chSendData[iDataUseLength] = '\0';

        iLength = iDataUseLength;

        return;
    }

    // 注意：除开帧头，帧尾，0x10须表示为0x10 0x10两个；但多加入的字节不算长度，同时也不参与CRC校验

    // 先取一遍不包括重复0x10的数据，用于计算CRC
    unsigned short usCRC = 0;
    {
        // 帧头(2)
        int iDataUseLength = 0;
        chSendData[iDataUseLength++] = 0x10;
        chSendData[iDataUseLength++] = 0x02;

        // 长度(2) = N + 10
        int iStrLength = 0;
        if(0 == iDataLength_in)
        {
            iStrLength = strlen(pszData_in);
            if(true == bIs16Data)
            {
                iStrLength = StringNumOnSplit(pszData_in);
            }
        }
        else
        {
            iStrLength = iDataLength_in;
        }
        
        int iHighLength = (iStrLength+10)/256;
        int iLowLength = (iStrLength+10)%256;
        chSendData[iDataUseLength++] = iHighLength;
        chSendData[iDataUseLength++] = iLowLength;

        // 源地址(1)
        int iSvrAddr = m_iSourceAddr;// 源地址
        chSendData[iDataUseLength++] = iSvrAddr;

        // 源端口(1)
        int iSvrPort = m_iSourcePort;// 源端口(1)
        chSendData[iDataUseLength++] = iSvrPort;

        // 备用1(1)
        int iBackup1 = m_iSourceResv;// 备用1(1)
        chSendData[iDataUseLength++] = iBackup1;

        // 目的地址(1)
        int iDestAddr = m_iTargetAddr; // 目的地址(1)
        chSendData[iDataUseLength++] = iDestAddr;

        // 目的端口(1)
        int iDestPort = m_iTargetPort; // 目的端口
        chSendData[iDataUseLength++] = iDestPort;

        // 备用2(1)
        int iBackup2 = m_iTargetResv;// 备用2
        chSendData[iDataUseLength++] = iBackup2;

        // 帧号(1)
        int iNums = m_iFrameNo;// 帧号
        chSendData[iDataUseLength++] = iNums;

        // 命令字(1)
        int iCommand = m_iCmd;// 命令字
        if(0 != chCommand) iCommand = chCommand;
        chSendData[iDataUseLength++] = iCommand;

        // 数据(N)
        if(0 == iDataLength_in)
        {
            if(false == bIs16Data)
            {
                for(int i = 0; i < iStrLength; ++i)
                {
                    chSendData[iDataUseLength + i] = pszData_in[i];
                }
                iDataUseLength += iStrLength;
            }
            else
            {
                const char* ptr = pszData_in;
                const char* ptrValue = ptr;
                int iSize = 0;
                int iValue = 0;
                for(int i = 0; i < iStrLength; ++i)
                {
                    while(' ' == *ptr) ++ptr;
                    ptrValue = ptr;
                    if('\0' == *ptr) break;
                    while((' ' != *ptr) && ('\0' != *ptr)) ++ptr;
                    iSize = 2;  //ptr-ptrValue; // 固定为2
                    memcpy(m_chWriteUseOne, ptrValue, iSize);
                    m_chWriteUseOne[iSize] = '\0';
                    sscanf_s((const char*)m_chWriteUseOne, "%x", &iValue);
                    chSendData[iDataUseLength++] = iValue;
                    ++ptr;
                }
            }
        }
        else
        {
            memcpy(chSendData+iDataUseLength, pszData_in, iDataLength_in);
            iDataUseLength += iDataLength_in;
        }
        
        // CRC(2)(从长度开始，止于CRC)
        usCRC = GetCRC16((chSendData+2), iDataUseLength-2);

        memset(pszDataBuf, 0, iBufSize);
    }

    // 帧头(2)
    int iDataUseLength = 0;
    chSendData[iDataUseLength++] = 0x10;
    chSendData[iDataUseLength++] = 0x02;

    // 长度(2) = N + 10
    int iStrLength = 0;
    if(0 == iDataLength_in)
    {
        iStrLength = strlen(pszData_in);
        if(true == bIs16Data)
        {
            iStrLength = StringNumOnSplit(pszData_in);
        }
    }
    else
    {
        iStrLength = iDataLength_in;
    }
    
    int iHighLength = (iStrLength+10)/256;
    int iLowLength = (iStrLength+10)%256;
    chSendData[iDataUseLength++] = iHighLength;
    if(0x10 == iHighLength) chSendData[iDataUseLength++] = iHighLength;
    chSendData[iDataUseLength++] = iLowLength;
    if(0x10 == iLowLength) chSendData[iDataUseLength++] = iLowLength;

    // 源地址(1)
    int iSvrAddr = m_iSourceAddr;// 源地址
    chSendData[iDataUseLength++] = iSvrAddr;
    if(0x10 == iSvrAddr) chSendData[iDataUseLength++] = iSvrAddr;

    // 源端口(1)
    int iSvrPort = m_iSourcePort;// 源端口(1)
    chSendData[iDataUseLength++] = iSvrPort;
    if(0x10 == iSvrPort) chSendData[iDataUseLength++] = iSvrPort;

    // 备用1(1)
    int iBackup1 = m_iSourceResv;// 备用1(1)
    chSendData[iDataUseLength++] = iBackup1;
    if(0x10 == iBackup1) chSendData[iDataUseLength++] = iBackup1;

    // 目的地址(1)
    int iDestAddr = m_iTargetAddr; // 目的地址(1)
    chSendData[iDataUseLength++] = iDestAddr;
    if(0x10 == iDestAddr) chSendData[iDataUseLength++] = iDestAddr;

    // 目的端口(1)
    int iDestPort = m_iTargetPort; // 目的端口
    chSendData[iDataUseLength++] = iDestPort;
    if(0x10 == iDestPort) chSendData[iDataUseLength++] = iDestPort;

    // 备用2(1)
    int iBackup2 = m_iTargetResv;// 备用2
    chSendData[iDataUseLength++] = iBackup2;
    if(0x10 == iBackup2) chSendData[iDataUseLength++] = iBackup2;

    // 帧号(1)
    int iNums = m_iFrameNo;// 帧号
    chSendData[iDataUseLength++] = iNums;
    if(0x10 == iNums) chSendData[iDataUseLength++] = iNums;

    // 命令字(1)
    int iCommand = m_iCmd;// 命令字
    if(0 != chCommand) iCommand = chCommand;
    chSendData[iDataUseLength++] = iCommand;
    if(0x10 == iCommand) chSendData[iDataUseLength++] = iCommand;

    // 数据(N)
    if(0 == iDataLength_in)
    {
        if(false == bIs16Data)
        {
            for(int i = 0; i < iStrLength; ++i)
            {
                unsigned char chDataH = pszData_in[i];
				chSendData[iDataUseLength++] = chDataH;
				if (0x10 == chDataH) chSendData[iDataUseLength++] = chDataH;
            }            
        }
        else
        {
            const char* ptr = pszData_in;
            const char* ptrValue = ptr;
            int iSize = 0;
            int iValue = 0;
            for(int i = 0; i < iStrLength; ++i)
            {
                while(' ' == *ptr) ++ptr;
                ptrValue = ptr;
                if('\0' == *ptr) break;
                while((' ' != *ptr) && ('\0' != *ptr)) ++ptr;
                iSize = 2;  //ptr-ptrValue; // 固定为2
                memcpy(m_chWriteUseOne, ptrValue, iSize);
                m_chWriteUseOne[iSize] = '\0';
                sscanf_s((const char*)m_chWriteUseOne, "%x", &iValue);
                chSendData[iDataUseLength++] = iValue;
                if(0x10 == iValue) chSendData[iDataUseLength++] = iValue;
                ++ptr;
            }
        }
    }
    else
    {
        for(int i = 0; i < iDataLength_in; ++i)
        {
            unsigned char chDataH = pszData_in[i];
            chSendData[iDataUseLength++] = chDataH;
            if(0x10 == chDataH) chSendData[iDataUseLength++] = chDataH;
        }
    }

    // CRC(2)(从长度开始，止于CRC)
    iHighLength = (usCRC)/256;
    iLowLength = (usCRC)%256;
    chSendData[iDataUseLength++] = iHighLength;
    if(0x10 == iHighLength) chSendData[iDataUseLength++] = iHighLength;
    chSendData[iDataUseLength++] = iLowLength;
    if(0x10 == iLowLength) chSendData[iDataUseLength++] = iLowLength;

    // 帧尾(2)
    chSendData[iDataUseLength++] = 0x10;
    chSendData[iDataUseLength++] = 0x03;

    chSendData[iDataUseLength] = '\0';

    iLength = iDataUseLength;

}

// 将一个16进制的值转成ASCII，并带一个空格
void CMyProt::Change16ToAscii(int iData, unsigned char* pszResult, int iSize)
{
    sprintf_s((char*)pszResult, iSize, "%02X ", iData);
}

unsigned short CMyProt::GetCRC16(unsigned char* cp,unsigned int leng)
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

// 计算字符串中以空格分割的字符的个数
int CMyProt::StringNumOnSplit(const char* pszBuf)
{
    int iCount = 0;
    const char* ptr = pszBuf;
    const char* ptrValue = ptr;
    while('\0' != *ptr)
    {
        while((' ' == *ptr) && ('\0' != *ptr)) ++ptr;
        if('\0' == *ptr)break;
        while((' ' != *ptr) && ('\0' != *ptr)) ++ptr;
        
        ++iCount;
    }
    return iCount;
}
