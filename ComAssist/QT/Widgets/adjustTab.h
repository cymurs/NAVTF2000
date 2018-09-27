#ifndef _ADJUSTTAB_H_
#define _ADJUSTTAB_H_

#include <QWidget>

// --------------------------------------------------------------------------------------
// 设备频率校准 调整等 tab
// --------------------------------------------------------------------------------------
class CUpdateTab;

class QSignalMapper;
class QGroupBox;
class QPushButton;
class QLineEdit;
class QTextEdit;

// --------------------------------------------------------------------------------------
// 功能映射编号
#define ID_OF_LOCALADJ_FREQ_ADD          0
#define ID_OF_LOCALADJ_FREQ_SUB          1
#define ID_OF_LOCALADJ_FREQ_CLEAR        2
#define ID_OF_LOCALADJ_FREQ_SAVE         3

// --------------------------------------------------------------------------------------
class CMyAdjustTab : public QWidget
{
    Q_OBJECT
public:
    CMyAdjustTab(QWidget* parent = 0);
    ~CMyAdjustTab();

public:
    void SetComNetTab(CUpdateTab* pComNetTab);

    // 设置按钮状态(针对端口是否打开)
    void SetFuncButtonEnable(bool bEnable);

    // 处理数据
    void HandleFrameData(unsigned char chFrameCmd, const char* pFrameDataBuf, int iFrameDataLength,
                         unsigned char chSourceAddr, unsigned char chSourcePort);

    // 是否锁定
    void SetLockFunc(bool bEnabled);

    void SetAdjustAll(char* pszAll);

protected:
    void paintEvent(QPaintEvent*);

private:
    void ShowInfo_Func(const QString&, QColor rgb = QColor(0, 0, 0));

private slots:
    void ShowInfo_Slot(const QString&, QColor);
    void HandleFrame_Slot(const QString&, unsigned char chSourceAddr, unsigned char chSourcePort);

    // 通信映射功能
    void AdjustMapFunc(int);

    void ClearInfoFunc();

    void PswLockFunc();

signals:
    void ShowInfo_Signal(const QString&, QColor);
    void HandleFrame_Signal(const QString&, unsigned char chSourceAddr, unsigned char chSourcePort);

private:
    void CreateWidget();

    QGroupBox* CreatePswLockGroup();
    QGroupBox* CreateFreqAdjust();
    QGroupBox* CreateClearOp();

private:
    // 本地频率校准
    QLineEdit* m_pLocalFreqAdjTotal_LineEdit;
    QLineEdit* m_pLocalFreqAdjDelta_LineEdit;
    QPushButton* m_pLocalFreqAdjAdd_Button;
    QPushButton* m_pLocalFreqAdjSub_Button;
    QPushButton* m_pLocalFreqAdjClear_Button;
    QPushButton* m_pLocalFreqAdjSave_Button;

    // 密码保护
    QLineEdit* m_pPswLock_LineEdit;
    QPushButton* m_pPswLock_Button;

    // Info Show
    QTextEdit* m_pInfoShow_TextEdit;
    QPushButton* m_pClearInfo_Button;

private:
    char* m_pRefStaInput_Buf;

    QSignalMapper* m_pSignalMapper;

    bool m_bPswIsOK;                        // 密码正确才能操作
    bool m_bButtonEnabled;                  // 设置按钮状态(针对端口是否打开)

private:
    CUpdateTab* m_pComNetTab;

};

#endif
