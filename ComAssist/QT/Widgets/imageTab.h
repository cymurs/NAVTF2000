#ifndef _IMAGETAB_H_
#define _IMAGETAB_H_

#include <vector>

#include <QWidget>


// --------------------------------------------------------------------------------------
// ���Ӳ��� tab (�ź�ѡ��)
// --------------------------------------------------------------------------------------
class QGroupBox;
class QVBoxLayout;
class QHBoxLayout;

class QLineEdit;
class QTextEdit;
class QPushButton;
class QLabel;
class QComboBox;

class QTimer;
class QSignalMapper;

class CUpdateTab;
class CMyLabel;

// --------------------------------------------------------------------------------------
// ΢�����С
#define WIDTH_CHANGE_LINE_EDIT      70
#define WIDTH_CHANGE_BUTTON         50

#define SEND_CMD_DELAY_MS           (100*3)         // ���ͻ�ȡ��������ʱ���ٺ���

// ����ӳ����
#define ID_OF_10M_FREQ_ADD          0
#define ID_OF_10M_FREQ_SUB          1
#define ID_OF_10M_FREQ_SAVE         2
#define ID_OF_10M_PHAS_ADD          3
#define ID_OF_10M_PHAS_SUB          4
#define ID_OF_10M_PHAS_SAVE         5

#define ID_OF_1023M_FREQ_ADD        6
#define ID_OF_1023M_FREQ_SUB        7
#define ID_OF_1023M_FREQ_SAVE       8
#define ID_OF_1023M_PHAS_ADD        9
#define ID_OF_1023M_PHAS_SUB        10
#define ID_OF_1023M_PHAS_SAVE       11

#define ID_OF_PLUS_ADD              12
#define ID_OF_PLUS_SUB              13

#define ID_OF_DELAY_ADD             14
#define ID_OF_DELAY_SUB             15

// ����
#define ID_OF_10M_FREQ_CLEAR        16
#define ID_OF_10M_PHAS_CLEAR        17
#define ID_OF_1023M_FREQ_CLEAR      18
#define ID_OF_1023M_PHAS_CLEAR      19
#define ID_OF_PLUS_CLEAR            20
#define ID_OF_DELAY_CLEAR           21

// ˥��
#define ID_OF_REDUCE_ADD_6          30
#define ID_OF_REDUCE_SUB_6          31
#define ID_OF_REDUCE_ADD_5          32
#define ID_OF_REDUCE_SUB_5          33
#define ID_OF_REDUCE_ADD_4          34
#define ID_OF_REDUCE_SUB_4          35
#define ID_OF_REDUCE_ADD_3          36
#define ID_OF_REDUCE_SUB_3          37

#define ID_OF_REDUCE_ADD_26         38
#define ID_OF_REDUCE_SUB_26         39
#define ID_OF_REDUCE_ADD_25         40
#define ID_OF_REDUCE_SUB_25         41
#define ID_OF_REDUCE_ADD_24         42
#define ID_OF_REDUCE_SUB_24         43
#define ID_OF_REDUCE_ADD_23         44
#define ID_OF_REDUCE_SUB_23         45

// ���벹��
#define ID_OF_ALIGNCOMPSE_ADD       50
#define ID_OF_ALIGNCOMPSE_SUB       51


// --------------------------------------------------------------------------------------
// �ź�����
struct stSignalInputData
{
    stSignalInputData()
    {
        m_iXLeft = 0;
        m_iYTop = 0;
        m_iWidth = 0;
        m_iHeight = 0;
    }

    // Բ��
    int m_iXLeft;
    int m_iYTop;
    int m_iWidth;
    int m_iHeight;

    // ��ͷ·��
    std::vector<QPoint> m_Path_vector;

    QString m_strName;
};

// ״̬
enum enSelectStatus
{
    en_Empty_sta,
    en_Signal_sta,
    en_Empty_Connect_sta,
    en_Signal_Connect_sta,
};

// �ź�ѡ��
struct stSignalSelectData
{
    stSignalSelectData()
    {
        m_iXLeft = 0;
        m_iYTop = 0;
        m_iWidth = 0;
        m_iHeight = 0;
        m_enSelectStatus = en_Empty_sta;
    }

