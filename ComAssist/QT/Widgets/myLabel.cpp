#include "stdafx.h"
#include "myLabel.h"
#include <QtGui>

#include "../../../Common/Function/Function.h"
#include "../../../Common/Define/DefineFile.h"

#include "updateTab.h"
#include "imageTab.h"       // 互相包含


// --------------------------------------------------------------------------------------
extern st_PageCheck g_oPageCheck;           // 功能权限

extern char g_chTopSigName[5][64];
extern char g_chTopSigName_2[5][64];

// --------------------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------------------
CMyLabel::CMyLabel(QWidget* parent)
{
    m_iWinWidth = 0;
    m_iWinHeight = 0;

    m_pComNetTab = NULL;
    m_pMyImageTab = NULL;

    m_pOriFont = new QFont();
    *m_pOriFont = font();

    m_pUseFont = new QFont();

    connect(this, SIGNAL(mousePressEventSignal(QMouseEvent*)), this, SLOT(mousePressEventSlot(QMouseEvent*)));

    // 本地信号
    ResetTopSelectConnectStatus();
    m_oImageData.m_oSigSelect_1.m_oSigStatus = en_Signal_Connect_Status;
}
CMyLabel::~CMyLabel()
{
    delete m_pOriFont;
    delete m_pUseFont;
}

void CMyLabel::SetUsePtr(CUpdateTab* pComNetTab, CMyImageTab* pMyImageTab)
{
    m_pComNetTab = pComNetTab;
    m_pMyImageTab = pMyImageTab;
}

void CMyLabel::SetTopSelectStatus(int iIndex, enSigSelectStatus oSigSta)
{
    switch(iIndex)
    {
    case 0:
        m_oImageData.m_oSigSelect_1.m_oSigStatus = oSigSta;
        break;
    case 1:
        m_oImageData.m_oSigSelect_2.m_oSigStatus = oSigSta;
        break;
    case 2:
        m_oImageData.m_oSigSelect_3.m_oSigStatus = oSigSta;
        break;
    case 3:
        m_oImageData.m_oSigSelect_4.m_oSigStatus = oSigSta;
        break;
    default:
        break;
    }
    update();
}
void CMyLabel::SetBottomSelectStatus(int iIndex, enSigSelectStatus oSigSta)
{
    switch(iIndex)
    {
    case 0:
        m_oImageData.m_oSigSelect_5.m_oSigStatus = oSigSta;
        break;
    case 1:
        m_oImageData.m_oSigSelect_6.m_oSigStatus = oSigSta;
        break;
    default:
        break;
    }
    update();
}

void CMyLabel::SetTopConnectStatus(int iIndex, enSigSelectStatus oSigSta)
{
    if(en_Signal_Connect_Status == oSigSta)
    {
        ResetTopSelectConnectStatus();
    }

    switch(iIndex)
    {
    case 0:
        m_oImageData.m_oSigSelect_1.m_oSigStatus = oSigSta;
        break;
    case 1:
        m_oImageData.m_oSigSelect_2.m_oSigStatus = oSigSta;
        break;
    case 2:
        m_oImageData.m_oSigSelect_3.m_oSigStatus = oSigSta;
        break;
    case 3:
        m_oImageData.m_oSigSelect_4.m_oSigStatus = oSigSta;
        break;
    default:
        break;
    }
    update();
}
void CMyLabel::SetBottomConnectStatus(int iIndex, enSigSelectStatus oSigSta)
{
    if(en_Signal_Connect_Status == oSigSta)
    {
        ResetBottomSelectConnectStatus();
    }

    switch(iIndex)
    {
    case 0:
        m_oImageData.m_oSigSelect_5.m_oSigStatus = oSigSta;
        break;
    case 1:
        m_oImageData.m_oSigSelect_6.m_oSigStatus = oSigSta;
        break;
    default:
        break;
    }
    update();
}

enSigSelectStatus CMyLabel::GetTopStatus(int iIndex)
{
    switch(iIndex)
    {
    case 0:
        return m_oImageData.m_oSigSelect_1.m_oSigStatus;
        break;
    case 1:
        return m_oImageData.m_oSigSelect_2.m_oSigStatus;
        break;
    case 2:
        return m_oImageData.m_oSigSelect_3.m_oSigStatus;
        break;
    case 3:
        return m_oImageData.m_oSigSelect_4.m_oSigStatus;
        break;
    default:
        break;
    }
    return en_Empty_Status;
}
enSigSelectStatus CMyLabel::GetBottomStatus(int iIndex)
{
    switch(iIndex)
    {
    case 0:
        return m_oImageData.m_oSigSelect_5.m_oSigStatus;
        break;
    case 1:
        return m_oImageData.m_oSigSelect_6.m_oSigStatus;
        break;
    default:
        break;
    }
    return en_Empty_Status;
}

void CMyLabel::paintEvent(QPaintEvent* pe)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // window
    int w,h,x,y;
    GetViewportXYWH(x, y, w, h);
    painter.setWindow(0, 0, m_iWinWidth, m_iWinHeight);
    painter.setViewport(x, y, w, h);

    DrawImageData(painter);
}

void CMyLabel::mousePressEventFunc(QMouseEvent* me)
{
    emit mousePressEventSignal(me);
}

