#ifndef _GNSSTAB_H_
#define _GNSSTAB_H_

#include <QWidget>

#include <vector>

// --------------------------------------------------------------------------------------
// GNSS 接收机 tab
// --------------------------------------------------------------------------------------
class CUpdateTab;

class QGroupBox;
class QVBoxLayout;
class QLabel;
class QComboBox;
class QPushButton;
class QLineEdit;
class QTextEdit;
class QTableWidget;

// --------------------------------------------------------------------------------------
// 参数
struct st_ParamInfo
{
    st_ParamInfo()
    {
        m_iIndex = 0;
        m_iDataDefaultIndex = 0;
    }
    ~st_ParamInfo()
    {
        m_Data_vector.clear();
    }

    int m_iIndex;                           // 序号
    QString m_strParamName;                 // 参数名称

    int m_iDataDefaultIndex;                // 缺省数据的索引
    std::vector<QString> m_Data_vector;     // 参数的数据
};

// 命令
struct st_CmdInfo
{
    st_CmdInfo()
    {
        m_iIndex = 0;
    }
    ~st_CmdInfo()
    {
        m_Param_vector.clear();
    }

    int m_iIndex;                               // 序号
    QString m_strCmdName;                       // 命令名称
    QString m_strDesc;                          // 说明
    
    std::vector<st_ParamInfo> m_Param_vector;   // 参数
};

// 执行命令
struct st_CmdExe
{
    st_CmdExe()
    {
        ;
    }

    void Clear()
    {
        m_strCmdName = "";
        m_Param_vector.clear();
    }

    void SetCmdName(QString& strCmdName)
    {
        m_strCmdName = strCmdName;
    }

    void AddOneParam(QString& strParam)
    {
        m_Param_vector.push_back(strParam);
    }

    void ChangeOneParam(int iIndex, QString& strParam)
    {
        if((iIndex >= 0) && (iIndex < (int)m_Param_vector.size()))
        {
            m_Param_vector[iIndex] = strParam;
        }
        else
        {
            AddOneParam(strParam);
        }
    }

    QString CombineToCmd()
    {
        QString strInfo = m_strCmdName;
        for(int i = 0; i < (int)m_Param_vector.size(); ++i)
        {
            strInfo += ", " + m_Param_vector[i];
        }
        return strInfo;
    }

    QString m_strCmdName;
    std::vector<QString> m_Param_vector;
};

// --------------------------------------------------------------------------------------
class CGNSSTab : public QWidget
{
    Q_OBJECT
public:
    CGNSSTab(QWidget* parent = 0);
    ~CGNSSTab();

public:
    void SetComNetTab(CUpdateTab* pComNetTab);

    // 设置按钮状态(针对端口是否打开)
    void SetFuncButtonEnable(bool bEnable);

    // 处理数据
    void HandleFrameData(unsigned char chFrameCmd, const char* pFrameDataBuf, int iFrameDataLength,
                         unsigned char chSourceAddr, unsigned char chSourcePort);

    void ShowInfo_Func(const QString strInfo, QColor rgb = QColor(0, 0, 0));

protected:
    void paintEvent(QPaintEvent*);
    void keyPressEvent(QKeyEvent*);

private:
    void InitCmdInfo();

    void InitCmdCtrl();

private slots:
    void CmdNameChanged_Slot(int);
    void CmdDescChanged_Slot(int);

    void CmdUseClicked_Slot();

    void Param0Changed_Slot();
    void Param1Changed_Slot();
    void Param2Changed_Slot();
    void Param3Changed_Slot();
    void Param4Changed_Slot();
    void Param5Changed_Slot();
    void Param6Changed_Slot();

    void Param0Changed_Slot(const QString&);
    void Param1Changed_Slot(const QString&);
    void Param2Changed_Slot(const QString&);
    void Param3Changed_Slot(const QString&);
    void Param4Changed_Slot(const QString&);
    void Param5Changed_Slot(const QString&);
    void Param6Changed_Slot(const QString&);

    void SendCmd_Slot();

    void HandleFrame_Slot(const QString&, unsigned char chSourceAddr, unsigned char chSourcePort);

    void ClearInfo_Slot();
    void ShowInfo_Slot(const QString strInfo, QColor);

signals:
    void HandleFrame_Signal(const QString&, unsigned char chSourceAddr, unsigned char chSourcePort);

    void ShowInfo_Signal(const QString strInfo, QColor);

private:
    void CreateWidgets();

    QGroupBox* CreateCmdGroup();
    QVBoxLayout* CreateShowLayout();
    QGroupBox* CreateFuncGroup();

    QGroupBox* CreateCmdInfoGroup();
    QGroupBox* CreateParamInfoGroup(QString strName, QLabel*& pLabel, QComboBox*& pComboBox, QPushButton*& pButton);


private:
    // 命令
    QComboBox* m_pCmdName_ComboBox;
    QComboBox* m_pCmdDesc_ComboBox;
    QPushButton* m_pCmdUse_Button;

    // 参数
    QLabel* m_pParam0Name_Label;
    QComboBox* m_pParam0Data_ComboBox;
    QPushButton* m_pParam0Change_Button;

    // 参数
    QLabel* m_pParam1Name_Label;
    QComboBox* m_pParam1Data_ComboBox;
    QPushButton* m_pParam1Change_Button;

    // 参数
    QLabel* m_pParam2Name_Label;
    QComboBox* m_pParam2Data_ComboBox;
    QPushButton* m_pParam2Change_Button;

    // 参数
    QLabel* m_pParam3Name_Label;
    QComboBox* m_pParam3Data_ComboBox;
    QPushButton* m_pParam3Change_Button;

    // 参数
    QLabel* m_pParam4Name_Label;
    QComboBox* m_pParam4Data_ComboBox;
    QPushButton* m_pParam4Change_Button;

    // 参数
    QLabel* m_pParam5Name_Label;
    QComboBox* m_pParam5Data_ComboBox;
    QPushButton* m_pParam5Change_Button;

    // 参数
    QLabel* m_pParam6Name_Label;
    QComboBox* m_pParam6Data_ComboBox;
    QPushButton* m_pParam6Change_Button;

    QTableWidget* m_pParamShow_Table;

    QGroupBox* m_pParam0_Group;
    QGroupBox* m_pParam1_Group;
    QGroupBox* m_pParam2_Group;
    QGroupBox* m_pParam3_Group;
    QGroupBox* m_pParam4_Group;
    QGroupBox* m_pParam5_Group;
    QGroupBox* m_pParam6_Group;

    // 功能
    QPushButton* m_pClearInfo_Button;
    QLineEdit* m_pSendData_LineEdit;
    QPushButton* m_pSendCmd_Button;

    // 显示
    QTextEdit* m_pInfoShow_TextEdit;

private:
    CUpdateTab* m_pComNetTab;

    char* m_pInput_Buf;

private:
    std::vector<st_CmdInfo> m_Cmd_vector;
    st_CmdExe m_oCmdExe;
};

#endif