    // ��ͷ·��
    std::vector<QPoint> m_Path_vector;

    // ѡ������
    QRect m_SelectRect;

    // ��
    QPoint m_sPoint;
    QPoint m_ePoint;

    // Բ��
    int m_iXLeft;
    int m_iYTop;
    int m_iWidth;
    int m_iHeight;

    QString m_strName;

    // ״̬
    enSelectStatus m_enSelectStatus;
};


// ��ͼ����
struct stImageData
{
    stImageData()
    {
        m_iSelectNums = 0;
        m_pCurSigSelect = NULL;
    }

    // ����ߺ�ѡ��
    int m_iSelectNums;
    stSignalSelectData m_stSigSelectArray[5];
    stSignalSelectData* m_pCurSigSelect;
    
    // һ��������
    QPoint m_EmptyPoint;

    // һ���ź�����
    stSignalInputData m_stSigInputData;

};

// ��������
enum en_FuncCmd
{
    en_Refresh_Cmd,
    en_getsta_Cmd,
    en_None_Cmd
};


// --------------------------------------------------------------------------------------
class CMyImageTab : public QWidget
{
    Q_OBJECT

public:
    CMyImageTab(QWidget* parent = 0);
    ~CMyImageTab();

public:
    void SetComNetTab(CUpdateTab* pComNetTab);

    // ���ð�ť״̬(��Զ˿��Ƿ��)
    void SetFuncButtonEnable(bool bEnable);

    // �������
    void CmdFinished_Func();

    // ��������
    void HandleFrameData(unsigned char chFrameCmd, const char* pFrameDataBuf, int iFrameDataLength,
                         unsigned char chSourceAddr, unsigned char chSourcePort);

    void ShowPrioInfo_Func(const QString&, bool);
    bool ComNetCanUse();
    void ShowLock_Func(int iLocalLock, int iRefLock, int i10MLock, int i1023MLock, int,int,int,int);

    // ˢ�����������ȡ�豸״̬��������
    void CmdGetDeviceStatus();
    void CmdClear();

    // ˥������ʾ���յ�������
    void SetReduceShowValue(int iValue, unsigned char chSourceAddr, unsigned char chSourcePort);
    void ClearAllReduceShow();

    // ֻ���� ���ֶ��л��� ģʽ�����л�Ƶ��
    bool CanSwitchSignal();

    // �������ȼ�
    void SetPriority(int iPriority1, int iPriority2, int iPriority3, int iPriority4, int iPriority5, int iPriority6, int iPriorityType);

    // ����Ƶ�ʻ�����λ�ȵ�������ֵ
    void SetFreqAndPhaseEdit(int iIndex, char* pchData);

protected:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);
    void resizeEvent(QResizeEvent*);

private:
    void GetViewportXYWH(int& x, int& y, int& w, int& h);

    void DelayMS(int iMS);

    void StartBeep();
    void StopBeep();

private:    // Image Data
    // �µ����ӽ��ϣ��ɵľ�ȡ��
    void ResetOldImageConnect(int iSelect);

private:
    // ���ͼ�0�����ȡ������������Ϣ
    void SendSubZeroForGetInfo();

    // ˥��
    void ReduceClickFunc(bool bIsAdd, int iShowValue_in, int iAddr, int iPort);

    
    // ��ȡ˥����
    void SendReduceCmdGetInfo();

    // ���벹��
    void CompseFunc(int iValue);

    // ��ȡ ����Ƶ��У׼
    void GetLocalFreqAdj();

    // ���ȼ��ؼ�
    void SetPriorityCtrl(bool bEnable);

    // ����Ƿ񳬳���Χ
    bool ValueOutRange(int iIndex, bool bAdd);

private:
    void CreateWidget();

    QGroupBox* CreateImageGroup();
    QGroupBox* CreatePriorityGroup();
    QGroupBox* CreateEightGroup();
    QHBoxLayout* CreateChangeGroup();
    QHBoxLayout* CreateShowGroup();

    QVBoxLayout* Create10MFreqGroup();
    QVBoxLayout* Create10MPhasGroup();

    QVBoxLayout* CreatePhaseDelayGroup();