void CMyLabel::mousePressEventSlot(QMouseEvent* me)
{
    mousePressEvent(me);
}
void CMyLabel::mousePressEvent(QMouseEvent* me)
{
    QPoint curPos = mapFromGlobal(me->globalPos());
    int x = curPos.x();
    int y = curPos.y();

    float fWidthDiv = (float)width() / (float)m_iWinWidth;
    float fHeightDiv = (float)height() / (float)m_iWinHeight;

    bool bSelect = false;
    if(Qt::LeftButton == me->button())
    {
        for(int i = 0; i < 1; ++i)
        {
            if(false == m_pMyImageTab->ComNetCanUse()) break;

            int iX0 = m_oImageData.m_oSigSelect_1.m_iSelect_X * fWidthDiv;
            int iY0 = m_oImageData.m_oSigSelect_1.m_iSelect_Y * fHeightDiv;
            int iX1 = (m_oImageData.m_oSigSelect_1.m_iSelect_X+m_oImageData.m_oSigSelect_1.m_iSelect_Width) * fWidthDiv;
            int iY1 = (m_oImageData.m_oSigSelect_1.m_iSelect_Y+m_oImageData.m_oSigSelect_1.m_iSelect_Height) * fHeightDiv;
            if((x >= iX0) && (x <= iX1) && (y >= iY0) && (y <= iY1))    // Inside
            {
                if(en_Signal_Status == m_oImageData.m_oSigSelect_1.m_oSigStatus)
                {
                    // 只有在 “手动切换” 模式才有效
                    if(false == m_pMyImageTab->CanSwitchSignal()) break;

                    m_pComNetTab->SendFreqSelect(0);

                    bSelect = true;
                    break;
                }
            }

            iX0 = m_oImageData.m_oSigSelect_2.m_iSelect_X * fWidthDiv;
            iY0 = m_oImageData.m_oSigSelect_2.m_iSelect_Y * fHeightDiv;
            iX1 = (m_oImageData.m_oSigSelect_2.m_iSelect_X+m_oImageData.m_oSigSelect_2.m_iSelect_Width) * fWidthDiv;
            iY1 = (m_oImageData.m_oSigSelect_2.m_iSelect_Y+m_oImageData.m_oSigSelect_2.m_iSelect_Height) * fHeightDiv;
            if((x >= iX0) && (x <= iX1) && (y >= iY0) && (y <= iY1))    // Inside
            {
                if(en_Signal_Status == m_oImageData.m_oSigSelect_2.m_oSigStatus)
                {
                    // 只有在 “手动切换” 模式才有效
                    if(false == m_pMyImageTab->CanSwitchSignal()) break;

                    m_pComNetTab->SendFreqSelect(1);

                    bSelect = true;
                    break;
                }
            }

            iX0 = m_oImageData.m_oSigSelect_3.m_iSelect_X * fWidthDiv;
            iY0 = m_oImageData.m_oSigSelect_3.m_iSelect_Y * fHeightDiv;
            iX1 = (m_oImageData.m_oSigSelect_3.m_iSelect_X+m_oImageData.m_oSigSelect_3.m_iSelect_Width) * fWidthDiv;
            iY1 = (m_oImageData.m_oSigSelect_3.m_iSelect_Y+m_oImageData.m_oSigSelect_3.m_iSelect_Height) * fHeightDiv;
            if((x >= iX0) && (x <= iX1) && (y >= iY0) && (y <= iY1))    // Inside
            {
                if(en_Signal_Status == m_oImageData.m_oSigSelect_3.m_oSigStatus)
                {
                    // 只有在 “手动切换” 模式才有效
                    if(false == m_pMyImageTab->CanSwitchSignal()) break;

                    m_pComNetTab->SendFreqSelect(2);

                    bSelect = true;
                    break;
                }
            }

            iX0 = m_oImageData.m_oSigSelect_4.m_iSelect_X * fWidthDiv;
            iY0 = m_oImageData.m_oSigSelect_4.m_iSelect_Y * fHeightDiv;
            iX1 = (m_oImageData.m_oSigSelect_4.m_iSelect_X+m_oImageData.m_oSigSelect_4.m_iSelect_Width) * fWidthDiv;
            iY1 = (m_oImageData.m_oSigSelect_4.m_iSelect_Y+m_oImageData.m_oSigSelect_4.m_iSelect_Height) * fHeightDiv;
            if((x >= iX0) && (x <= iX1) && (y >= iY0) && (y <= iY1))    // Inside
            {
                if(en_Signal_Status == m_oImageData.m_oSigSelect_4.m_oSigStatus)
                {
                    // 只有在 “手动切换” 模式才有效
                    if(false == m_pMyImageTab->CanSwitchSignal()) break;

                    m_pComNetTab->SendFreqSelect(3);

                    bSelect = true;
                    break;
                }
            }

            iX0 = m_oImageData.m_oSigSelect_5.m_iSelect_X * fWidthDiv;
            iY0 = m_oImageData.m_oSigSelect_5.m_iSelect_Y * fHeightDiv;
            iX1 = (m_oImageData.m_oSigSelect_5.m_iSelect_X+m_oImageData.m_oSigSelect_5.m_iSelect_Width) * fWidthDiv;
            iY1 = (m_oImageData.m_oSigSelect_5.m_iSelect_Y+m_oImageData.m_oSigSelect_5.m_iSelect_Height) * fHeightDiv;
            if((x >= iX0) && (x <= iX1) && (y >= iY0) && (y <= iY1))    // Inside
            {
                if(en_Signal_Status == m_oImageData.m_oSigSelect_5.m_oSigStatus)
                {
                    // 只有在 “手动切换” 模式才有效
                    if(false == m_pMyImageTab->CanSwitchSignal()) break;

                    m_pComNetTab->SendPlusSelect(0);

                    bSelect = true;
                    break;
                }
            }

            iX0 = m_oImageData.m_oSigSelect_6.m_iSelect_X * fWidthDiv;
            iY0 = m_oImageData.m_oSigSelect_6.m_iSelect_Y * fHeightDiv;
            iX1 = (m_oImageData.m_oSigSelect_6.m_iSelect_X+m_oImageData.m_oSigSelect_6.m_iSelect_Width) * fWidthDiv;
            iY1 = (m_oImageData.m_oSigSelect_6.m_iSelect_Y+m_oImageData.m_oSigSelect_6.m_iSelect_Height) * fHeightDiv;
            if((x >= iX0) && (x <= iX1) && (y >= iY0) && (y <= iY1))    // Inside
            {
                if(en_Signal_Status == m_oImageData.m_oSigSelect_6.m_oSigStatus)
                {
                    // 只有在 “手动切换” 模式才有效
                    if(false == m_pMyImageTab->CanSwitchSignal()) break;

                    m_pComNetTab->SendPlusSelect(1);

                    bSelect = true;
                    break;
                }
            }
        }
    }

}
void CMyLabel::resizeEvent(QResizeEvent* re)
{
    const QSize size = re->size();

    // 视图窗口尺寸
    m_iWinWidth = 400;
    m_iWinHeight = m_iWinWidth * size.height()/size.width();

    CalcImageData();
}

void CMyLabel::ResetTopSelectConnectStatus()
{
    if(en_Signal_Connect_Status == m_oImageData.m_oSigSelect_1.m_oSigStatus)
    {
        m_oImageData.m_oSigSelect_1.m_oSigStatus = en_Signal_Status;
    }

    if(en_Signal_Connect_Status == m_oImageData.m_oSigSelect_2.m_oSigStatus)
    {
        m_oImageData.m_oSigSelect_2.m_oSigStatus = en_Signal_Status;
    }

    if(en_Signal_Connect_Status == m_oImageData.m_oSigSelect_3.m_oSigStatus)
    {
        m_oImageData.m_oSigSelect_3.m_oSigStatus = en_Signal_Status;
    }

    if(en_Signal_Connect_Status == m_oImageData.m_oSigSelect_4.m_oSigStatus)
    {
        m_oImageData.m_oSigSelect_4.m_oSigStatus = en_Signal_Status;
    }
}
void CMyLabel::ResetBottomSelectConnectStatus()
{
    if(en_Signal_Connect_Status == m_oImageData.m_oSigSelect_5.m_oSigStatus)
    {
        m_oImageData.m_oSigSelect_5.m_oSigStatus = en_Signal_Status;
    }

    if(en_Signal_Connect_Status == m_oImageData.m_oSigSelect_6.m_oSigStatus)
    {
        m_oImageData.m_oSigSelect_6.m_oSigStatus = en_Signal_Status;
    }
}

