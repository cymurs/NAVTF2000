#ifndef _STATUSTAB_H_
#define _STATUSTAB_H_

#include <QWidget>

// --------------------------------------------------------------------------------------
// 状态/监控 tab
// --------------------------------------------------------------------------------------
class CMyComBasic;

class QGroupBox;
class QLineEdit;

// --------------------------------------------------------------------------------------
class CStatusTab : public QWidget
{
    Q_OBJECT

public:
    CStatusTab(QWidget* parent = 0);
    ~CStatusTab();

public:
    void SetComBasic(CMyComBasic* pComBasic);

public:
    // 处理数据
    void HandleFrameData(unsigned char chFrameCmd, const char* pFrameDataBuf, int iFrameDataLength,
                         unsigned char chSourceAddr, unsigned char chSourcePort);

protected:
    void paintEvent(QPaintEvent*);

private slots:
    void HandleFrame_Slot(const QString&, unsigned char chSourceAddr, unsigned char chSourcePort);

signals:
    void HandleFrame_Signal(const QString&, unsigned char chSourceAddr, unsigned char chSourcePort);

private:
    bool ParseInputString(const char* ptrInput, const char* ptrKey, char* ptrOutput);

    void ParseInputForBoard(const char* ptrInput);
    void BoardShowOnInput(int iAddr, float fUV, float fUI, float fLT);

private:
    void CreateWidgets();

    QGroupBox* CreateMainCtrlGroup();
    QGroupBox* CreatePLL10MGroup();
    QGroupBox* CreatePLL1023Group();

    QGroupBox* CreateAddr06Group();
    QGroupBox* CreateAddr05Group();
    QGroupBox* CreateAddr04Group();
    QGroupBox* CreateAddr03Group();
    QGroupBox* CreateAddr02Group();
    QGroupBox* CreateAddr01Group();

    QGroupBox* CreateAddr26Group();
    QGroupBox* CreateAddr25Group();
    QGroupBox* CreateAddr24Group();
    QGroupBox* CreateAddr23Group();
    QGroupBox* CreateAddr22Group();
    QGroupBox* CreateAddr21Group();

private:
    // Main Ctrl
    QLineEdit* m_pMainCtrl_MainPowerVol;
    QLineEdit* m_pMainCtrl_BackupPowerVol;
    
    QLineEdit* m_pMainCtrl_MainBoardVol;
    QLineEdit* m_pMainCtrl_MainBoardAn;

    QLineEdit* m_pMainCtrl_PLL1An;
    QLineEdit* m_pMainCtrl_PLL2An;

    QLineEdit* m_pMainCtrl_RefClockVol;
    QLineEdit* m_pMainCtrl_RefClockAn;
    

    // PLL 10MHz
    QLineEdit* m_pPLL10M_BoardTemp;
    QLineEdit* m_pPLL10M_PowerTemp;
    QLineEdit* m_pPLL10M_ConstCTemp;
    QLineEdit* m_pPLL10M_WorkVol;
    QLineEdit* m_pPLL10M_FreqLock;

    // PLL 10.23MHz
    QLineEdit* m_pPLL1023M_BoardTemp;
    QLineEdit* m_pPLL1023M_PowerTemp;
    QLineEdit* m_pPLL1023M_ConstCTemp;
    QLineEdit* m_pPLL1023M_WorkVol;
    QLineEdit* m_pPLL1023M_FreqLock;

    // Addr 0x06
    QLineEdit* m_pAddr06_WorkVol;
    QLineEdit* m_pAddr06_WorkAn;
    QLineEdit* m_pAddr06_BoardTemp;

    // Addr 0x05
    QLineEdit* m_pAddr05_WorkVol;
    QLineEdit* m_pAddr05_WorkAn;
    QLineEdit* m_pAddr05_BoardTemp;

    // Addr 0x04
    QLineEdit* m_pAddr04_WorkVol;
    QLineEdit* m_pAddr04_WorkAn;
    QLineEdit* m_pAddr04_BoardTemp;

    // Addr 0x03
    QLineEdit* m_pAddr03_WorkVol;
    QLineEdit* m_pAddr03_WorkAn;
    QLineEdit* m_pAddr03_BoardTemp;

    // Addr 0x02
    QLineEdit* m_pAddr02_WorkVol;
    QLineEdit* m_pAddr02_WorkAn;
    QLineEdit* m_pAddr02_BoardTemp;

    // Addr 0x01
    QLineEdit* m_pAddr01_WorkVol;
    QLineEdit* m_pAddr01_WorkAn;
    QLineEdit* m_pAddr01_BoardTemp;

    // Addr 0x26
    QLineEdit* m_pAddr26_WorkVol;
    QLineEdit* m_pAddr26_WorkAn;
    QLineEdit* m_pAddr26_BoardTemp;

    // Addr 0x25
    QLineEdit* m_pAddr25_WorkVol;
    QLineEdit* m_pAddr25_WorkAn;
    QLineEdit* m_pAddr25_BoardTemp;

    // Addr 0x24
    QLineEdit* m_pAddr24_WorkVol;
    QLineEdit* m_pAddr24_WorkAn;
    QLineEdit* m_pAddr24_BoardTemp;

    // Addr 0x23
    QLineEdit* m_pAddr23_WorkVol;
    QLineEdit* m_pAddr23_WorkAn;
    QLineEdit* m_pAddr23_BoardTemp;

    // Addr 0x22
    QLineEdit* m_pAddr22_WorkVol;
    QLineEdit* m_pAddr22_WorkAn;
    QLineEdit* m_pAddr22_BoardTemp;

    // Addr 0x21
    QLineEdit* m_pAddr21_WorkVol;
    QLineEdit* m_pAddr21_WorkAn;
    QLineEdit* m_pAddr21_BoardTemp;

private:
    CMyComBasic* m_pMyComBasic;

    char* m_pRefStaInput_Buf;
};

#endif