private slots:
    void ShowPrioInfo_Slot(const QString&, bool);
    void HandleFrame_Slot(const QString&, unsigned char chSourceAddr, unsigned char chSourcePort);

    void TimerReach();
    

    // ͨ��ӳ�书��
    void ComNetFunc(int);
    void AlignFunc();

    // ���ȼ�
    void PriorityQuery_Slot();
    void PrioritySet_Slot();

    void ShowLock_Slot(int iLocalLock, int iRefLock, int i10MLock, int i1023MLock, int,int,int,int);

signals:
    void ShowPrioInfo_Signal(const QString&, bool);
    void HandleFrame_Signal(const QString&, unsigned char chSourceAddr, unsigned char chSourcePort);
    void ShowLock_Signal(int iLocalLock, int iRefLock, int i10MLock, int i1023MLock, int,int,int,int);

private:
    // ͼ�����򣬰�ť����Ϣ��ʾ
    QGroupBox* m_pImageGroup;

    // 10MHz Ƶ��΢��
    // 1
    QLineEdit* m_p10MFreqTotao1_LineEdit;
    QLineEdit* m_p10MFreqDelta1_LineEdit;
    QPushButton* m_p10MFreqAdd1_Button;
    QPushButton* m_p10MFreqSub1_Button;
    QPushButton* m_p10MFreqClear1_Button;
    QPushButton* m_p10MFreqSave1_Button;

    // 2
    QLineEdit* m_p10MFreqTotao2_LineEdit;
    QLineEdit* m_p10MFreqDelta2_LineEdit;
    QPushButton* m_p10MFreqAdd2_Button;
    QPushButton* m_p10MFreqSub2_Button;
    QPushButton* m_p10MFreqClear2_Button;
    QPushButton* m_p10MFreqSave2_Button;

    // 10.23MHz ��λ΢��
    // 1
    QLineEdit* m_p1023MPhasTotao1_LineEdit;
    QLineEdit* m_p1023MPhasDelta1_LineEdit;
    QPushButton* m_p1023MPhasAdd1_Button;
    QPushButton* m_p1023MPhasSub1_Button;
    QPushButton* m_p1023MPhasClear1_Button;
    QPushButton* m_p1023MPhasSave1_Button;

    // 2
    QLineEdit* m_p1023MPhasTotao2_LineEdit;
    QLineEdit* m_p1023MPhasDelta2_LineEdit;
    QPushButton* m_p1023MPhasAdd2_Button;
    QPushButton* m_p1023MPhasSub2_Button;
    QPushButton* m_p1023MPhasClear2_Button;
    QPushButton* m_p1023MPhasSave2_Button;

    // �������
    QLineEdit* m_pPlusTotal_LineEdit;
    QLineEdit* m_pPlusDelta_LineEdit;
    QPushButton* m_pPlusAdd_Button;
    QPushButton* m_pPlusSub_Button;
    QPushButton* m_pPlusClear_Button;       // ��Ϊ��Ĭ�ϡ�
    QLabel* m_pPlusTip_Label;

    // ��ʱ����
    QLineEdit* m_pDelayTotal_LineEdit;
    QLineEdit* m_pDelayDelta_LineEdit;
    QPushButton* m_pDelayAdd_Button;
    QPushButton* m_pDelaySub_Button;
    QPushButton* m_pDelayClear_Button;

    QPushButton* m_pAlign_Button;

    // ˥��
    QLineEdit*  m_pReduceShow_6_LineEdit;
    QPushButton* m_pReduceAdd_6_Button;
    QPushButton* m_pReduceSub_6_Button;

    QLineEdit*  m_pReduceShow_5_LineEdit;
    QPushButton* m_pReduceAdd_5_Button;
    QPushButton* m_pReduceSub_5_Button;

    QLineEdit*  m_pReduceShow_4_LineEdit;
    QPushButton* m_pReduceAdd_4_Button;
    QPushButton* m_pReduceSub_4_Button;

    QLineEdit*  m_pReduceShow_3_LineEdit;
    QPushButton* m_pReduceAdd_3_Button;
    QPushButton* m_pReduceSub_3_Button;

    QLineEdit*  m_pReduceShow_26_LineEdit;
    QPushButton* m_pReduceAdd_26_Button;
    QPushButton* m_pReduceSub_26_Button;

    QLineEdit*  m_pReduceShow_25_LineEdit;
    QPushButton* m_pReduceAdd_25_Button;
    QPushButton* m_pReduceSub_25_Button;

    QLineEdit*  m_pReduceShow_24_LineEdit;
    QPushButton* m_pReduceAdd_24_Button;
    QPushButton* m_pReduceSub_24_Button;

    QLineEdit*  m_pReduceShow_23_LineEdit;
    QPushButton* m_pReduceAdd_23_Button;
    QPushButton* m_pReduceSub_23_Button;

    // ���벹��
    QLineEdit* m_pAlignCompse_LineEdit;
    QPushButton* m_pAlignCompseAdd_Button;
    QPushButton* m_pAlignCompseSub_Button;

    // �ұ߿ؼ�����
    QGroupBox* m_pEight_Group;
    QGroupBox* m_pPriority_Group;
    QGroupBox* m_p10MDDS_Group;
    QGroupBox* m_pPhaseWidth_Group;
    QGroupBox* m_pPhaseDelay_Group;

    // ���ȼ�����
    QComboBox* m_pPriority1_ComboBox;
    QComboBox* m_pPriority2_ComboBox;
    QComboBox* m_pPriority3_ComboBox;
    QComboBox* m_pPriority4_ComboBox;
    QComboBox* m_pPriority5_ComboBox;
    QComboBox* m_pPriority6_ComboBox;
    QComboBox* m_pPriorityType_ComboBox;

    QPushButton* m_pPriorityQuery_Button;
    QPushButton* m_pPrioritySet_Button;

    QLabel* m_pPriorityInfoShow_Label;

    // �ź�����
    QLabel* m_pInFreqVal_Label;
    QLabel* m_pInFreqPic_Label;

    QLabel* m_pRefFreqVal_Label;
    QLabel* m_pRefFreqPic_Label;

    QLabel* m_p10MFreqVal_Label;
    QLabel* m_p10MFreqPic_Label;

    QLabel* m_p1023MFreqVal_Label;
    QLabel* m_p1023MFreqPic_Label;