void CMyLabel::GetViewportXYWH(int& x, int& y, int& w, int& h)
{
    x = 0;
    y = 0;
    w = width();
    h = height();
}

void CMyLabel::CalcImageData()
{
    const int iMarginDelta = 11;     // 四周边缘值

    // IC和连接线的宽度
    const int iLeftICWidth = 30;
    const int iRightICWidth = 40;
    const int iConnectLineWidth = 150;
    
    const int iICOneDelta = (m_iWinHeight - 3 * iMarginDelta) / 6;

    // 信号线和输出线的宽度
    int iIOLineWidth = (m_iWinWidth - 2 * iMarginDelta - iLeftICWidth - iRightICWidth - iConnectLineWidth) / 2;
    m_oImageData.m_iInputLineWidth = iIOLineWidth - 20;
    m_oImageData.m_iOutputLineWidth = iIOLineWidth + 20;

    // 左上IC
    m_oImageData.m_iLeftTopIC_X = iMarginDelta + m_oImageData.m_iInputLineWidth;
    m_oImageData.m_iLeftTopIC_Y = iMarginDelta;
    m_oImageData.m_iLeftTopIC_Width = iLeftICWidth;
    m_oImageData.m_iLeftTopIC_Height = iICOneDelta * 4;

    // 左下IC
    m_oImageData.m_iLeftBottomIC_X = iMarginDelta + m_oImageData.m_iInputLineWidth;
    m_oImageData.m_iLeftBottomIC_Y = 2 * iMarginDelta + m_oImageData.m_iLeftTopIC_Height;
    m_oImageData.m_iLeftBottomIC_Width = iLeftICWidth;
    m_oImageData.m_iLeftBottomIC_Height = iICOneDelta * 2;

    // 右边IC
    m_oImageData.m_iRightIC_X = iMarginDelta + m_oImageData.m_iInputLineWidth + iLeftICWidth + iConnectLineWidth;
    m_oImageData.m_iRightIC_Y = iMarginDelta;
    m_oImageData.m_iRightIC_Width = iRightICWidth;
    m_oImageData.m_iRightIC_Height = m_iWinHeight - 2 * iMarginDelta;

    // 输入线
    const int iInputHeight = 10;
    int iInputOneHeight = m_oImageData.m_iLeftTopIC_Height / 4;

    m_oImageData.m_oSigSelect_1.m_iLine_X = iMarginDelta;
    m_oImageData.m_oSigSelect_1.m_iLine_Y = iMarginDelta;
    m_oImageData.m_oSigSelect_1.m_iLine_Width = m_oImageData.m_iInputLineWidth;
    m_oImageData.m_oSigSelect_1.m_iLine_Height = iInputHeight;

    // 信号区域
    const int iWaveHeight = 6;
    const int iDivV = 5;
    m_oImageData.m_oSigSelect_1.m_iWave_X = iMarginDelta + m_oImageData.m_oSigSelect_1.m_iLine_Width/iDivV;
    m_oImageData.m_oSigSelect_1.m_iWave_Y = iMarginDelta;
    m_oImageData.m_oSigSelect_1.m_iWave_Width = m_oImageData.m_oSigSelect_1.m_iLine_Width/2;
    m_oImageData.m_oSigSelect_1.m_iWave_Height = iWaveHeight;

    // 输入线
    m_oImageData.m_oSigSelect_2.m_iLine_X = iMarginDelta;
    m_oImageData.m_oSigSelect_2.m_iLine_Y = iMarginDelta;
    m_oImageData.m_oSigSelect_2.m_iLine_Width = m_oImageData.m_iInputLineWidth;
    m_oImageData.m_oSigSelect_2.m_iLine_Height = iInputHeight;

    // 信号区域
    m_oImageData.m_oSigSelect_2.m_iWave_X = iMarginDelta + m_oImageData.m_oSigSelect_2.m_iLine_Width/iDivV;
    m_oImageData.m_oSigSelect_2.m_iWave_Y = iMarginDelta;
    m_oImageData.m_oSigSelect_2.m_iWave_Width = m_oImageData.m_oSigSelect_2.m_iLine_Width/2;
    m_oImageData.m_oSigSelect_2.m_iWave_Height = iWaveHeight;

    // 输入线
    m_oImageData.m_oSigSelect_3.m_iLine_X = iMarginDelta;
    m_oImageData.m_oSigSelect_3.m_iLine_Y = iMarginDelta;
    m_oImageData.m_oSigSelect_3.m_iLine_Width = m_oImageData.m_iInputLineWidth;
    m_oImageData.m_oSigSelect_3.m_iLine_Height = iInputHeight;

    // 信号区域
    m_oImageData.m_oSigSelect_3.m_iWave_X = iMarginDelta + m_oImageData.m_oSigSelect_3.m_iLine_Width/iDivV;
    m_oImageData.m_oSigSelect_3.m_iWave_Y = iMarginDelta;
    m_oImageData.m_oSigSelect_3.m_iWave_Width = m_oImageData.m_oSigSelect_3.m_iLine_Width/2;
    m_oImageData.m_oSigSelect_3.m_iWave_Height = iWaveHeight;

    // 输入线
    m_oImageData.m_oSigSelect_4.m_iLine_X = iMarginDelta;
    m_oImageData.m_oSigSelect_4.m_iLine_Y = iMarginDelta;
    m_oImageData.m_oSigSelect_4.m_iLine_Width = m_oImageData.m_iInputLineWidth;
    m_oImageData.m_oSigSelect_4.m_iLine_Height = iInputHeight;

    // 信号区域
    m_oImageData.m_oSigSelect_4.m_iWave_X = iMarginDelta + m_oImageData.m_oSigSelect_4.m_iLine_Width/iDivV;
    m_oImageData.m_oSigSelect_4.m_iWave_Y = iMarginDelta;
    m_oImageData.m_oSigSelect_4.m_iWave_Width = m_oImageData.m_oSigSelect_4.m_iLine_Width/2;
    m_oImageData.m_oSigSelect_4.m_iWave_Height = iWaveHeight;

    // 输入线
    m_oImageData.m_oSigSelect_5.m_iLine_X = iMarginDelta;
    m_oImageData.m_oSigSelect_5.m_iLine_Y = iMarginDelta;
    m_oImageData.m_oSigSelect_5.m_iLine_Width = m_oImageData.m_iInputLineWidth;
    m_oImageData.m_oSigSelect_5.m_iLine_Height = iInputHeight;

    // 信号区域
    m_oImageData.m_oSigSelect_5.m_iWave_X = iMarginDelta + m_oImageData.m_oSigSelect_5.m_iLine_Width/iDivV;
    m_oImageData.m_oSigSelect_5.m_iWave_Y = iMarginDelta;
    m_oImageData.m_oSigSelect_5.m_iWave_Width = m_oImageData.m_oSigSelect_5.m_iLine_Width/2;
    m_oImageData.m_oSigSelect_5.m_iWave_Height = iWaveHeight;

    // 输入线
    m_oImageData.m_oSigSelect_6.m_iLine_X = iMarginDelta;
    m_oImageData.m_oSigSelect_6.m_iLine_Y = iMarginDelta;
    m_oImageData.m_oSigSelect_6.m_iLine_Width = m_oImageData.m_iInputLineWidth;
    m_oImageData.m_oSigSelect_6.m_iLine_Height = iInputHeight;

    // 信号区域
    m_oImageData.m_oSigSelect_6.m_iWave_X = iMarginDelta + m_oImageData.m_oSigSelect_6.m_iLine_Width/iDivV;
    m_oImageData.m_oSigSelect_6.m_iWave_Y = iMarginDelta;
    m_oImageData.m_oSigSelect_6.m_iWave_Width = m_oImageData.m_oSigSelect_6.m_iLine_Width/2;
    m_oImageData.m_oSigSelect_6.m_iWave_Height = iWaveHeight;
}

