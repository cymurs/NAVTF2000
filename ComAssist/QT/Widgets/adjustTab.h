#ifndef _ADJUSTTAB_H_
#define _ADJUSTTAB_H_

#include <QWidget>

// --------------------------------------------------------------------------------------
// �豸Ƶ��У׼ ������ tab
// --------------------------------------------------------------------------------------
class CUpdateTab;

class QSignalMapper;
class QGroupBox;
class QPushButton;
class QLineEdit;
class QTextEdit;

// --------------------------------------------------------------------------------------
// ����ӳ����
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

    // ���ð�ť״̬(��Զ˿��Ƿ��)
    void SetFuncButtonEnable(bool bEnable);

    // ��������
    void HandleFrameData(unsigned char chFrameCmd, const char* pFrameDataBuf, int iFrameDataLength,
                         unsigned char chSourceAddr, unsigned char chSourcePort);

    // �Ƿ�����
    void SetLockFunc(bool bEnabled);

    void SetAdjustAll(char* pszAll);

protected:
    void paintEvent(QPaintEvent*);

private:
    void ShowInfo_Func(const QString&, QColor rgb = QColor(0, 0, 0));

private slots:
    void ShowInfo_Slot(const QString&, QColor);
    void HandleFrame_Slot(const QString&, unsigned char chSourceAddr, unsigned char chSourcePort);

    // ͨ��ӳ�书��
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
    // ����Ƶ��У׼
    QLineEdit* m_pLocalFreqAdjTotal_LineEdit;
    QLineEdit* m_pLocalFreqAdjDelta_LineEdit;
    QPushButton* m_pLocalFreqAdjAdd_Button;
    QPushButton* m_pLocalFreqAdjSub_Button;
    QPushButton* m_pLocalFreqAdjClear_Button;
    QPushButton* m_pLocalFreqAdjSave_Button;

    // ���뱣��
    QLineEdit* m_pPswLock_LineEdit;
    QPushButton* m_pPswLock_Button;

    // Info Show
    QTextEdit* m_pInfoShow_TextEdit;
    QPushButton* m_pClearInfo_Button;

private:
    char* m_pRefStaInput_Buf;

    QSignalMapper* m_pSignalMapper;

    bool m_bPswIsOK;                        // ������ȷ���ܲ���
    bool m_bButtonEnabled;                  // ���ð�ť״̬(��Զ˿��Ƿ��)

private:
    CUpdateTab* m_pComNetTab;

};

#endif