private:
    int m_iWinWidth;
    int m_iWinHeight;
    int m_iImageWidth;
    int m_iImageHeight;

    // Beep
    int m_iBeepDiscSec;
    int m_iBeepTimes;

private:
    // ˥����
    int m_iReduceVal_6;
    int m_iReduceVal_5;
    int m_iReduceVal_4;
    int m_iReduceVal_3;

    int m_iReduceVal_26;
    int m_iReduceVal_25;
    int m_iReduceVal_24;
    int m_iReduceVal_23;


private:
    stImageData m_TopImageData;
    stImageData m_BottomImageData;

    QFont* m_pOriFont;
    QFont* m_pUseFont;

    en_FuncCmd m_enFuncCmd;
    bool m_bComNetCanUse;

    char* m_pRefStaInput_Buf;

    QSignalMapper* m_pSignalMapper;

    QTimer* m_pTimer;                           // 1�붨ʱ
    int m_iPrioInfoHide_Count;                  // ��Ϣ��ʾʱ��
    int m_iPrioCmdTimeout_Count;                // �����·���ʱ

    // ���ȼ�(�� 1 ��ʼ)
    int m_iPriority1_Cur;
    int m_iPriority2_Cur;
    int m_iPriority3_Cur;
    int m_iPriority4_Cur;
    int m_iPriority5_Cur;
    int m_iPriority6_Cur;
    int m_iPriorityType_Cur;

    int m_iPriority1_New;
    int m_iPriority2_New;
    int m_iPriority3_New;
    int m_iPriority4_New;
    int m_iPriority5_New;
    int m_iPriority6_New;
    int m_iPriorityType_New;


    int m_iPrioSetTimeout_Count;            // ���� ���ȼ� ��ʱʱ��
    bool m_bQueryPriority;

    int m_iOutputClk_Count;                 // ���10s�忨�������

private:
    CUpdateTab* m_pComNetTab;

    CMyLabel* m_pMyLabel;

};

#endif