void CMyLabel::DrawImageData(QPainter& pt)
{
    // IC
    DrawICImage(m_oImageData.m_iLeftTopIC_X,    m_oImageData.m_iLeftTopIC_Y,    m_oImageData.m_iLeftTopIC_Width,    m_oImageData.m_iLeftTopIC_Height,    pt);
    DrawICImage(m_oImageData.m_iLeftBottomIC_X, m_oImageData.m_iLeftBottomIC_Y, m_oImageData.m_iLeftBottomIC_Width, m_oImageData.m_iLeftBottomIC_Height, pt);
    DrawICImage(m_oImageData.m_iRightIC_X,      m_oImageData.m_iRightIC_Y,      m_oImageData.m_iRightIC_Width,      m_oImageData.m_iRightIC_Height,      pt);

    // IC中间的连接线
    int iX = m_oImageData.m_iLeftTopIC_X + m_oImageData.m_iLeftTopIC_Width;
    int iY = m_oImageData.m_iLeftTopIC_Y;
    int iWidth = m_oImageData.m_iRightIC_X - m_oImageData.m_iLeftTopIC_X - m_oImageData.m_iLeftTopIC_Width;
    int iHeight = (m_oImageData.m_iLeftTopIC_Height)/4;
    int iY1 = 0;
    int iY2 = 0;
    int iY3 = 0;
    int iY4 = 0;

    if(false == g_oPageCheck.SignalTab_InputCtrl_Use())
    {
        iY -= iHeight/2;    // 上移
        iY1 = DrawConnectLine(iX, iY, iWidth, iHeight, tr(g_chTopSigName_2[0]), pt, m_oImageData.m_oSigSelect_1.m_oSigStatus);
        iY += iHeight;
        iY2 = DrawConnectLine(iX, iY, iWidth, iHeight, tr(g_chTopSigName_2[1]), pt, m_oImageData.m_oSigSelect_2.m_oSigStatus);
        iY += iHeight;
        iY3 = DrawConnectLine(iX, iY, iWidth, iHeight, tr(g_chTopSigName_2[2]), pt, m_oImageData.m_oSigSelect_3.m_oSigStatus);
        iY += iHeight;
        iY4 = DrawConnectLine(iX, iY, iWidth, iHeight, tr(g_chTopSigName_2[3]), pt, m_oImageData.m_oSigSelect_4.m_oSigStatus);
    }
    else
    {
        iY -= iHeight/2;    // 上移
        iY1 = DrawConnectLine(iX, iY, iWidth, iHeight, tr(g_chTopSigName[0]), pt, m_oImageData.m_oSigSelect_1.m_oSigStatus);
        iY += iHeight;
        iY2 = DrawConnectLine(iX, iY, iWidth, iHeight, tr(g_chTopSigName[1]), pt, m_oImageData.m_oSigSelect_2.m_oSigStatus);
        iY += iHeight;
        iY3 = DrawConnectLine(iX, iY, iWidth, iHeight, tr(g_chTopSigName[2]), pt, m_oImageData.m_oSigSelect_3.m_oSigStatus);
        iY += iHeight;
        iY4 = DrawConnectLine(iX, iY, iWidth, iHeight, tr(g_chTopSigName[3]), pt, m_oImageData.m_oSigSelect_4.m_oSigStatus);
    }

    iY = m_oImageData.m_iLeftBottomIC_Y;
    iHeight = (m_oImageData.m_iLeftBottomIC_Height)/2;
    iY -= iHeight/2;    // 上移
    int iY5 = DrawConnectLine(iX, iY, iWidth, iHeight, tr("1PPS_TTL/LVTTL"), pt, m_oImageData.m_oSigSelect_5.m_oSigStatus);
    iY += iHeight;
    int iY6 = DrawConnectLine(iX, iY, iWidth, iHeight, tr("1PPS_LVDS"), pt, m_oImageData.m_oSigSelect_6.m_oSigStatus);

    // 输出线
    iX = m_oImageData.m_iRightIC_X + m_oImageData.m_iRightIC_Width;
    iWidth = m_oImageData.m_iOutputLineWidth;
    iHeight = 20;
    int iOneHeight = (m_oImageData.m_iRightIC_Height - 2 * iHeight) / 3;
    iY = m_oImageData.m_iLeftTopIC_Y + (m_oImageData.m_iLeftTopIC_Height)/2 - iHeight;

    if((en_Signal_Connect_Status == m_oImageData.m_oSigSelect_1.m_oSigStatus) ||
       (en_Signal_Connect_Status == m_oImageData.m_oSigSelect_2.m_oSigStatus) ||
       (en_Signal_Connect_Status == m_oImageData.m_oSigSelect_3.m_oSigStatus) ||
       (en_Signal_Connect_Status == m_oImageData.m_oSigSelect_4.m_oSigStatus))
    {
        DrawOutputLine(iX, iY, iWidth, iHeight, tr("参考频率输入"), pt, en_Signal_Connect_Status);
    }
    else
    {
        DrawOutputLine(iX, iY, iWidth, iHeight, tr("参考频率输入"), pt, en_Empty_Status);
    }
    
    iY = m_oImageData.m_iLeftBottomIC_Y + (m_oImageData.m_iLeftBottomIC_Height)/2 - iHeight;
    if((en_Signal_Connect_Status == m_oImageData.m_oSigSelect_5.m_oSigStatus) ||
        (en_Signal_Connect_Status == m_oImageData.m_oSigSelect_6.m_oSigStatus))
    {
        DrawOutputLine(iX, iY, iWidth, iHeight, tr("参考脉冲输入"), pt, en_Signal_Connect_Status);
    }
    else
    {
        DrawOutputLine(iX, iY, iWidth, iHeight, tr("参考脉冲输入"), pt, en_Empty_Status);
    }

    // 注意：各线之间的平行对齐问题
    // 输入线，wave
    DrawInputLine(m_oImageData.m_oSigSelect_1.m_iLine_X, iY1, 
                  m_oImageData.m_oSigSelect_1.m_iLine_Width, m_oImageData.m_oSigSelect_1.m_iLine_Height, 
                  pt, m_oImageData.m_oSigSelect_1.m_oSigStatus);
    if((en_Signal_Status == m_oImageData.m_oSigSelect_1.m_oSigStatus) || 
       (en_Signal_Connect_Status == m_oImageData.m_oSigSelect_1.m_oSigStatus))
    {
        DrawWave(m_oImageData.m_oSigSelect_1.m_iWave_X, iY1-m_oImageData.m_oSigSelect_1.m_iWave_Height-3,
            m_oImageData.m_oSigSelect_1.m_iWave_Width,m_oImageData.m_oSigSelect_1.m_iWave_Height, pt);
    }

    // 点选区域(头部)(更新)
    const int iCLineHeight = 4;
    const int iHeadWidth = m_oImageData.m_oSigSelect_1.m_iLine_Height;
    m_oImageData.m_oSigSelect_1.m_iSelect_X = m_oImageData.m_oSigSelect_1.m_iLine_X;
    m_oImageData.m_oSigSelect_1.m_iSelect_Y = iY1-(iHeadWidth-iCLineHeight)/2;
    m_oImageData.m_oSigSelect_1.m_iSelect_Width = iHeadWidth;
    m_oImageData.m_oSigSelect_1.m_iSelect_Height = iHeadWidth;

    DrawInputLine(m_oImageData.m_oSigSelect_2.m_iLine_X, iY2, 
        m_oImageData.m_oSigSelect_2.m_iLine_Width, m_oImageData.m_oSigSelect_2.m_iLine_Height, 
        pt, m_oImageData.m_oSigSelect_2.m_oSigStatus);
    if((en_Signal_Status == m_oImageData.m_oSigSelect_2.m_oSigStatus) || 
       (en_Signal_Connect_Status == m_oImageData.m_oSigSelect_2.m_oSigStatus))
    {
        DrawWave(m_oImageData.m_oSigSelect_2.m_iWave_X, iY2-m_oImageData.m_oSigSelect_2.m_iWave_Height-3,
            m_oImageData.m_oSigSelect_2.m_iWave_Width,m_oImageData.m_oSigSelect_2.m_iWave_Height, pt);
    }

    // 点选区域(头部)(更新)
    m_oImageData.m_oSigSelect_2.m_iSelect_X = m_oImageData.m_oSigSelect_2.m_iLine_X;
    m_oImageData.m_oSigSelect_2.m_iSelect_Y = iY2-(iHeadWidth-iCLineHeight)/2;
    m_oImageData.m_oSigSelect_2.m_iSelect_Width = iHeadWidth;
    m_oImageData.m_oSigSelect_2.m_iSelect_Height = iHeadWidth;

    DrawInputLine(m_oImageData.m_oSigSelect_3.m_iLine_X, iY3, 
        m_oImageData.m_oSigSelect_3.m_iLine_Width, m_oImageData.m_oSigSelect_3.m_iLine_Height, 
        pt, m_oImageData.m_oSigSelect_3.m_oSigStatus);
    if((en_Signal_Status == m_oImageData.m_oSigSelect_3.m_oSigStatus) || 
       (en_Signal_Connect_Status == m_oImageData.m_oSigSelect_3.m_oSigStatus))
    {
        DrawWave(m_oImageData.m_oSigSelect_3.m_iWave_X, iY3-m_oImageData.m_oSigSelect_3.m_iWave_Height-3,
            m_oImageData.m_oSigSelect_3.m_iWave_Width,m_oImageData.m_oSigSelect_3.m_iWave_Height, pt);
    }

    // 点选区域(头部)(更新)
    m_oImageData.m_oSigSelect_3.m_iSelect_X = m_oImageData.m_oSigSelect_3.m_iLine_X;
    m_oImageData.m_oSigSelect_3.m_iSelect_Y = iY3-(iHeadWidth-iCLineHeight)/2;
    m_oImageData.m_oSigSelect_3.m_iSelect_Width = iHeadWidth;
    m_oImageData.m_oSigSelect_3.m_iSelect_Height = iHeadWidth;

    DrawInputLine(m_oImageData.m_oSigSelect_4.m_iLine_X, iY4, 
        m_oImageData.m_oSigSelect_4.m_iLine_Width, m_oImageData.m_oSigSelect_4.m_iLine_Height, 
        pt, m_oImageData.m_oSigSelect_4.m_oSigStatus);
    if((en_Signal_Status == m_oImageData.m_oSigSelect_4.m_oSigStatus) || 
       (en_Signal_Connect_Status == m_oImageData.m_oSigSelect_4.m_oSigStatus))
    {
        DrawWave(m_oImageData.m_oSigSelect_4.m_iWave_X, iY4-m_oImageData.m_oSigSelect_4.m_iWave_Height-3,
            m_oImageData.m_oSigSelect_4.m_iWave_Width,m_oImageData.m_oSigSelect_4.m_iWave_Height, pt);
    }

    // 点选区域(头部)(更新)
    m_oImageData.m_oSigSelect_4.m_iSelect_X = m_oImageData.m_oSigSelect_4.m_iLine_X;
    m_oImageData.m_oSigSelect_4.m_iSelect_Y = iY4-(iHeadWidth-iCLineHeight)/2;
    m_oImageData.m_oSigSelect_4.m_iSelect_Width = iHeadWidth;
    m_oImageData.m_oSigSelect_4.m_iSelect_Height = iHeadWidth;

    DrawInputLine(m_oImageData.m_oSigSelect_5.m_iLine_X, iY5, 
        m_oImageData.m_oSigSelect_5.m_iLine_Width, m_oImageData.m_oSigSelect_5.m_iLine_Height, 
        pt, m_oImageData.m_oSigSelect_5.m_oSigStatus);
    if((en_Signal_Status == m_oImageData.m_oSigSelect_5.m_oSigStatus) || 
       (en_Signal_Connect_Status == m_oImageData.m_oSigSelect_5.m_oSigStatus))
    {
        DrawWave(m_oImageData.m_oSigSelect_5.m_iWave_X, iY5-m_oImageData.m_oSigSelect_5.m_iWave_Height-3,
            m_oImageData.m_oSigSelect_5.m_iWave_Width,m_oImageData.m_oSigSelect_5.m_iWave_Height, pt);
    }

    // 点选区域(头部)(更新)
    m_oImageData.m_oSigSelect_5.m_iSelect_X = m_oImageData.m_oSigSelect_5.m_iLine_X;
    m_oImageData.m_oSigSelect_5.m_iSelect_Y = iY5-(iHeadWidth-iCLineHeight)/2;
    m_oImageData.m_oSigSelect_5.m_iSelect_Width = iHeadWidth;
    m_oImageData.m_oSigSelect_5.m_iSelect_Height = iHeadWidth;

    DrawInputLine(m_oImageData.m_oSigSelect_6.m_iLine_X, iY6, 
        m_oImageData.m_oSigSelect_6.m_iLine_Width, m_oImageData.m_oSigSelect_6.m_iLine_Height, 
        pt, m_oImageData.m_oSigSelect_6.m_oSigStatus);
    if((en_Signal_Status == m_oImageData.m_oSigSelect_6.m_oSigStatus) || 
       (en_Signal_Connect_Status == m_oImageData.m_oSigSelect_6.m_oSigStatus))
    {
        DrawWave(m_oImageData.m_oSigSelect_6.m_iWave_X, iY6-m_oImageData.m_oSigSelect_6.m_iWave_Height-3,
            m_oImageData.m_oSigSelect_6.m_iWave_Width,m_oImageData.m_oSigSelect_6.m_iWave_Height, pt);
    }

    // 点选区域(头部)(更新)
    m_oImageData.m_oSigSelect_6.m_iSelect_X = m_oImageData.m_oSigSelect_6.m_iLine_X;
    m_oImageData.m_oSigSelect_6.m_iSelect_Y = iY6-(iHeadWidth-iCLineHeight)/2;
    m_oImageData.m_oSigSelect_6.m_iSelect_Width = iHeadWidth;
    m_oImageData.m_oSigSelect_6.m_iSelect_Height = iHeadWidth;

}

