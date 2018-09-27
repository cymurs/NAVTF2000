#ifndef _MYLABEL_H_
#define _MYLABEL_H_

#include <QLabel>

// --------------------------------------------------------------------------------------
// 信号可视图(新)
// --------------------------------------------------------------------------------------
class QPainter;

class CUpdateTab;
class CMyImageTab;

// --------------------------------------------------------------------------------------
// 信号状态
enum enSigSelectStatus
{
    en_Empty_Status,
    en_Signal_Status,
    en_Empty_Connect_Status,
    en_Signal_Connect_Status,
};

// 信号选择
struct stSigSelectData
{
    stSigSelectData()
    {
        m_oSigStatus = en_Empty_Status;
        m_iLine_X = 0;
        m_iLine_Y = 0;
        m_iLine_Width = 0;
        m_iLine_Height = 0;

        m_iWave_X = 0;
        m_iWave_Y = 0;
        m_iWave_Width = 0;
        m_iWave_Height = 0;

        m_iSelect_X = 0;
        m_iSelect_Y = 0;
        m_iSelect_Width = 0;
        m_iSelect_Height = 0;
    }

    // 线区域
    int m_iLine_X;
    int m_iLine_Y;
    int m_iLine_Width;
    int m_iLine_Height;

    // 信号区域
    int m_iWave_X;
    int m_iWave_Y;
    int m_iWave_Width;
    int m_iWave_Height;

    // 鼠标点选区域
    int m_iSelect_X;
    int m_iSelect_Y;
    int m_iSelect_Width;
    int m_iSelect_Height;

    // 信号状态
    enSigSelectStatus m_oSigStatus;
};

// 图像数据
struct stImageDrawData
{
    stImageDrawData()
    {
        m_iLeftTopIC_X = 0;
        m_iLeftTopIC_Y = 0;
        m_iLeftTopIC_Width = 0;
        m_iLeftTopIC_Height = 0;

        m_iLeftBottomIC_X = 0;
        m_iLeftBottomIC_Y = 0;
        m_iLeftBottomIC_Width = 0;
        m_iLeftBottomIC_Height = 0;

        m_iRightIC_X = 0;
        m_iRightIC_Y = 0;
        m_iRightIC_Width = 0;
        m_iRightIC_Height = 0;

        m_iInputLineWidth = 0;
        m_iOutputLineWidth = 0;
    }

    // 左上IC
    int m_iLeftTopIC_X;
    int m_iLeftTopIC_Y;
    int m_iLeftTopIC_Width;
    int m_iLeftTopIC_Height;

    // 左下IC
    int m_iLeftBottomIC_X;
    int m_iLeftBottomIC_Y;
    int m_iLeftBottomIC_Width;
    int m_iLeftBottomIC_Height;

    // 右边IC
    int m_iRightIC_X;
    int m_iRightIC_Y;
    int m_iRightIC_Width;
    int m_iRightIC_Height;

    // 上部4个连接线(以及文字区域)(由各IC尺寸算出)
    // 下部2个连接线(以及文字区域)(由各IC尺寸算出)
    // 上部输出(以及文字区域)(由各IC尺寸算出)
    // 下部输出(以及文字区域)(由各IC尺寸算出)
    
    // 信号线和输出线的宽度
    int m_iInputLineWidth;
    int m_iOutputLineWidth;

    // 信号选择
    stSigSelectData m_oSigSelect_1;
    stSigSelectData m_oSigSelect_2;
    stSigSelectData m_oSigSelect_3;
    stSigSelectData m_oSigSelect_4;
    stSigSelectData m_oSigSelect_5;
    stSigSelectData m_oSigSelect_6;
};


// --------------------------------------------------------------------------------------
// 
// --------------------------------------------------------------------------------------
class CMyLabel : public QLabel
{
    Q_OBJECT

public:
    CMyLabel(QWidget* parent = 0);
    ~CMyLabel();

public:
    void SetUsePtr(CUpdateTab* pComNetTab, CMyImageTab* pMyImageTab);

    void SetTopSelectStatus(int iIndex, enSigSelectStatus oSigSta);
    void SetBottomSelectStatus(int iIndex, enSigSelectStatus oSigSta);
    void SetTopConnectStatus(int iIndex, enSigSelectStatus oSigSta);
    void SetBottomConnectStatus(int iIndex, enSigSelectStatus oSigSta);

    enSigSelectStatus GetTopStatus(int iIndex);
    enSigSelectStatus GetBottomStatus(int iIndex);

    void mousePressEventFunc(QMouseEvent*);

public slots:
    void mousePressEventSlot(QMouseEvent*);

protected:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);
    void resizeEvent(QResizeEvent*);

private:
    void GetViewportXYWH(int& x, int& y, int& w, int& h);

private:
    void DrawICImage(int iX, int iY, int iWidth, int iHeght, QPainter& pt);
    void DrawWave(int iX, int iY, int iWidth, int iHeght, QPainter& pt);
    int DrawConnectLine(int iX, int iY, int iWidth, int iHeght, QString& strName, QPainter& pt, enSigSelectStatus oSigSta);
    void DrawInputLine(int iX, int iY, int iWidth, int iHeght, QPainter& pt, enSigSelectStatus oSigSta);
    void DrawOutputLine(int iX, int iY, int iWidth, int iHeght, QString& strName, QPainter& pt, enSigSelectStatus oSigSta);

    void CalcImageData();
    void DrawImageData(QPainter& pt);

    void ResetTopSelectConnectStatus();
    void ResetBottomSelectConnectStatus();

signals:
    void mousePressEventSignal(QMouseEvent*);

private:
    int m_iWinWidth;
    int m_iWinHeight;

    QFont* m_pOriFont;
    QFont* m_pUseFont;

    stImageDrawData m_oImageData;

private:
    CUpdateTab* m_pComNetTab;
    CMyImageTab* m_pMyImageTab;

};


#endif