void CMyLabel::DrawICImage(int iX, int iY, int iWidth, int iHeght, QPainter& pt)
{
    // 四边
    QPoint p0(iX, iY);
    QPoint p1(iX+iWidth, iY);
    QPoint p2(iX+iWidth, iY+iHeght);
    QPoint p3(iX, iY+iHeght);

    int iDelta = 5;
    QPoint p4(iX+iDelta, iY+iDelta);
    QPoint p5(iX+iWidth-iDelta, iY+iDelta);
    QPoint p6(iX+iWidth-iDelta, iY+iHeght-iDelta);
    QPoint p7(iX+iDelta, iY+iHeght-iDelta);

    // 绿色
    QColor color1(0, 130, 0);
    QColor color2(0, 150, 0);
    QColor color3(0, 200, 0);
    QColor color4(0, 150, 0);
    QColor color5(0, 130, 0);

    QColor qcBDard(0, 0, 0);        // 大暗
    QColor qcMDard(50,50,50/*3, 75, 72*/);      // 中暗
    QColor qcLDard(100,100,100/*4, 96, 92*/);      // 小暗
    QColor qcBLight(250,250,250/*7, 255, 250*/);   // 大亮
    QColor qcMLight(200,200,200/*0, 192, 191*/);   // 中亮
    QColor qcLLight(150,150,150/*0, 131, 126*/);   // 小亮

    // 左边
    QLinearGradient linearGradientLeft(p0.x(), p0.y(), p4.x(), p0.y());
    color1.setRgb(qcBDard.red(), qcBDard.green(), qcBDard.blue());
    color2.setRgb(qcBLight.red(), qcBLight.green(), qcBLight.blue());
    color3.setRgb(qcBLight.red(), qcBLight.green(), qcBLight.blue());
    color4.setRgb(qcMLight.red(), qcMLight.green(), qcMLight.blue());
    color5.setRgb(qcMLight.red(), qcMLight.green(), qcMLight.blue());

    linearGradientLeft.setColorAt(0.0, color1);
    linearGradientLeft.setColorAt(0.1, color2);
    linearGradientLeft.setColorAt(0.4, color3);
    linearGradientLeft.setColorAt(0.8, color4);
    linearGradientLeft.setColorAt(1.0, color5);
    pt.setBrush(linearGradientLeft);
    pt.setPen(Qt::NoPen);

    QPainterPath pathLeft;
    pathLeft.moveTo(p0);
    pathLeft.lineTo(p4);
    pathLeft.lineTo(p7);
    pathLeft.lineTo(p3);
    pathLeft.lineTo(p0);
    pt.drawPath(pathLeft);

    // 上边
    QLinearGradient linearGradientTop(p4.x(), p0.y(), p4.x(), p4.y());
    color1.setRgb(qcBDard.red(), qcBDard.green(), qcBDard.blue());
    color2.setRgb(qcBLight.red(), qcBLight.green(), qcBLight.blue());
    color3.setRgb(qcBLight.red(), qcBLight.green(), qcBLight.blue());
    color4.setRgb(qcMLight.red(), qcMLight.green(), qcMLight.blue());
    color5.setRgb(qcMLight.red(), qcMLight.green(), qcMLight.blue());

    linearGradientTop.setColorAt(0.0, color1);
    linearGradientTop.setColorAt(0.1, color2);
    linearGradientTop.setColorAt(0.4, color3);
    linearGradientTop.setColorAt(0.8, color4);
    linearGradientTop.setColorAt(1.0, color5);
    pt.setBrush(linearGradientTop);
    pt.setPen(Qt::NoPen);

    QPainterPath pathLeftTop;
    pathLeftTop.moveTo(p0);
    pathLeftTop.lineTo(p1);
    pathLeftTop.lineTo(p5);
    pathLeftTop.lineTo(p4);
    pathLeftTop.lineTo(p0);
    pt.drawPath(pathLeftTop);

    // 右边
    QLinearGradient linearGradientRight(p1.x(), p5.y(), p5.x(), p5.y());
    color1.setRgb(qcBDard.red(), qcBDard.green(), qcBDard.blue());
    color2.setRgb(qcMDard.red(), qcMDard.green(), qcMDard.blue());
    color3.setRgb(qcLDard.red(), qcLDard.green(), qcLDard.blue());
    color4.setRgb(qcMLight.red(), qcMLight.green(), qcMLight.blue());
    color5.setRgb(qcMLight.red(), qcMLight.green(), qcMLight.blue());

    linearGradientRight.setColorAt(0.0, color1);
    linearGradientRight.setColorAt(0.1, color2);
    linearGradientRight.setColorAt(0.4, color3);
    linearGradientRight.setColorAt(0.8, color4);
    linearGradientRight.setColorAt(1.0, color5);
    pt.setBrush(linearGradientRight);
    pt.setPen(Qt::NoPen);

    QPainterPath pathLeftRight;
    pathLeftRight.moveTo(p1);
    pathLeftRight.lineTo(p2);
    pathLeftRight.lineTo(p6);
    pathLeftRight.lineTo(p5);
    pathLeftRight.lineTo(p1);
    pt.drawPath(pathLeftRight);

    // 下边
    QLinearGradient linearGradientBottom(p6.x(), p2.y(), p6.x(), p6.y());
    color1.setRgb(qcBDard.red(), qcBDard.green(), qcBDard.blue());
    color2.setRgb(qcMDard.red(), qcMDard.green(), qcMDard.blue());
    color3.setRgb(qcLDard.red(), qcLDard.green(), qcLDard.blue());
    color4.setRgb(qcMLight.red(), qcMLight.green(), qcMLight.blue());
    color5.setRgb(qcMLight.red(), qcMLight.green(), qcMLight.blue());

    linearGradientBottom.setColorAt(0.0, color1);
    linearGradientBottom.setColorAt(0.1, color2);
    linearGradientBottom.setColorAt(0.4, color3);
    linearGradientBottom.setColorAt(0.8, color4);
    linearGradientBottom.setColorAt(1.0, color5);
    pt.setBrush(linearGradientBottom);
    pt.setPen(Qt::NoPen);

    QPainterPath pathLeftBottom;
    pathLeftBottom.moveTo(p2);
    pathLeftBottom.lineTo(p3);
    pathLeftBottom.lineTo(p7);
    pathLeftBottom.lineTo(p6);
    pathLeftBottom.lineTo(p2);
    pt.drawPath(pathLeftBottom);

    // 中央
    QBrush centerBrush(qcMLight);
    QRect rectCenter(p4.x()-1, p4.y()-1, p5.x()-p4.x()+2, p7.y()-p4.y()+2);
    pt.setBrush(centerBrush);
    pt.drawRect(rectCenter);

    // 中央使用新的方式
    {
        QColor color1(119, 130, 132);   // 暗
        QColor color2(160, 174, 177);   // 灰
        QColor color3(197, 214, 218);   // 亮

        QRect rt(iX+1, iY+1, iWidth-2, iHeght-2);
        
        QLinearGradient linearGradientBottom(rt.x(), rt.y(), rt.x(), rt.y()+rt.height());
        linearGradientBottom.setColorAt(0.0, color1);
        linearGradientBottom.setColorAt(0.1, color2);
        linearGradientBottom.setColorAt(0.4, color3);
        linearGradientBottom.setColorAt(0.8, color2);
        linearGradientBottom.setColorAt(1.0, color1);
        pt.setBrush(linearGradientBottom);

        pt.drawRect(rt);
    }
}

void CMyLabel::DrawWave(int iX, int iY, int iWidth, int iHeght, QPainter& pt)
{
    return;     // 不使用

    int iWDelta = iWidth / 6;

    // 明、暗各画一次
    QPen oPen;
    int iChange = 1;
    int x = 0;
    int yT = 0;
    int yB = 0;
    for(int i = 1; i >= 0; --i)
    {
        yT = iY + i*iChange;
        yB = iY + iHeght + i*iChange;

        if(1 == i)
        {
            oPen.setColor(QColor(128, 128, 128));
            oPen.setWidth(2);
        }
        else
        {
            oPen.setColor(QColor(0, 238, 0));
            oPen.setWidth(2);
        }
        pt.setPen(oPen);

        x = iX + i*iChange;
        QPoint p0(x, yB);

        x = x + iWDelta;
        QPoint p1(x, yB);
        QPoint p2(x, yT);

        x = x + iWDelta;
        QPoint p3(x, yT);
        QPoint p4(x, yB);

        x = x + iWDelta;
        QPoint p5(x, yB);
        QPoint p6(x, yT);

        x = x + iWDelta;
        QPoint p7(x, yT);
        QPoint p8(x, yB);

        x = x + iWDelta;
        QPoint p9(x, yB);
        QPoint p10(x, yT);

        x = x + iWDelta;
        QPoint p11(x, yT);
        QPoint p12(x, yB);

        pt.drawLine(p0, p1);
        pt.drawLine(p1, p2);
        pt.drawLine(p2, p3);
        pt.drawLine(p3, p4);

        pt.drawLine(p4, p5);
        pt.drawLine(p5, p6);
        pt.drawLine(p6, p7);
        pt.drawLine(p7, p8);

        pt.drawLine(p8, p9);
        pt.drawLine(p9, p10);
        pt.drawLine(p10, p11);
        pt.drawLine(p11, p12);
    }
}

int CMyLabel::DrawConnectLine(int iX, int iY, int iWidth, int iHeght, QString& strName, QPainter& pt, enSigSelectStatus oSigSta)
{
    int iLineHeight = 3;
    QRect rectLine(iX, iY+iHeght-iLineHeight, iWidth, iLineHeight);

    QPen oPen(QColor(0, 0, 0));
    pt.setPen(oPen);

    // text color
    if(en_Signal_Connect_Status == oSigSta)
    {
        QPen ConnectPen(QColor(0, 100, 0));
        pt.setPen(ConnectPen);
        m_pUseFont->setBold(true);
        m_pUseFont->setItalic(true);
    }

    // 文字
    QRect rectText(iX, iY, iWidth, iHeght-iLineHeight);
    m_pUseFont->setFamily("黑体");
    m_pUseFont->setPointSize(8);
    pt.setFont(*m_pUseFont);
    pt.drawText(rectText, Qt::AlignHCenter|Qt::AlignVCenter, strName);

    m_pUseFont->setBold(false);
    m_pUseFont->setItalic(false);

    // 线
    QColor color1(0, 0, 0);         // 暗
    QColor color2(128, 128, 128);   // 灰
    QColor color3(200, 200, 200);   // 亮

    // 改变颜色
    if(en_Signal_Connect_Status == oSigSta)
    {
        color1.setRgb(3, 46, 9);        // 暗
        color2.setRgb(9, 110, 23);      // 灰
        color3.setRgb(16, 227, 46);     // 亮
    }

    QLinearGradient linearGradientBottom(rectLine.x(), rectLine.y(), rectLine.x(), rectLine.y()+iLineHeight);

    linearGradientBottom.setColorAt(0.0, color1);
    linearGradientBottom.setColorAt(0.1, color2);
    linearGradientBottom.setColorAt(0.4, color3);
    linearGradientBottom.setColorAt(0.8, color2);
    linearGradientBottom.setColorAt(1.0, color1);
    pt.setBrush(linearGradientBottom);

    pt.drawRect(rectLine);
    return rectLine.y();
}
void CMyLabel::DrawInputLine(int iX, int iY, int iWidth, int iHeght, QPainter& pt, enSigSelectStatus oSigSta)
{
    QColor color1(0, 0, 0);         // 暗
    QColor color2(128, 128, 128);   // 灰
    QColor color3(200, 200, 200);   // 亮

    // 改变颜色
    if((en_Signal_Connect_Status == oSigSta) || (en_Signal_Status == oSigSta))
    {
        color1.setRgb(3, 46, 9);        // 暗
        color2.setRgb(9, 110, 23);      // 灰
        color3.setRgb(16, 227, 46);     // 亮
    }

    QPen oHPen(QColor(0, 0, 0));
    pt.setPen(oHPen);

    // 头部
    int iLineHeight = 4;
    int iHeadWidth = iHeght;
    int iHeadY = iY-(iHeght-iLineHeight)/2;
    QRect rectHead(iX, iHeadY, iHeadWidth, iHeght);

    QRadialGradient radialGradientHead(iX+iHeadWidth/2, iHeadY+iHeadWidth/2, iHeadWidth, iX+iHeadWidth/2, iHeadY+iHeadWidth/2);
    radialGradientHead.setColorAt(0.0, color3);
    radialGradientHead.setColorAt(0.1, color3);
    radialGradientHead.setColorAt(0.4, color2);
    radialGradientHead.setColorAt(0.8, color2);
    radialGradientHead.setColorAt(1.0, color1);

    pt.setBrush(radialGradientHead);
    pt.drawEllipse(rectHead);

    // 线
    QRect rectLine(iX+iHeadWidth-1, iY, iWidth-iHeadWidth+1, iLineHeight);

    
    QLinearGradient linearGradientBottom(rectLine.x(), rectLine.y(), rectLine.x(), rectLine.y()+iLineHeight);
    linearGradientBottom.setColorAt(0.0, color1);
    linearGradientBottom.setColorAt(0.1, color2);
    linearGradientBottom.setColorAt(0.4, color3);
    linearGradientBottom.setColorAt(0.8, color2);
    linearGradientBottom.setColorAt(1.0, color1);
    pt.setBrush(linearGradientBottom);
    pt.setPen(Qt::NoPen);

    pt.drawRect(rectLine);
}
void CMyLabel::DrawOutputLine(int iX, int iY, int iWidth, int iHeght, QString& strName, QPainter& pt, enSigSelectStatus oSigSta)
{
    int iLineHeight = 4;
    int iArrowWidth = 10;
    int iArrowHDela = 2;
    QRect rectLine(iX, iY+iHeght-iLineHeight, iWidth-iArrowWidth, iLineHeight);

    QPen oPen(QColor(0, 0, 0));
    pt.setPen(oPen);

    // text color
    if(en_Signal_Connect_Status == oSigSta)
    {
        QPen ConnectPen(QColor(0, 100, 0));
        pt.setPen(ConnectPen);
        m_pUseFont->setBold(true);
        m_pUseFont->setItalic(true);
    }

    // 文字
    QRect rectText(iX, iY, iWidth-iArrowWidth, iHeght-iLineHeight);
    m_pUseFont->setFamily("黑体");
    m_pUseFont->setPointSize(8);
    pt.setFont(*m_pUseFont);
    pt.drawText(rectText, Qt::AlignHCenter|Qt::AlignVCenter, strName);

    m_pUseFont->setBold(false);
    m_pUseFont->setItalic(false);

    // 线
    QColor color1(0, 0, 0);         // 暗
    QColor color2(128, 128, 128);   // 灰
    QColor color3(200, 200, 200);   // 亮

    // 改变颜色
    if(en_Signal_Connect_Status == oSigSta)
    {
        color1.setRgb(3, 46, 9);        // 暗
        color2.setRgb(9, 110, 23);      // 灰
        color3.setRgb(16, 227, 46);     // 亮
    }

    QLinearGradient linearGradientBottom(rectLine.x(), rectLine.y(), rectLine.x(), rectLine.y()+iLineHeight);

    linearGradientBottom.setColorAt(0.0, color1);
    linearGradientBottom.setColorAt(0.1, color2);
    linearGradientBottom.setColorAt(0.4, color3);
    linearGradientBottom.setColorAt(0.8, color2);
    linearGradientBottom.setColorAt(1.0, color1);
    pt.setBrush(linearGradientBottom);

    pt.drawRect(rectLine);

    // 箭头
    QPoint p0(rectLine.x()+rectLine.width(), rectLine.y()-iArrowHDela);
    QPoint p1(p0.x()+iArrowWidth, rectLine.y()+iLineHeight/2);
    QPoint p2(p0.x(), p0.y()+rectLine.height()+2*iArrowHDela);

    QLinearGradient arrowGradient(p0.x(), p1.y(), p1.x(), p1.y());
    arrowGradient.setColorAt(0.0, color2);
    arrowGradient.setColorAt(0.2, color2);
    arrowGradient.setColorAt(0.4, color2);
    arrowGradient.setColorAt(0.8, color2);
    arrowGradient.setColorAt(1.0, color3);
    pt.setBrush(arrowGradient);

    QPainterPath path;
    path.moveTo(p0);
    path.lineTo(p1);
    path.lineTo(p2);
    path.lineTo(p0);
    pt.drawPath(path);
}

