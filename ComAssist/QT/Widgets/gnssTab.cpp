#include "stdafx.h"
#include "gnssTab.h"

#include <QtGui>
#include <QPushButton>
#include <QStyleOption>

#include "../../../Common/Function/Function.h"
#include "../../../Common/Define/DefineFile.h"

#include "updateTab.h"

// --------------------------------------------------------------------------------------
QString g_strGNSSAddr = "0";            // GNSS �忨��Ŀ�ĵ�ַ
QString g_strGNSSPort = "0";

// --------------------------------------------------------------------------------------
// GNSS ���ջ� tab
// --------------------------------------------------------------------------------------
CGNSSTab::CGNSSTab(QWidget* parent) : QWidget(parent)
{
    m_pComNetTab = NULL;

    m_pInput_Buf = new char[SIZE_OF_INPUT_BUF];

    connect(this, SIGNAL(HandleFrame_Signal(const QString&, unsigned char, unsigned char)), this, SLOT(HandleFrame_Slot(const QString&, unsigned char, unsigned char)));
    connect(this, SIGNAL(ShowInfo_Signal(const QString, QColor)), this, SLOT(ShowInfo_Slot(const QString, QColor)));

    InitCmdInfo();
    CreateWidgets();
    InitCmdCtrl();

}
CGNSSTab::~CGNSSTab()
{
    delete[] m_pInput_Buf;
}

void CGNSSTab::SetComNetTab(CUpdateTab* pComNetTab)
{
    m_pComNetTab = pComNetTab;
}

// ���ð�ť״̬(��Զ˿��Ƿ��)
void CGNSSTab::SetFuncButtonEnable(bool bEnable)
{
    m_pSendCmd_Button->setEnabled(bEnable);
}

void CGNSSTab::paintEvent(QPaintEvent*)
{
    // Style Sheet
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void CGNSSTab::keyPressEvent(QKeyEvent* keyEvent)
{
    if(Qt::Key_Return == keyEvent->key() || Qt::Key_Enter == keyEvent->key())
    {
        if((m_pSendData_LineEdit->hasFocus()) && (m_pSendData_LineEdit->text().length() > 0))
        {
            if(true == m_pSendCmd_Button->isEnabled()) SendCmd_Slot();
        }
    }
}


// ��������
void CGNSSTab::HandleFrameData(unsigned char chFrameCmd, const char* pFrameDataBuf, int iFrameDataLength,
                                   unsigned char chSourceAddr, unsigned char chSourcePort)
{
    QString strData = pFrameDataBuf;
    emit HandleFrame_Signal(strData, chSourceAddr, chSourcePort);
}
void CGNSSTab::HandleFrame_Slot(const QString& strData, unsigned char chSourceAddr, unsigned char chSourcePort)
{
    ShowInfo_Func(strData);
}

void CGNSSTab::CreateWidgets()
{
    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(CreateCmdGroup(), 1);
    pHLayout->addLayout(CreateShowLayout(), 1);

    QVBoxLayout* pVLayout = new QVBoxLayout();
    pVLayout->addLayout(pHLayout);
    pVLayout->addWidget(CreateFuncGroup());

    setLayout(pVLayout);
}

void CGNSSTab::InitCmdCtrl()
{
    for(int i = 0; i < (int)m_Cmd_vector.size(); ++i)
    {
        st_CmdInfo& oCmd = m_Cmd_vector[i];
        m_pCmdName_ComboBox->addItem(oCmd.m_strCmdName);
        m_pCmdDesc_ComboBox->addItem(oCmd.m_strDesc);
    }
}

QGroupBox* CGNSSTab::CreateCmdGroup()
{
    QGroupBox* pGroup = new QGroupBox(tr(""));

    m_pParam0_Group = CreateParamInfoGroup("����һ", m_pParam0Name_Label, m_pParam0Data_ComboBox, m_pParam0Change_Button);
    m_pParam1_Group = CreateParamInfoGroup("������", m_pParam1Name_Label, m_pParam1Data_ComboBox, m_pParam1Change_Button);
    m_pParam2_Group = CreateParamInfoGroup("������", m_pParam2Name_Label, m_pParam2Data_ComboBox, m_pParam2Change_Button);
    m_pParam3_Group = CreateParamInfoGroup("������", m_pParam3Name_Label, m_pParam3Data_ComboBox, m_pParam3Change_Button);
    m_pParam4_Group = CreateParamInfoGroup("������", m_pParam4Name_Label, m_pParam4Data_ComboBox, m_pParam4Change_Button);
    m_pParam5_Group = CreateParamInfoGroup("������", m_pParam5Name_Label, m_pParam5Data_ComboBox, m_pParam5Change_Button);
    m_pParam6_Group = CreateParamInfoGroup("������", m_pParam6Name_Label, m_pParam6Data_ComboBox, m_pParam6Change_Button);

    connect(m_pParam0Data_ComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(Param0Changed_Slot(const QString&)));
    connect(m_pParam1Data_ComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(Param1Changed_Slot(const QString&)));
    connect(m_pParam2Data_ComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(Param2Changed_Slot(const QString&)));
    connect(m_pParam3Data_ComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(Param3Changed_Slot(const QString&)));
    connect(m_pParam4Data_ComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(Param4Changed_Slot(const QString&)));
    connect(m_pParam5Data_ComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(Param5Changed_Slot(const QString&)));
    connect(m_pParam6Data_ComboBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(Param6Changed_Slot(const QString&)));

    connect(m_pParam0Data_ComboBox, SIGNAL(editTextChanged(const QString&)), this, SLOT(Param0Changed_Slot(const QString&)));
    connect(m_pParam1Data_ComboBox, SIGNAL(editTextChanged(const QString&)), this, SLOT(Param1Changed_Slot(const QString&)));
    connect(m_pParam2Data_ComboBox, SIGNAL(editTextChanged(const QString&)), this, SLOT(Param2Changed_Slot(const QString&)));
    connect(m_pParam3Data_ComboBox, SIGNAL(editTextChanged(const QString&)), this, SLOT(Param3Changed_Slot(const QString&)));
    connect(m_pParam4Data_ComboBox, SIGNAL(editTextChanged(const QString&)), this, SLOT(Param4Changed_Slot(const QString&)));
    connect(m_pParam5Data_ComboBox, SIGNAL(editTextChanged(const QString&)), this, SLOT(Param5Changed_Slot(const QString&)));
    connect(m_pParam6Data_ComboBox, SIGNAL(editTextChanged(const QString&)), this, SLOT(Param6Changed_Slot(const QString&)));

    // Table
    m_pParamShow_Table = new QTableWidget();
    m_pParamShow_Table->verticalHeader()->setVisible(false);
    m_pParamShow_Table->setSelectionBehavior(QTableWidget::SelectRows);
    m_pParamShow_Table->setEditTriggers(QTableWidget::NoEditTriggers);
    m_pParamShow_Table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pParamShow_Table->resizeColumnsToContents();
    m_pParamShow_Table->setSortingEnabled(false);

    QVBoxLayout* pVLayout = new QVBoxLayout();
    pVLayout->addWidget(CreateCmdInfoGroup());
    pVLayout->addWidget(m_pParam0_Group);
    pVLayout->addWidget(m_pParam1_Group);
    pVLayout->addWidget(m_pParam2_Group);
    pVLayout->addWidget(m_pParam3_Group);
    pVLayout->addWidget(m_pParam4_Group);
    pVLayout->addWidget(m_pParam5_Group);
    pVLayout->addWidget(m_pParam6_Group);
    pVLayout->addStretch();
    pVLayout->addWidget(m_pParamShow_Table, 1);

    pGroup->setLayout(pVLayout);
    return pGroup;
}
QVBoxLayout* CGNSSTab::CreateShowLayout()
{
    m_pInfoShow_TextEdit = new QTextEdit();
    m_pInfoShow_TextEdit->setReadOnly(true);
    m_pInfoShow_TextEdit->document()->setMaximumBlockCount(1024);

    QVBoxLayout* pVLayout = new QVBoxLayout();
    pVLayout->addWidget(m_pInfoShow_TextEdit);

    return pVLayout;
}
QGroupBox* CGNSSTab::CreateFuncGroup()
{
    QGroupBox* pGroup = new QGroupBox(tr(""));

    m_pClearInfo_Button = new QPushButton(tr("�� ��"));
    m_pSendData_LineEdit = new QLineEdit();
    m_pSendCmd_Button = new QPushButton(tr("�� ��"));
    m_pSendCmd_Button->setEnabled(false);

    m_pClearInfo_Button->setMinimumHeight(50);
    m_pSendData_LineEdit->setMinimumHeight(50);
    m_pSendData_LineEdit->setObjectName("GnssSend_LineEdit");
    m_pSendCmd_Button->setMinimumHeight(50);

    // QSS
    m_pSendData_LineEdit->setStyleSheet("font-size:25px;");
    
    connect(m_pClearInfo_Button, SIGNAL(clicked()), this, SLOT(ClearInfo_Slot()));
    connect(m_pSendCmd_Button, SIGNAL(clicked()), this, SLOT(SendCmd_Slot()));

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(m_pClearInfo_Button);
    pHLayout->addWidget(m_pSendData_LineEdit);
    pHLayout->addWidget(m_pSendCmd_Button);

    pGroup->setLayout(pHLayout);
    return pGroup;
}

QGroupBox* CGNSSTab::CreateCmdInfoGroup()
{
    QGroupBox* pGroup = new QGroupBox(tr(""));

    QLabel* pLabel = new QLabel(tr("����"));
    m_pCmdName_ComboBox = new QComboBox();
    m_pCmdDesc_ComboBox = new QComboBox();
    m_pCmdUse_Button = new QPushButton(tr("ʹ ��"));

    // QSS
    m_pCmdName_ComboBox->setStyleSheet(tr("min-height: 25px;font-size: 25px;"));
    m_pCmdDesc_ComboBox->setStyleSheet(tr("min-height: 25px;font-size: 15px;"));
    m_pCmdUse_Button->setStyleSheet(tr("min-height: 25px;"));

    connect(m_pCmdName_ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(CmdNameChanged_Slot(int)));
    connect(m_pCmdDesc_ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(CmdDescChanged_Slot(int)));
    connect(m_pCmdUse_Button, SIGNAL(clicked()), this, SLOT(CmdUseClicked_Slot()));

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(pLabel);
    pHLayout->addWidget(m_pCmdName_ComboBox, 1);
    pHLayout->addWidget(m_pCmdDesc_ComboBox, 1);
    pHLayout->addWidget(m_pCmdUse_Button);

    pGroup->setLayout(pHLayout);
    return pGroup;
}
QGroupBox* CGNSSTab::CreateParamInfoGroup(QString strName, QLabel*& pLabel, QComboBox*& pComboBox, QPushButton*& pButton)
{
    QGroupBox* pGroup = new QGroupBox(tr(""));

    QLabel* pIndexLabel = new QLabel(strName);
    pLabel = new QLabel(tr(""));
    pLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    pComboBox = new QComboBox();
    pComboBox->setEditable(true);
    pButton = new QPushButton(tr("�޸�"));              // ��ʹ��
    pButton->hide();

    // QSS
    pComboBox->setStyleSheet(tr("min-height: 25px;font-size: 15px;"));

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(pIndexLabel);
    pHLayout->addWidget(pLabel, 1);
    pHLayout->addWidget(pComboBox, 1);

    pGroup->setLayout(pHLayout);
    return pGroup;
}

void CGNSSTab::CmdUseClicked_Slot()
{
    m_pCmdUse_Button->setEnabled(false);

    m_pParam0Change_Button->setEnabled(true);
    m_pParam1Change_Button->setEnabled(true);
    m_pParam2Change_Button->setEnabled(true);
    m_pParam3Change_Button->setEnabled(true);
    m_pParam4Change_Button->setEnabled(true);
    m_pParam5Change_Button->setEnabled(true);
    m_pParam6Change_Button->setEnabled(true);

    m_oCmdExe.Clear();

    int iCurIndex = m_pCmdName_ComboBox->currentIndex();
    st_CmdInfo& oCmd = m_Cmd_vector[iCurIndex];

    m_oCmdExe.SetCmdName(oCmd.m_strCmdName);
    int iParamNum = (int)oCmd.m_Param_vector.size();
    QStringList listParam;
    switch(iParamNum)
    {
    case 7:
        listParam << m_pParam6Data_ComboBox->currentText();
    case 6:
        listParam << m_pParam5Data_ComboBox->currentText();
    case 5:
        listParam << m_pParam4Data_ComboBox->currentText();
    case 4:
        listParam << m_pParam3Data_ComboBox->currentText();
    case 3:
        listParam << m_pParam2Data_ComboBox->currentText();
    case 2:
        listParam << m_pParam1Data_ComboBox->currentText();
    case 1:
        listParam << m_pParam0Data_ComboBox->currentText();
    default:
        break;
    }

    for(int i = iParamNum-1; i >= 0; --i)
    {
        m_oCmdExe.AddOneParam(listParam[i]);
    }

    m_pSendData_LineEdit->setText(m_oCmdExe.CombineToCmd());

    m_pCmdUse_Button->setEnabled(true);
}

void CGNSSTab::Param0Changed_Slot()
{
    m_oCmdExe.ChangeOneParam(0, m_pParam0Data_ComboBox->currentText());
    m_pSendData_LineEdit->setText(m_oCmdExe.CombineToCmd());
}
void CGNSSTab::Param1Changed_Slot()
{
    m_oCmdExe.ChangeOneParam(1, m_pParam1Data_ComboBox->currentText());
    m_pSendData_LineEdit->setText(m_oCmdExe.CombineToCmd());
}
void CGNSSTab::Param2Changed_Slot()
{
    m_oCmdExe.ChangeOneParam(2, m_pParam2Data_ComboBox->currentText());
    m_pSendData_LineEdit->setText(m_oCmdExe.CombineToCmd());
}
void CGNSSTab::Param3Changed_Slot()
{
    m_oCmdExe.ChangeOneParam(3, m_pParam3Data_ComboBox->currentText());
    m_pSendData_LineEdit->setText(m_oCmdExe.CombineToCmd());
}
void CGNSSTab::Param4Changed_Slot()
{
    m_oCmdExe.ChangeOneParam(4, m_pParam4Data_ComboBox->currentText());
    m_pSendData_LineEdit->setText(m_oCmdExe.CombineToCmd());
}
void CGNSSTab::Param5Changed_Slot()
{
    m_oCmdExe.ChangeOneParam(5, m_pParam5Data_ComboBox->currentText());
    m_pSendData_LineEdit->setText(m_oCmdExe.CombineToCmd());
}
void CGNSSTab::Param6Changed_Slot()
{
    m_oCmdExe.ChangeOneParam(6, m_pParam6Data_ComboBox->currentText());
    m_pSendData_LineEdit->setText(m_oCmdExe.CombineToCmd());
}

void CGNSSTab::Param0Changed_Slot(const QString& strInfo)
{
    Param0Changed_Slot();
}
void CGNSSTab::Param1Changed_Slot(const QString& strInfo)
{
    Param1Changed_Slot();
}
void CGNSSTab::Param2Changed_Slot(const QString& strInfo)
{
    Param2Changed_Slot();
}
void CGNSSTab::Param3Changed_Slot(const QString& strInfo)
{
    Param3Changed_Slot();
}
void CGNSSTab::Param4Changed_Slot(const QString& strInfo)
{
    Param4Changed_Slot();
}
void CGNSSTab::Param5Changed_Slot(const QString& strInfo)
{
    Param5Changed_Slot();
}
void CGNSSTab::Param6Changed_Slot(const QString& strInfo)
{
    Param6Changed_Slot();
}

void CGNSSTab::CmdNameChanged_Slot(int iIndex)
{
    if(iIndex < 0) return;
    if(iIndex >= (int)m_Cmd_vector.size()) return;

    m_pSendData_LineEdit->setText("");
    m_pParamShow_Table->clear();
    m_pParamShow_Table->clearContents();
    m_pCmdDesc_ComboBox->setCurrentIndex(iIndex);

    st_CmdInfo& oCmd = m_Cmd_vector[iIndex];
    int iParamNum = (int)oCmd.m_Param_vector.size();
    switch(iParamNum)
    {
    case 7:
        m_pParam6_Group->show();
    case 6:
        m_pParam5_Group->show();
    case 5:
        m_pParam4_Group->show();
    case 4:
        m_pParam3_Group->show();
    case 3:
        m_pParam2_Group->show();
    case 2:
        m_pParam1_Group->show();
    case 1:
        m_pParam0_Group->show();
    default:
        break;
    }
    switch(iParamNum)
    {
    case 0:
        m_pParam0_Group->hide();
    case 1:
        m_pParam1_Group->hide();
    case 2:
        m_pParam2_Group->hide();
    case 3:
        m_pParam3_Group->hide();
    case 4:
        m_pParam4_Group->hide();
    case 5:
        m_pParam5_Group->hide();
    case 6:
        m_pParam6_Group->hide();
    default:
        break;
    }
    m_pParam0Change_Button->setEnabled(false);
    m_pParam1Change_Button->setEnabled(false);
    m_pParam2Change_Button->setEnabled(false);
    m_pParam3Change_Button->setEnabled(false);
    m_pParam4Change_Button->setEnabled(false);
    m_pParam5Change_Button->setEnabled(false);
    m_pParam6Change_Button->setEnabled(false);
    
    int iMaxSize = 0;
    int iDataSize = 0;
    QString strShow = "";
    for(int i = 0; i < iParamNum; ++i)
    {
        st_ParamInfo& oParam = oCmd.m_Param_vector[i];
        QLabel* pParamName = NULL;
        QComboBox* pParamDataCombo = NULL;
        switch(i)
        {
        case 0:
            pParamName = m_pParam0Name_Label;
            pParamDataCombo = m_pParam0Data_ComboBox;
            break;
        case 1:
            pParamName = m_pParam1Name_Label;
            pParamDataCombo = m_pParam1Data_ComboBox;
            break;
        case 2:
            pParamName = m_pParam2Name_Label;
            pParamDataCombo = m_pParam2Data_ComboBox;
            break;
        case 3:
            pParamName = m_pParam3Name_Label;
            pParamDataCombo = m_pParam3Data_ComboBox;
            break;
        case 4:
            pParamName = m_pParam4Name_Label;
            pParamDataCombo = m_pParam4Data_ComboBox;
            break;
        case 5:
            pParamName = m_pParam5Name_Label;
            pParamDataCombo = m_pParam5Data_ComboBox;
            break;
        case 6:
            pParamName = m_pParam6Name_Label;
            pParamDataCombo = m_pParam6Data_ComboBox;
            break;
        default:
            break;
        }

        strShow += tr("���� %1\t").arg(i+1);

        if(NULL == pParamDataCombo) continue;
        pParamDataCombo->clear();
        pParamName->setText(oParam.m_strParamName);
        iDataSize = (int)oParam.m_Data_vector.size();
        iMaxSize = qMax(iMaxSize, iDataSize);
        for(int j = 0; j < iDataSize; ++j)
        {
            pParamDataCombo->addItem(oParam.m_Data_vector[j]);
        }
        pParamDataCombo->setCurrentIndex(oParam.m_iDataDefaultIndex);
    }

    // ��ʾ����
    m_pParamShow_Table->setColumnCount(iParamNum);
    QStringList headerList;
    for(int i = 0; i < iParamNum; ++i)
    {
        headerList << tr("���� %1").arg(i+1);
    }
    m_pParamShow_Table->setHorizontalHeaderLabels(headerList);

    m_pParamShow_Table->setRowCount(iMaxSize);
    QTableWidgetItem* pItem = NULL;
    for(int i = 0; i < iMaxSize; ++i)
    {
        for(int j = 0; j < iParamNum; ++j)
        {
            strShow = " ";
            st_ParamInfo& oParam = oCmd.m_Param_vector[j];
            if(i < (int)oParam.m_Data_vector.size())            // ��
            {
                strShow = tr("%1").arg(oParam.m_Data_vector[i]);
            }

            pItem = new QTableWidgetItem(strShow);
            pItem->setTextAlignment(Qt::AlignCenter);
            m_pParamShow_Table->setItem(i, j, pItem);
        }
    }

    // �޸��п�
    for(int i = 0; i < iParamNum; ++i)
    {
        m_pParamShow_Table->resizeColumnToContents(i);
    }

    // ��ʾ�������
    CmdUseClicked_Slot();
}
void CGNSSTab::CmdDescChanged_Slot(int iIndex)
{
    if(iIndex < 0) return;
    if(iIndex >= (int)m_Cmd_vector.size()) return;

    m_pCmdName_ComboBox->setCurrentIndex(iIndex);
}

void CGNSSTab::ClearInfo_Slot()
{
    m_pInfoShow_TextEdit->clear();
}
void CGNSSTab::ShowInfo_Func(const QString strInfo, QColor rgb)
{
    emit ShowInfo_Signal(strInfo, rgb);
}
void CGNSSTab::ShowInfo_Slot(const QString strInfo, QColor rgb)
{
    static bool bColor = false;

    int iR = 0;
    int iG = 0;
    int iB = 0;
    rgb.getRgb(&iR, &iG, &iB);

    if((0 == iR) && (0 == iG) && (0 == iB))
    {
        bColor = !bColor;
        if(true == bColor)
        {
            m_pInfoShow_TextEdit->setTextColor(QColor(0, 0, 250));
        }
        else
        {
            m_pInfoShow_TextEdit->setTextColor(QColor(0, 0, 10));
        }
    }
    else
    {
        m_pInfoShow_TextEdit->setTextColor(rgb);
    }
    
    m_pInfoShow_TextEdit->append(strInfo);

    // Auto scroll
    QTextCursor oCursor = m_pInfoShow_TextEdit->textCursor();
    oCursor.movePosition(QTextCursor::End);
    m_pInfoShow_TextEdit->setTextCursor(oCursor);
}

void CGNSSTab::SendCmd_Slot()
{
    QString strCmd = m_pSendData_LineEdit->text();
    if(strCmd.isEmpty()) return;

    // ����ԭ�е�Ŀ�ĵ�ַ
    QString strAddr = "";
    QString strPort = "";
    m_pComNetTab->GetTargetForGNSS(strAddr, strPort);

    // ���� GNSS Ŀ�ĵ�ַ
    m_pComNetTab->SetTargetForGNSS(g_strGNSSAddr, g_strGNSSPort);

    // ����������Դ��ַ����Ϣ
    m_pComNetTab->SetSourceTargetInfo(COMMAND_IS_XXB2);
    m_pComNetTab->SetFrameDataStyle(true, true, true, true);

    // ����
    char chData[256] = {0};
    strcpy_s(chData, sizeof(chData), strCmd.toLatin1().data());

    // д����/���ڡ�pszDataBuf�������������ݣ�������Դ��ַ��������Ϣ
    m_pComNetTab->SendComNetData(0, chData, 0);

    // �ָ�ԭ�е�Ŀ�ĵ�ַ
    m_pComNetTab->SetTargetForGNSS(strAddr, strPort);
    m_pSendData_LineEdit->setText("");
}



void CGNSSTab::InitCmdInfo()
{
    int iCmdIndex = 0;

    // 3.1 Receiver Administration Commands ====================================
    // lstAntennaInfo,    Antenna
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "lstAntennaInfo";
        oCmdInfo.m_strDesc = "�г����ߵ���Ϣ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Antenna";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("Overview");
        oParam0.m_Data_vector.push_back("Main");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // lstCommandHelp, Action
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "lstCommandHelp";
        oCmdInfo.m_strDesc = "�г�������Ϣ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Action";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("Overview");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // lstConfigFile, File
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "lstConfigFile";
        oCmdInfo.m_strDesc = "�г������ļ�������";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "File";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("Current");
        oParam0.m_Data_vector.push_back("Boot");
        oParam0.m_Data_vector.push_back("RxDefault");
        oParam0.m_Data_vector.push_back("User1");
        oParam0.m_Data_vector.push_back("User2");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // exeCopyConfigFile, Source, Target
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "exeCopyConfigFile";
        oCmdInfo.m_strDesc = "���������ļ�";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Source";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("Current");
        oParam0.m_Data_vector.push_back("Boot");
        oParam0.m_Data_vector.push_back("User1");
        oParam0.m_Data_vector.push_back("User2");
        oParam0.m_Data_vector.push_back("RxDefault");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Target";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("Current");
        oParam1.m_Data_vector.push_back("Boot");
        oParam1.m_Data_vector.push_back("User1");
        oParam1.m_Data_vector.push_back("User2");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getCopyConfigFile
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getCopyConfigFile";
        oCmdInfo.m_strDesc = "�г������ļ�";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // lstInternalFile, File
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "lstInternalFile";
        oCmdInfo.m_strDesc = "�г����ջ��ڲ��ļ�������";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "File";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("Permissions");
        oParam0.m_Data_vector.push_back("Identification");
        oParam0.m_Data_vector.push_back("Debug");
        oParam0.m_Data_vector.push_back("Error");
        oParam0.m_Data_vector.push_back("SisError");
        oParam0.m_Data_vector.push_back("DiffCorrError");
        oParam0.m_Data_vector.push_back("SetupError");
        oParam0.m_Data_vector.push_back("LBAS1Access");
        oParam0.m_Data_vector.push_back("LBAS1Subscr");
        oParam0.m_Data_vector.push_back("IPParameters");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getReceiverCapabilities
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getReceiverCapabilities";
        oCmdInfo.m_strDesc = "�г����ջ��Ĺ��ܷ�Χ";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getReceiverInterface, Item
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getReceiverInterface";
        oCmdInfo.m_strDesc = "�г����ջ��İ汾��Ϣ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Item";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+RxName");
        oParam0.m_Data_vector.push_back("+SNMPLanguage");
        oParam0.m_Data_vector.push_back("+SNMPVersion");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // exeResetReceiver, Level, EraseMemory
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "exeResetReceiver";
        oCmdInfo.m_strDesc = "��λ���ջ����ҿ���ˢ��ָ������";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Level";
        oParam0.m_iDataDefaultIndex = 1;

        // ��������
        oParam0.m_Data_vector.push_back("Soft");
        oParam0.m_Data_vector.push_back("Hard");
        oParam0.m_Data_vector.push_back("Upgrade");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "EraseMemory";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("none");
        oParam1.m_Data_vector.push_back("+Config");
        oParam1.m_Data_vector.push_back("+PVTData");
        oParam1.m_Data_vector.push_back("+SatData");
        oParam1.m_Data_vector.push_back("+BaseStations");
        oParam1.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getResetReceiver
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getResetReceiver";
        oCmdInfo.m_strDesc = "�г����ջ���λ�ķ�ʽ";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // 3.2 Authentication Commands =============================================
    // lstCurrentUser
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "lstCurrentUser";
        oCmdInfo.m_strDesc = "�г���¼���û�";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setDefaultAccessLevel, Web, Ftp, Ip, Com, Usb
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setDefaultAccessLevel";
        oCmdInfo.m_strDesc = "���������û��Ĳ���Ȩ��";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Web";
        oParam0.m_iDataDefaultIndex = 2;

        // ��������
        oParam0.m_Data_vector.push_back("none");
        oParam0.m_Data_vector.push_back("Viewer");
        oParam0.m_Data_vector.push_back("User");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Ftp";
        oParam1.m_iDataDefaultIndex = 2;

        // ��������
        oParam1.m_Data_vector.push_back("none");
        oParam1.m_Data_vector.push_back("Viewer");
        oParam1.m_Data_vector.push_back("User");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "Ip";
        oParam2.m_iDataDefaultIndex = 2;

        // ��������
        oParam2.m_Data_vector.push_back("none");
        oParam2.m_Data_vector.push_back("Viewer");
        oParam2.m_Data_vector.push_back("User");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ����
        st_ParamInfo oParam3;
        oParam3.m_iIndex = iParamIndex++;
        oParam3.m_strParamName = "Com";
        oParam3.m_iDataDefaultIndex = 2;

        // ��������
        oParam3.m_Data_vector.push_back("none");
        oParam3.m_Data_vector.push_back("Viewer");
        oParam3.m_Data_vector.push_back("User");
        oCmdInfo.m_Param_vector.push_back(oParam3);

        // ����
        st_ParamInfo oParam4;
        oParam4.m_iIndex = iParamIndex++;
        oParam4.m_strParamName = "Usb";
        oParam4.m_iDataDefaultIndex = 2;

        // ��������
        oParam4.m_Data_vector.push_back("none");
        oParam4.m_Data_vector.push_back("Viewer");
        oParam4.m_Data_vector.push_back("User");
        oCmdInfo.m_Param_vector.push_back(oParam4);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getDefaultAccessLevel
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getDefaultAccessLevel";
        oCmdInfo.m_strDesc = "�г������û��Ĳ���Ȩ��";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // LogIn, UserName(16), Password(15)
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "LogIn";
        oCmdInfo.m_strDesc = "�û���¼";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "UserName(16)";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Password(15)";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // LogOut
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "LogOut";
        oCmdInfo.m_strDesc = "�û��˳�";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setUserAccessLevel, UserID, UserName(16), Password(15), UserLevel 
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setUserAccessLevel";
        oCmdInfo.m_strDesc = "�����û����������Լ�Ȩ��";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "UserID";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+User1...User8");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "UserName(16)";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "Password(15)";
        oParam2.m_iDataDefaultIndex = 0;

        // ��������
        oParam2.m_Data_vector.push_back("");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ����
        st_ParamInfo oParam3;
        oParam3.m_iIndex = iParamIndex++;
        oParam3.m_strParamName = "UserLevel";
        oParam3.m_iDataDefaultIndex = 1;

        // ��������
        oParam3.m_Data_vector.push_back("Viewer");
        oParam3.m_Data_vector.push_back("User");
        oCmdInfo.m_Param_vector.push_back(oParam3);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getUserAccessLevel, UserID
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getUserAccessLevel";
        oCmdInfo.m_strDesc = "�г��û���Ϣ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "UserID";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+User1...User8");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // 3.3 Tracking Configuration Commands =====================================
    // setAGCMode, Band, Mode, Gain
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setAGCMode";
        oCmdInfo.m_strDesc = "���ý��ջ�ǰ�˵��Զ�����ģʽ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Band";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+L1");
        oParam0.m_Data_vector.push_back("+L2L5");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Mode";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("auto");
        oParam1.m_Data_vector.push_back("frozen");
        oParam1.m_Data_vector.push_back("manual");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "Gain";
        oParam2.m_iDataDefaultIndex = 0;

        // ��������
        oParam2.m_Data_vector.push_back("0...35..70 dB");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getAGCMode, Band
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getAGCMode";
        oCmdInfo.m_strDesc = "�г����ջ�ǰ�˵�����ģʽ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Band";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+L1");
        oParam0.m_Data_vector.push_back("+L2L5");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setChannelAllocation, Channel, Satellite, Search, Doppler, Window
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setChannelAllocation";
        oCmdInfo.m_strDesc = "�������Ǻ�ͨ���Ĺ�ϵ����Ϣ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Channel";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+Ch01...Ch40");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Satellite";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("auto");
        oParam1.m_Data_vector.push_back("G01...G32");
        oParam1.m_Data_vector.push_back("F01...F21");
        oParam1.m_Data_vector.push_back("E01...E32");
        oParam1.m_Data_vector.push_back("S120...S140");
        oParam1.m_Data_vector.push_back("C01...C32");
        oParam1.m_Data_vector.push_back("J01");
        oParam1.m_Data_vector.push_back("J02");
        oParam1.m_Data_vector.push_back("J03");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "Search";
        oParam2.m_iDataDefaultIndex = 0;

        // ��������
        oParam2.m_Data_vector.push_back("auto");
        oParam2.m_Data_vector.push_back("manual");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ����
        st_ParamInfo oParam3;
        oParam3.m_iIndex = iParamIndex++;
        oParam3.m_strParamName = "Doppler";
        oParam3.m_iDataDefaultIndex = 0;

        // ��������
        oParam3.m_Data_vector.push_back("-50000...0...50000 Hz");
        oCmdInfo.m_Param_vector.push_back(oParam3);

        // ����
        st_ParamInfo oParam4;
        oParam4.m_iIndex = iParamIndex++;
        oParam4.m_strParamName = "Window";
        oParam4.m_iDataDefaultIndex = 0;

        // ��������
        oParam4.m_Data_vector.push_back("1...16000...100000 Hz");
        oCmdInfo.m_Param_vector.push_back(oParam4);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getChannelAllocation
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getChannelAllocation";
        oCmdInfo.m_strDesc = "�г����Ǻ�ͨ���Ĺ�ϵ����Ϣ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Channel";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+Ch01...Ch40");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getChannelConfiguration, Channel
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getChannelConfiguration";
        oCmdInfo.m_strDesc = "�г�ͨ�����ٵ��ź�";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Channel";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+Ch01...Ch40");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setCN0Mask, Signal, Mask
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setCN0Mask";
        oCmdInfo.m_strDesc = "�����źŵ�����ȷ�ֵ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Signal";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+GPSL1CA");
        oParam0.m_Data_vector.push_back("+Reserved1");
        oParam0.m_Data_vector.push_back("+Reserved2");
        oParam0.m_Data_vector.push_back("+GPSL2C");
        oParam0.m_Data_vector.push_back("+GPSL5");
        oParam0.m_Data_vector.push_back("+GLOL1CA");
        oParam0.m_Data_vector.push_back("+GLOL2P");
        oParam0.m_Data_vector.push_back("+GLOL2CA");
        oParam0.m_Data_vector.push_back("+GLOL3");
        oParam0.m_Data_vector.push_back("+GALL1BC");
        oParam0.m_Data_vector.push_back("+GALE5a");
        oParam0.m_Data_vector.push_back("+GALE5b");
        oParam0.m_Data_vector.push_back("+GALE5");
        oParam0.m_Data_vector.push_back("+GEOL1");
        oParam0.m_Data_vector.push_back("+GEOL5");
        oParam0.m_Data_vector.push_back("+CMPL1");
        oParam0.m_Data_vector.push_back("+CMPE5b");
        oParam0.m_Data_vector.push_back("+QZSL1CA");
        oParam0.m_Data_vector.push_back("+QZSL2C");
        oParam0.m_Data_vector.push_back("+QZSL5");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Mask";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("0...10...60 dB-Hz");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getCN0Mask, Signal
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getCN0Mask";
        oCmdInfo.m_strDesc = "�г��źŵ�����ȷ�ֵ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Signal";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+GPSL1CA");
        oParam0.m_Data_vector.push_back("+Reserved1");
        oParam0.m_Data_vector.push_back("+Reserved2");
        oParam0.m_Data_vector.push_back("+GPSL2C");
        oParam0.m_Data_vector.push_back("+GPSL5");
        oParam0.m_Data_vector.push_back("+GLOL1CA");
        oParam0.m_Data_vector.push_back("+GLOL2P");
        oParam0.m_Data_vector.push_back("+GLOL2CA");
        oParam0.m_Data_vector.push_back("+GLOL3");
        oParam0.m_Data_vector.push_back("+GALL1BC");
        oParam0.m_Data_vector.push_back("+GALE5a");
        oParam0.m_Data_vector.push_back("+GALE5b");
        oParam0.m_Data_vector.push_back("+GALE5");
        oParam0.m_Data_vector.push_back("+GEOL1");
        oParam0.m_Data_vector.push_back("+GEOL5");
        oParam0.m_Data_vector.push_back("+CMPL1");
        oParam0.m_Data_vector.push_back("+CMPE5b");
        oParam0.m_Data_vector.push_back("+QZSL1CA");
        oParam0.m_Data_vector.push_back("+QZSL2C");
        oParam0.m_Data_vector.push_back("+QZSL5");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setMultipathMitigation, Code, Carrier
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setMultipathMitigation";
        oCmdInfo.m_strDesc = "���ö�·��ЧӦ����";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Code";
        oParam0.m_iDataDefaultIndex = 1;

        // ��������
        oParam0.m_Data_vector.push_back("off");
        oParam0.m_Data_vector.push_back("on");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Carrier";
        oParam1.m_iDataDefaultIndex = 1;

        // ��������
        oParam1.m_Data_vector.push_back("off");
        oParam1.m_Data_vector.push_back("on");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getMultipathMitigation
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getMultipathMitigation";
        oCmdInfo.m_strDesc = "�г���·��ЧӦ����";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setSatelliteTracking, Satellite
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setSatelliteTracking";
        oCmdInfo.m_strDesc = "����Ҫ���ٵ�����";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Satellite";
        oParam0.m_iDataDefaultIndex = 1;

        // ��������
        oParam0.m_Data_vector.push_back("none");
        oParam0.m_Data_vector.push_back("+G01...G32");
        oParam0.m_Data_vector.push_back("+R01...R24");
        oParam0.m_Data_vector.push_back("+E01...E32");
        oParam0.m_Data_vector.push_back("+S120...S140");
        oParam0.m_Data_vector.push_back("+C01...C32");
        oParam0.m_Data_vector.push_back("+J01");
        oParam0.m_Data_vector.push_back("+J02");
        oParam0.m_Data_vector.push_back("+J03");
        oParam0.m_Data_vector.push_back("+GPS");
        oParam0.m_Data_vector.push_back("+GLONASS");
        oParam0.m_Data_vector.push_back("+GALILEO");
        oParam0.m_Data_vector.push_back("+SBAS");
        oParam0.m_Data_vector.push_back("+COMPASS");
        oParam0.m_Data_vector.push_back("+QZSS");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getSatelliteTracking
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getSatelliteTracking";
        oCmdInfo.m_strDesc = "�г����ٵ�����";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setSignalTracking, Signal
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setSignalTracking";
        oCmdInfo.m_strDesc = "����Ҫ���ٵ��ź�";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Signal";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+GPSL1CA");
        oParam0.m_Data_vector.push_back("+GPSL1PY");
        oParam0.m_Data_vector.push_back("+GPSL2PY");
        oParam0.m_Data_vector.push_back("+GPSL2C");
        oParam0.m_Data_vector.push_back("+GPSL5");
        oParam0.m_Data_vector.push_back("+GLOL1CA");
        oParam0.m_Data_vector.push_back("+GLOL2P");
        oParam0.m_Data_vector.push_back("+GLOL2CA");
        oParam0.m_Data_vector.push_back("+GLOL3");
        oParam0.m_Data_vector.push_back("+GALL1BC");
        oParam0.m_Data_vector.push_back("+GALE5a");
        oParam0.m_Data_vector.push_back("+GALE5b");
        oParam0.m_Data_vector.push_back("+GALE5");
        oParam0.m_Data_vector.push_back("+GEOL1");
        oParam0.m_Data_vector.push_back("+GEOL5");
        oParam0.m_Data_vector.push_back("+CMPL1");
        oParam0.m_Data_vector.push_back("+CMPE5b");
        oParam0.m_Data_vector.push_back("+QZSL1CA");
        oParam0.m_Data_vector.push_back("+QZSL2C");
        oParam0.m_Data_vector.push_back("+QZSL5");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getSignalTracking
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getSignalTracking";
        oCmdInfo.m_strDesc = "�г����ٵ��ź�";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setSmoothingInterval, Signal, Interval, Alignment
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setSmoothingInterval";
        oCmdInfo.m_strDesc = "�����ز���λƽ��α�������ʱ�������ź�";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Signal";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+GPSL1CA");
        oParam0.m_Data_vector.push_back("+GPSL2PY");
        oParam0.m_Data_vector.push_back("+GPSL2C");
        oParam0.m_Data_vector.push_back("+GPSL5");
        oParam0.m_Data_vector.push_back("+GLOL1CA");
        oParam0.m_Data_vector.push_back("+GLOL2P");
        oParam0.m_Data_vector.push_back("+GLOL2CA");
        oParam0.m_Data_vector.push_back("+GLOL3");
        oParam0.m_Data_vector.push_back("+GALL1BC");
        oParam0.m_Data_vector.push_back("+GALE5a");
        oParam0.m_Data_vector.push_back("+GALE5b");
        oParam0.m_Data_vector.push_back("+GALE5");
        oParam0.m_Data_vector.push_back("+GEOL1");
        oParam0.m_Data_vector.push_back("+GEOL5");
        oParam0.m_Data_vector.push_back("+CMPL1");
        oParam0.m_Data_vector.push_back("+CMPE5b");
        oParam0.m_Data_vector.push_back("+QZSL1CA");
        oParam0.m_Data_vector.push_back("+QZSL2C");
        oParam0.m_Data_vector.push_back("+QZSL5");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Interval";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("0...1000 sec");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "Alignment";
        oParam2.m_iDataDefaultIndex = 0;

        // ��������
        oParam2.m_Data_vector.push_back("0...1000 sec");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getSmoothingInterval, Signal
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getSmoothingInterval";
        oCmdInfo.m_strDesc = "�г��ز���λƽ��α�������ʱ�������ź�";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Signal";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+GPSL1CA");
        oParam0.m_Data_vector.push_back("+GPSL2PY");
        oParam0.m_Data_vector.push_back("+GPSL2C");
        oParam0.m_Data_vector.push_back("+GPSL5");
        oParam0.m_Data_vector.push_back("+GLOL1CA");
        oParam0.m_Data_vector.push_back("+GLOL2P");
        oParam0.m_Data_vector.push_back("+GLOL2CA");
        oParam0.m_Data_vector.push_back("+GLOL3");
        oParam0.m_Data_vector.push_back("+GALL1BC");
        oParam0.m_Data_vector.push_back("+GALE5a");
        oParam0.m_Data_vector.push_back("+GALE5b");
        oParam0.m_Data_vector.push_back("+GALE5");
        oParam0.m_Data_vector.push_back("+GEOL1");
        oParam0.m_Data_vector.push_back("+GEOL5");
        oParam0.m_Data_vector.push_back("+CMPL1");
        oParam0.m_Data_vector.push_back("+CMPE5b");
        oParam0.m_Data_vector.push_back("+QZSL1CA");
        oParam0.m_Data_vector.push_back("+QZSL2C");
        oParam0.m_Data_vector.push_back("+QZSL5");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // 3.4 Navigation Configuration Commands ===================================
    // setAntennaLocation, Antenna, Mode, DeltaX, DelatY, DeltaZ
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setAntennaLocation";
        oCmdInfo.m_strDesc = "��������λ��";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Antenna";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+Base");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Mode";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("auto");
        oParam1.m_Data_vector.push_back("manual");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "DeltaX";
        oParam2.m_iDataDefaultIndex = 0;

        // ��������
        oParam2.m_Data_vector.push_back("-1000.0000...0.0000...1000.0000 m");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ����
        st_ParamInfo oParam3;
        oParam3.m_iIndex = iParamIndex++;
        oParam3.m_strParamName = "DeltaY";
        oParam3.m_iDataDefaultIndex = 0;

        // ��������
        oParam3.m_Data_vector.push_back("-1000.0000...0.0000...1000.0000 m");
        oCmdInfo.m_Param_vector.push_back(oParam3);

        // ����
        st_ParamInfo oParam4;
        oParam4.m_iIndex = iParamIndex++;
        oParam4.m_strParamName = "DeltaZ";
        oParam4.m_iDataDefaultIndex = 0;

        // ��������
        oParam4.m_Data_vector.push_back("-1000.0000...0.0000...1000.0000 m");
        oCmdInfo.m_Param_vector.push_back(oParam4);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getAntennaLocation, Antenna
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getAntennaLocation";
        oCmdInfo.m_strDesc = "�г����ߵ�λ��";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Antenna";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+Base");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setElevationMask, Engine, Mask
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setElevationMask";
        oCmdInfo.m_strDesc = "��������������Ϣ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Engine";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+Tracking");
        oParam0.m_Data_vector.push_back("+PVT");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Mask";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("-90...0...90 deg");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getElevationMask, Engine
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getElevationMask";
        oCmdInfo.m_strDesc = "�г��������ǵ���Ϣ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Engine";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+Tracking");
        oParam0.m_Data_vector.push_back("+PVT");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setGeodeticDatum, Datum
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setGeodeticDatum";
        oCmdInfo.m_strDesc = "���ô������ϵ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Datum";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("WGS84");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getGeodeticDatum
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getGeodeticDatum";
        oCmdInfo.m_strDesc = "�г��������ϵ";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setHealthMask, Engine, Mask
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setHealthMask";
        oCmdInfo.m_strDesc = "�������ǽ���������Ϣ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Engine";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+Tracking");
        oParam0.m_Data_vector.push_back("+PVT");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Mask";
        oParam1.m_iDataDefaultIndex = 1;

        // ��������
        oParam1.m_Data_vector.push_back("off");
        oParam1.m_Data_vector.push_back("on");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getHealthMask, Engine
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getHealthMask";
        oCmdInfo.m_strDesc = "�г����ǽ������˵���Ϣ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Engine";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+Tracking");
        oParam0.m_Data_vector.push_back("+PVT");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setIonosphereModel, Model
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setIonosphereModel";
        oCmdInfo.m_strDesc = "���õ����ģ�͵�ģʽ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Model";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("auto");
        oParam0.m_Data_vector.push_back("off");
        oParam0.m_Data_vector.push_back("Klobuchar");
        oParam0.m_Data_vector.push_back("SBAS");
        oParam0.m_Data_vector.push_back("MultiFreq");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getIonosphereModel
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getIonosphereModel";
        oCmdInfo.m_strDesc = "�г������ģ�͵�ģʽ";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setMagneticVariance, Mode, Variance
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setMagneticVariance";
        oCmdInfo.m_strDesc = "�����Ŵ������ģʽ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Mode";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("auto");
        oParam0.m_Data_vector.push_back("manual");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Variance";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("-180.0...0.0...180.0 deg");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getMagneticVariance
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getMagneticVariance";
        oCmdInfo.m_strDesc = "�г��Ŵ������ģʽ";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setPVTMode, Mode, RoverMode, StaticPosition, ExtSensorIntegration
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setPVTMode";
        oCmdInfo.m_strDesc = "���ý��ջ�PVT��ģʽ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Mode";
        oParam0.m_iDataDefaultIndex = 1;

        // ��������
        oParam0.m_Data_vector.push_back("Static");
        oParam0.m_Data_vector.push_back("Rover");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "RoverMode";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("+StandAlone");
        oParam1.m_Data_vector.push_back("+SBAS");
        oParam1.m_Data_vector.push_back("+DGPS");
        oParam1.m_Data_vector.push_back("+RTKFloat");
        oParam1.m_Data_vector.push_back("+RTKFixed");
        oParam1.m_Data_vector.push_back("+PPP");
        oParam1.m_Data_vector.push_back("+RTK");
        oParam1.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "StaticPosition";
        oParam2.m_iDataDefaultIndex = 0;

        // ��������
        oParam2.m_Data_vector.push_back("auto");
        oParam2.m_Data_vector.push_back("Geodetic1");
        oParam2.m_Data_vector.push_back("Geodetic2");
        oParam2.m_Data_vector.push_back("Geodetic3");
        oParam2.m_Data_vector.push_back("Geodetic4");
        oParam2.m_Data_vector.push_back("Geodetic5");
        oParam2.m_Data_vector.push_back("Cartesian1");
        oParam2.m_Data_vector.push_back("Cartesian2");
        oParam2.m_Data_vector.push_back("Cartesian3");
        oParam2.m_Data_vector.push_back("Cartesian4");
        oParam2.m_Data_vector.push_back("Cartesian5");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ����
        st_ParamInfo oParam3;
        oParam3.m_iIndex = iParamIndex++;
        oParam3.m_strParamName = "ExtSensorIntegration";
        oParam3.m_iDataDefaultIndex = 0;

        // ��������
        oParam3.m_Data_vector.push_back("off");
        oParam3.m_Data_vector.push_back("SIGIL");
        oCmdInfo.m_Param_vector.push_back(oParam3);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getPVTMode
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getPVTMode";
        oCmdInfo.m_strDesc = "�г����ջ�PVT��ģʽ";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setRAIMLevels, Mode, Pfa, Pmd, Reliability
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setRAIMLevels";
        oCmdInfo.m_strDesc = "���ý��ջ���������Լ��";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Mode";
        oParam0.m_iDataDefaultIndex = 1;

        // ��������
        oParam0.m_Data_vector.push_back("off");
        oParam0.m_Data_vector.push_back("on");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Pfa";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("-12...-4...-1");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "Pmd";
        oParam2.m_iDataDefaultIndex = 0;

        // ��������
        oParam2.m_Data_vector.push_back("-12...-4...-1");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ����
        st_ParamInfo oParam3;
        oParam3.m_iIndex = iParamIndex++;
        oParam3.m_strParamName = "Reliability";
        oParam3.m_iDataDefaultIndex = 0;

        // ��������
        oParam3.m_Data_vector.push_back("-12...-3...-1");
        oCmdInfo.m_Param_vector.push_back(oParam3);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getRAIMLevels
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getRAIMLevels";
        oCmdInfo.m_strDesc = "�г����ջ�����������Լ����Ϣ";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setReceiverDynamics, Level, Motion
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setReceiverDynamics";
        oCmdInfo.m_strDesc = "���ý��ջ��Ĺ�����̬��";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Level";
        oParam0.m_iDataDefaultIndex = 2;

        // ��������
        oParam0.m_Data_vector.push_back("Max");
        oParam0.m_Data_vector.push_back("High");
        oParam0.m_Data_vector.push_back("Moderate");
        oParam0.m_Data_vector.push_back("Low");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Motion";
        oParam1.m_iDataDefaultIndex = 3;

        // ��������
        oParam1.m_Data_vector.push_back("Static");
        oParam1.m_Data_vector.push_back("Quasistatic");
        oParam1.m_Data_vector.push_back("Pedestrian");
        oParam1.m_Data_vector.push_back("Automotive");
        oParam1.m_Data_vector.push_back("RaceCar");
        oParam1.m_Data_vector.push_back("HeavyMachinery");
        oParam1.m_Data_vector.push_back("UAV");
        oParam1.m_Data_vector.push_back("Unlimited");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getReceiverDynamics
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getReceiverDynamics";
        oCmdInfo.m_strDesc = "�г����ջ��Ĺ�����̬��";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setSatelliteUsage, Satellite
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setSatelliteUsage";
        oCmdInfo.m_strDesc = "���ñ�������PVT���������";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Satellite";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("none");
        oParam0.m_Data_vector.push_back("+G01...G32");
        oParam0.m_Data_vector.push_back("+R01...R24");
        oParam0.m_Data_vector.push_back("+E01...E32");
        oParam0.m_Data_vector.push_back("+S120...S140");
        oParam0.m_Data_vector.push_back("+C01...C32");
        oParam0.m_Data_vector.push_back("+GPS");
        oParam0.m_Data_vector.push_back("+GLONASS");
        oParam0.m_Data_vector.push_back("+GALILEO");
        oParam0.m_Data_vector.push_back("+SBAS");
        oParam0.m_Data_vector.push_back("+COMPASS");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getSatelliteUsage
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getSatelliteUsage";
        oCmdInfo.m_strDesc = "�г���������PVT���������";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setSBASCorrections, Satellite, SISMode, NavMode, DO229Version
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setSBASCorrections";
        oCmdInfo.m_strDesc = "���ù����ֵ���Ϣ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Satellite";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("auto");
        oParam0.m_Data_vector.push_back("EGNOS");
        oParam0.m_Data_vector.push_back("WAAS");
        oParam0.m_Data_vector.push_back("MSAS");
        oParam0.m_Data_vector.push_back("S120...S140");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "SISMode";
        oParam1.m_iDataDefaultIndex = 1;

        // ��������
        oParam1.m_Data_vector.push_back("Test");
        oParam1.m_Data_vector.push_back("Operational");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "NavMode";
        oParam2.m_iDataDefaultIndex = 0;

        // ��������
        oParam2.m_Data_vector.push_back("EnRoute");
        oParam2.m_Data_vector.push_back("PrecApp");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ����
        st_ParamInfo oParam3;
        oParam3.m_iIndex = iParamIndex++;
        oParam3.m_strParamName = "DO229Version";
        oParam3.m_iDataDefaultIndex = 0;

        // ��������
        oParam3.m_Data_vector.push_back("auto");
        oParam3.m_Data_vector.push_back("DO229C");
        oCmdInfo.m_Param_vector.push_back(oParam3);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getSBASCorrections
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getSBASCorrections";
        oCmdInfo.m_strDesc = "�г������ֵ���Ϣ";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setSignalUsage, Signal, NavData
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setSignalUsage";
        oCmdInfo.m_strDesc = "���ñ�������PVT������ź�";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Signal";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+GPSL1CA");
        oParam0.m_Data_vector.push_back("+GPSL1PY");
        oParam0.m_Data_vector.push_back("+GPSL2PY");
        oParam0.m_Data_vector.push_back("+GPSL2C");
        oParam0.m_Data_vector.push_back("+GPSL5");
        oParam0.m_Data_vector.push_back("+GLOL1CA");
        oParam0.m_Data_vector.push_back("+CLOL2P");
        oParam0.m_Data_vector.push_back("+GLOL2CA");
        oParam0.m_Data_vector.push_back("+GLOL3");
        oParam0.m_Data_vector.push_back("+GALL1BC");
        oParam0.m_Data_vector.push_back("+GALE5a");
        oParam0.m_Data_vector.push_back("+GALE5b");
        oParam0.m_Data_vector.push_back("+GALE5");
        oParam0.m_Data_vector.push_back("+GEOL1");
        oParam0.m_Data_vector.push_back("+GEOL5");
        oParam0.m_Data_vector.push_back("+CMPL1");
        oParam0.m_Data_vector.push_back("+CMPE5b");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "NavData";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("+GPSL1CA");
        oParam1.m_Data_vector.push_back("+GPSL1PY");
        oParam1.m_Data_vector.push_back("+GPSL2PY");
        oParam1.m_Data_vector.push_back("+GPSL2C");
        oParam1.m_Data_vector.push_back("+GPSL5");
        oParam1.m_Data_vector.push_back("+GLOL1CA");
        oParam1.m_Data_vector.push_back("+CLOL2P");
        oParam1.m_Data_vector.push_back("+GLOL2CA");
        oParam1.m_Data_vector.push_back("+GLOL3");
        oParam1.m_Data_vector.push_back("+GALL1BC");
        oParam1.m_Data_vector.push_back("+GALE5a");
        oParam1.m_Data_vector.push_back("+GALE5b");
        oParam1.m_Data_vector.push_back("+GALE5");
        oParam1.m_Data_vector.push_back("+GEOL1");
        oParam1.m_Data_vector.push_back("+GEOL5");
        oParam1.m_Data_vector.push_back("+CMPL1");
        oParam1.m_Data_vector.push_back("+CMPE5b");
        oParam1.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getSignalUsage
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getSignalUsage";
        oCmdInfo.m_strDesc = "�г���������PVT������ź�";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setStaticPosCartesian, Position, X, Y, Z, Datum
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setStaticPosCartesian";
        oCmdInfo.m_strDesc = "���þ�̬λ��";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Position";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+Cartesian1");
        oParam0.m_Data_vector.push_back("+Cartesian2");
        oParam0.m_Data_vector.push_back("+Cartesian3");
        oParam0.m_Data_vector.push_back("+Cartesian4");
        oParam0.m_Data_vector.push_back("+Cartesian5");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "X";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("-8000000.0000...0.0000...8000000.0000 m");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "Y";
        oParam2.m_iDataDefaultIndex = 0;

        // ��������
        oParam2.m_Data_vector.push_back("-8000000.0000...0.0000...8000000.0000 m");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ����
        st_ParamInfo oParam3;
        oParam3.m_iIndex = iParamIndex++;
        oParam3.m_strParamName = "Z";
        oParam3.m_iDataDefaultIndex = 0;

        // ��������
        oParam3.m_Data_vector.push_back("-8000000.0000...0.0000...8000000.0000 m");
        oCmdInfo.m_Param_vector.push_back(oParam3);

        // ����
        st_ParamInfo oParam4;
        oParam4.m_iIndex = iParamIndex++;
        oParam4.m_strParamName = "Datum";
        oParam4.m_iDataDefaultIndex = 0;

        // ��������
        oParam4.m_Data_vector.push_back("WGS84");
        oCmdInfo.m_Param_vector.push_back(oParam4);
        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getStaticPosCartesian, Position
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getStaticPosCartesian";
        oCmdInfo.m_strDesc = "�г���̬λ��";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Position";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+Cartesian1");
        oParam0.m_Data_vector.push_back("+Cartesian2");
        oParam0.m_Data_vector.push_back("+Cartesian3");
        oParam0.m_Data_vector.push_back("+Cartesian4");
        oParam0.m_Data_vector.push_back("+Cartesian5");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setStaticPosGeodetic, Position, Latitude, Longitude, Altitude, Datum
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setStaticPosGeodetic";
        oCmdInfo.m_strDesc = "���þ�̬��γ�ȸ߳�";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Position";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+Geodetic1");
        oParam0.m_Data_vector.push_back("+Geodetic2");
        oParam0.m_Data_vector.push_back("+Geodetic3");
        oParam0.m_Data_vector.push_back("+Geodetic4");
        oParam0.m_Data_vector.push_back("+Geodetic5");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Latitude";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("-90.000000000...0.000000000.90.000000000 deg");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "Longitude";
        oParam2.m_iDataDefaultIndex = 0;

        // ��������
        oParam2.m_Data_vector.push_back("-180.000000000...0.000000000...180.000000000 deg");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ����
        st_ParamInfo oParam3;
        oParam3.m_iIndex = iParamIndex++;
        oParam3.m_strParamName = "Altitude";
        oParam3.m_iDataDefaultIndex = 0;

        // ��������
        oParam3.m_Data_vector.push_back("-1000.0000...0.0000...30000.0000 m");
        oCmdInfo.m_Param_vector.push_back(oParam3);

        // ����
        st_ParamInfo oParam4;
        oParam4.m_iIndex = iParamIndex++;
        oParam4.m_strParamName = "Datum";
        oParam4.m_iDataDefaultIndex = 0;

        // ��������
        oParam4.m_Data_vector.push_back("WGS84");
        oCmdInfo.m_Param_vector.push_back(oParam4);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getStaticPosGeodetic, Position
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getStaticPosGeodetic";
        oCmdInfo.m_strDesc = "�г���̬��γ�ȸ߳�";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Position";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+Geodetic1");
        oParam0.m_Data_vector.push_back("+Geodetic2");
        oParam0.m_Data_vector.push_back("+Geodetic3");
        oParam0.m_Data_vector.push_back("+Geodetic4");
        oParam0.m_Data_vector.push_back("+Geodetic5");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setTroposphereModel, ZenithModel, MappingModel
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setTroposphereModel";
        oCmdInfo.m_strDesc = "���ö�����ģ��";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "ZenithModel";
        oParam0.m_iDataDefaultIndex = 2;

        // ��������
        oParam0.m_Data_vector.push_back("off");
        oParam0.m_Data_vector.push_back("Saastamoinen");
        oParam0.m_Data_vector.push_back("MOPS");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "MappingModel";
        oParam1.m_iDataDefaultIndex = 1;

        // ��������
        oParam1.m_Data_vector.push_back("Niell");
        oParam1.m_Data_vector.push_back("MOPS");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getTroposphereModel
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getTroposphereModel";
        oCmdInfo.m_strDesc = "�г�������ģ��";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setTroposphereParameters, Temperature, Pressure, Humidity
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setTroposphereParameters";
        oCmdInfo.m_strDesc = "���ö��������";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Temperature";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("-100.0...15.0...100.0 degC");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Pressure";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("800.00...1013.25...1500.00 hPa");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "Humidity";
        oParam2.m_iDataDefaultIndex = 0;

        // ��������
        oParam2.m_Data_vector.push_back("0...50...100 %");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getTroposphereParameters
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getTroposphereParameters";
        oCmdInfo.m_strDesc = "�г����������";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // 3.5 Receiver Operation Commands =========================================
    // setEventParameters, Event, Polarity
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setEventParameters";
        oCmdInfo.m_strDesc = "�����¼�����";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Event";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+EventA");
        oParam0.m_Data_vector.push_back("+EventB");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Polarity";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("Low2High");
        oParam1.m_Data_vector.push_back("High2Low");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getEventParameters, Event
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getEventParameters";
        oCmdInfo.m_strDesc = "�г��¼�����";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Event";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+EventA");
        oParam0.m_Data_vector.push_back("+EventB");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setPPSParameters, Interval, Polarity, Delay, TimeScale, MaxSyncAge
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setPPSParameters";
        oCmdInfo.m_strDesc = "����PPS����";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Interval";
        oParam0.m_iDataDefaultIndex = 4;

        // ��������
        oParam0.m_Data_vector.push_back("off");
        oParam0.m_Data_vector.push_back("msec100");
        oParam0.m_Data_vector.push_back("msec200");
        oParam0.m_Data_vector.push_back("msec500");
        oParam0.m_Data_vector.push_back("sec1");
        oParam0.m_Data_vector.push_back("sec2");
        oParam0.m_Data_vector.push_back("sec5");
        oParam0.m_Data_vector.push_back("sec10");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Polarity";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("Low2High");
        oParam1.m_Data_vector.push_back("High2Low");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "Delay";
        oParam2.m_iDataDefaultIndex = 0;

        // ��������
        oParam2.m_Data_vector.push_back("-1000000.00...0.00...1000000.00 nsec");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ����
        st_ParamInfo oParam3;
        oParam3.m_iIndex = iParamIndex++;
        oParam3.m_strParamName = "TimeScale";
        oParam3.m_iDataDefaultIndex = 0;

        // ��������
        oParam3.m_Data_vector.push_back("TimeSys");
        oParam3.m_Data_vector.push_back("UTC");
        oParam3.m_Data_vector.push_back("RxClock");
        oParam3.m_Data_vector.push_back("GLONASS");
        oCmdInfo.m_Param_vector.push_back(oParam3);

        // ����
        st_ParamInfo oParam4;
        oParam4.m_iIndex = iParamIndex++;
        oParam4.m_strParamName = "MaxSyncAge";
        oParam4.m_iDataDefaultIndex = 0;

        // ��������
        oParam4.m_Data_vector.push_back("1...60...3600 sec");
        oCmdInfo.m_Param_vector.push_back(oParam4);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getPPSParameters
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getPPSParameters";
        oCmdInfo.m_strDesc = "�г�PPS����";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }


    // 3.6 Session Settings Commands ===========================================


    // 3.7 Input/Output Commands ===============================================
    // setCOMSettings, Cd, Rate, DataBits, Parity, StopBits, FlowControl
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setCOMSettings";
        oCmdInfo.m_strDesc = "���ô��ڲ���";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Cd";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+COM1");
        oParam0.m_Data_vector.push_back("+COM2");
        oParam0.m_Data_vector.push_back("+COM3");
        oParam0.m_Data_vector.push_back("+COM4");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Rate";
        oParam1.m_iDataDefaultIndex = 7;

        // ��������
        oParam1.m_Data_vector.push_back("baud1200");
        oParam1.m_Data_vector.push_back("baud2400");
        oParam1.m_Data_vector.push_back("baud4800");
        oParam1.m_Data_vector.push_back("baud9600");
        oParam1.m_Data_vector.push_back("baud19200");
        oParam1.m_Data_vector.push_back("baud38400");
        oParam1.m_Data_vector.push_back("baud57600");
        oParam1.m_Data_vector.push_back("baud115200");
        oParam1.m_Data_vector.push_back("baud230400");
        oParam1.m_Data_vector.push_back("baud460800");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "DataBits";
        oParam2.m_iDataDefaultIndex = 0;

        // ��������
        oParam2.m_Data_vector.push_back("bits8");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ����
        st_ParamInfo oParam3;
        oParam3.m_iIndex = iParamIndex++;
        oParam3.m_strParamName = "Parity";
        oParam3.m_iDataDefaultIndex = 0;

        // ��������
        oParam3.m_Data_vector.push_back("No");
        oCmdInfo.m_Param_vector.push_back(oParam3);

        // ����
        st_ParamInfo oParam4;
        oParam4.m_iIndex = iParamIndex++;
        oParam4.m_strParamName = "StopBits";
        oParam4.m_iDataDefaultIndex = 0;

        // ��������
        oParam4.m_Data_vector.push_back("bit1");
        oCmdInfo.m_Param_vector.push_back(oParam4);

        // ����
        st_ParamInfo oParam5;
        oParam5.m_iIndex = iParamIndex++;
        oParam5.m_strParamName = "FlowControl";
        oParam5.m_iDataDefaultIndex = 0;

        // ��������
        oParam5.m_Data_vector.push_back("none");
        oParam5.m_Data_vector.push_back("RTS|CTS");
        oCmdInfo.m_Param_vector.push_back(oParam5);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getCOMSettings
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getCOMSettings";
        oCmdInfo.m_strDesc = "�г����ڲ���";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Cd";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+COM1");
        oParam0.m_Data_vector.push_back("+COM2");
        oParam0.m_Data_vector.push_back("+COM3");
        oParam0.m_Data_vector.push_back("+COM4");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setDataInOut, Cd, Input, Output, (Show)
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setDataInOut";
        oCmdInfo.m_strDesc = "���������������";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Cd";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+DSK1");
        oParam0.m_Data_vector.push_back("+COM1");
        oParam0.m_Data_vector.push_back("+COM2");
        oParam0.m_Data_vector.push_back("+COM3");
        oParam0.m_Data_vector.push_back("+COM4");
        oParam0.m_Data_vector.push_back("+USB1");
        oParam0.m_Data_vector.push_back("+USB2");
        oParam0.m_Data_vector.push_back("+IP10...IP17");
        oParam0.m_Data_vector.push_back("+NTR1");
        oParam0.m_Data_vector.push_back("+NTR2");
        oParam0.m_Data_vector.push_back("+NTR3");
        oParam0.m_Data_vector.push_back("+IPS1");
        oParam0.m_Data_vector.push_back("+IPS2");
        oParam0.m_Data_vector.push_back("+IPS3");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Input";
        oParam1.m_iDataDefaultIndex = 9;

        // ��������
        oParam1.m_Data_vector.push_back("none");
        oParam1.m_Data_vector.push_back("CMD");
        oParam1.m_Data_vector.push_back("RTCMv2");
        oParam1.m_Data_vector.push_back("RTCMv3");
        oParam1.m_Data_vector.push_back("CMRv2");
        oParam1.m_Data_vector.push_back("DC1");
        oParam1.m_Data_vector.push_back("DC2");
        oParam1.m_Data_vector.push_back("RTCMV");
        oParam1.m_Data_vector.push_back("ASCIIIN");
        oParam1.m_Data_vector.push_back("auto");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "Output";
        oParam2.m_iDataDefaultIndex = 4;

        // ��������
        oParam2.m_Data_vector.push_back("none");
        oParam2.m_Data_vector.push_back("+RTCMv2");
        oParam2.m_Data_vector.push_back("+RTCMv3");
        oParam2.m_Data_vector.push_back("+CMRv2");
        oParam2.m_Data_vector.push_back("+SBF");
        oParam2.m_Data_vector.push_back("+NMEA");
        oParam2.m_Data_vector.push_back("+ASCIIDisplay");
        oParam2.m_Data_vector.push_back("+DC1");
        oParam2.m_Data_vector.push_back("+DC2");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ����
        st_ParamInfo oParam3;
        oParam3.m_iIndex = iParamIndex++;
        oParam3.m_strParamName = "(Show)";
        oParam3.m_iDataDefaultIndex = 0;

        // ��������
        oParam3.m_Data_vector.push_back("(off)");
        oParam3.m_Data_vector.push_back("(on)");
        oCmdInfo.m_Param_vector.push_back(oParam3);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getDataInOut, Cd
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getDataInOut";
        oCmdInfo.m_strDesc = "�г������������";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Cd";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+DSK1");
        oParam0.m_Data_vector.push_back("+COM1");
        oParam0.m_Data_vector.push_back("+COM2");
        oParam0.m_Data_vector.push_back("+COM3");
        oParam0.m_Data_vector.push_back("+COM4");
        oParam0.m_Data_vector.push_back("+USB1");
        oParam0.m_Data_vector.push_back("+USB2");
        oParam0.m_Data_vector.push_back("+IP10...IP17");
        oParam0.m_Data_vector.push_back("+NTR1");
        oParam0.m_Data_vector.push_back("+NTR2");
        oParam0.m_Data_vector.push_back("+NTR3");
        oParam0.m_Data_vector.push_back("+IPS1");
        oParam0.m_Data_vector.push_back("+IPS2");
        oParam0.m_Data_vector.push_back("+IPS3");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // exeEchoMessage, Cd, Message(242), EndOfLine
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "exeEchoMessage";
        oCmdInfo.m_strDesc = "���ö˿ڻ���";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Cd";
        oParam0.m_iDataDefaultIndex = 1;

        // ��������
        oParam0.m_Data_vector.push_back("DSK1");
        oParam0.m_Data_vector.push_back("COM1");
        oParam0.m_Data_vector.push_back("COM2");
        oParam0.m_Data_vector.push_back("COM3");
        oParam0.m_Data_vector.push_back("COM4");
        oParam0.m_Data_vector.push_back("USB1");
        oParam0.m_Data_vector.push_back("USB2");
        oParam0.m_Data_vector.push_back("IP10...IP17");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Message(242)";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("A:Unknown");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "EndOfLine";
        oParam2.m_iDataDefaultIndex = 0;

        // ��������
        oParam2.m_Data_vector.push_back("none");
        oParam2.m_Data_vector.push_back("+CR");
        oParam2.m_Data_vector.push_back("+LF");
        oParam2.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getEchoMessage
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getEchoMessage";
        oCmdInfo.m_strDesc = "�г��˿ڻ���";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setIPPortSettings, Command
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setIPPortSettings";
        oCmdInfo.m_strDesc = "����IP�˿�";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Command";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("1...28784...65535");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getIPPortSettings
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getIPPortSettings";
        oCmdInfo.m_strDesc = "�г�IP�˿�";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setIPServerSettings, Cd, Port
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setIPServerSettings";
        oCmdInfo.m_strDesc = "���ý��ջ�IP����˿�";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Cd";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+IPS1");
        oParam0.m_Data_vector.push_back("+IPS2");
        oParam0.m_Data_vector.push_back("+IPS3");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Port";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("0...65535");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getIPServerSettings, Cd
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getIPServerSettings";
        oCmdInfo.m_strDesc = "�г����ջ�IP����˿�";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Cd";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+IPS1");
        oParam0.m_Data_vector.push_back("+IPS2");
        oParam0.m_Data_vector.push_back("+IPS3");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setIPSettings, Mode, IP(16), Netmask(16), Gateway(16), Domain(63), DNS1(16), DNS2(16)
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setIPSettings";
        oCmdInfo.m_strDesc = "����IP����";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Mode";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("DHCP");
        oParam0.m_Data_vector.push_back("Static");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "IP(16)";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("192.168.2.2");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "Netmask(16)";
        oParam2.m_iDataDefaultIndex = 0;

        // ��������
        oParam2.m_Data_vector.push_back("255.255.255.0");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ����
        st_ParamInfo oParam3;
        oParam3.m_iIndex = iParamIndex++;
        oParam3.m_strParamName = "Gateway(16)";
        oParam3.m_iDataDefaultIndex = 0;

        // ��������
        oParam3.m_Data_vector.push_back("192.168.2.1");
        oCmdInfo.m_Param_vector.push_back(oParam3);

        // ����
        st_ParamInfo oParam4;
        oParam4.m_iIndex = iParamIndex++;
        oParam4.m_strParamName = "Domain(63)";
        oParam4.m_iDataDefaultIndex = 0;

        // ��������
        oParam4.m_Data_vector.push_back("");
        oCmdInfo.m_Param_vector.push_back(oParam4);

        // ����
        st_ParamInfo oParam5;
        oParam5.m_iIndex = iParamIndex++;
        oParam5.m_strParamName = "DNS1(16)";
        oParam5.m_iDataDefaultIndex = 0;

        // ��������
        oParam5.m_Data_vector.push_back("8.8.8.8");
        oCmdInfo.m_Param_vector.push_back(oParam5);

        // ����
        st_ParamInfo oParam6;
        oParam6.m_iIndex = iParamIndex++;
        oParam6.m_strParamName = "DNS2(16)";
        oParam6.m_iDataDefaultIndex = 0;

        // ��������
        oParam6.m_Data_vector.push_back("8.8.4.4");
        oCmdInfo.m_Param_vector.push_back(oParam6);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getIPSettings
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getIPSettings";
        oCmdInfo.m_strDesc = "�г�IP����";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // exeNMEAOnce, Cd, Messages
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "exeNMEAOnce";
        oCmdInfo.m_strDesc = "ִ��һ��NMEA��Ϣ��ȡ";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Cd";
        oParam0.m_iDataDefaultIndex = 1;

        // ��������
        oParam0.m_Data_vector.push_back("DSK1");
        oParam0.m_Data_vector.push_back("COM1");
        oParam0.m_Data_vector.push_back("COM2");
        oParam0.m_Data_vector.push_back("COM3");
        oParam0.m_Data_vector.push_back("COM4");
        oParam0.m_Data_vector.push_back("USB1");
        oParam0.m_Data_vector.push_back("USB2");
        oParam0.m_Data_vector.push_back("IP10...IP17");
        oParam0.m_Data_vector.push_back("NTR1");
        oParam0.m_Data_vector.push_back("NTR2");
        oParam0.m_Data_vector.push_back("NTR3");
        oParam0.m_Data_vector.push_back("IPS1");
        oParam0.m_Data_vector.push_back("IPS2");
        oParam0.m_Data_vector.push_back("IPS3");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Messages";
        oParam1.m_iDataDefaultIndex = 5;

        // ��������
        oParam1.m_Data_vector.push_back("+ALM");
        oParam1.m_Data_vector.push_back("+DTM");
        oParam1.m_Data_vector.push_back("+GBS");
        oParam1.m_Data_vector.push_back("+GGA");
        oParam1.m_Data_vector.push_back("+GLL");
        oParam1.m_Data_vector.push_back("+GNS");
        oParam1.m_Data_vector.push_back("+GRS");
        oParam1.m_Data_vector.push_back("+GSA");
        oParam1.m_Data_vector.push_back("+GST");
        oParam1.m_Data_vector.push_back("+GSV");
        oParam1.m_Data_vector.push_back("+HDT");
        oParam1.m_Data_vector.push_back("+RMC");
        oParam1.m_Data_vector.push_back("+ROT");
        oParam1.m_Data_vector.push_back("+VTG");
        oParam1.m_Data_vector.push_back("+ZDA");
        oParam1.m_Data_vector.push_back("+HRP");
        oParam1.m_Data_vector.push_back("+LLQ");
        oParam1.m_Data_vector.push_back("+RBP");
        oParam1.m_Data_vector.push_back("+RBV");
        oParam1.m_Data_vector.push_back("+RBD");
        oParam1.m_Data_vector.push_back("+AVR");
        oParam1.m_Data_vector.push_back("+GGK");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getNMEAOnce
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getNMEAOnce";
        oCmdInfo.m_strDesc = "�г�һ��NMEA����";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setNMEAOutput, Stream, Cd, Messages, Interval
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setNMEAOutput";
        oCmdInfo.m_strDesc = "����NMEA�������";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Stream";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+Stream1...Stream10");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Cd";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("none");
        oParam1.m_Data_vector.push_back("DSK1");
        oParam1.m_Data_vector.push_back("COM1");
        oParam1.m_Data_vector.push_back("COM2");
        oParam1.m_Data_vector.push_back("COM3");
        oParam1.m_Data_vector.push_back("COM4");
        oParam1.m_Data_vector.push_back("USB1");
        oParam1.m_Data_vector.push_back("USB2");
        oParam1.m_Data_vector.push_back("IP10...IP17");
        oParam1.m_Data_vector.push_back("NTR1");
        oParam1.m_Data_vector.push_back("NTR2");
        oParam1.m_Data_vector.push_back("NTR3");
        oParam1.m_Data_vector.push_back("IPS1");
        oParam1.m_Data_vector.push_back("IPS2");
        oParam1.m_Data_vector.push_back("IPS3");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "Messages";
        oParam2.m_iDataDefaultIndex = 0;

        // ��������
        oParam2.m_Data_vector.push_back("none");
        oParam2.m_Data_vector.push_back("+ALM");
        oParam2.m_Data_vector.push_back("+DTM");
        oParam2.m_Data_vector.push_back("+GBS");
        oParam2.m_Data_vector.push_back("+GGA");
        oParam2.m_Data_vector.push_back("+GLL");
        oParam2.m_Data_vector.push_back("+GNS");
        oParam2.m_Data_vector.push_back("+GRS");
        oParam2.m_Data_vector.push_back("+GSA");
        oParam2.m_Data_vector.push_back("+GST");
        oParam2.m_Data_vector.push_back("+GSV");
        oParam2.m_Data_vector.push_back("+HDT");
        oParam2.m_Data_vector.push_back("+RMC");
        oParam2.m_Data_vector.push_back("+ROT");
        oParam2.m_Data_vector.push_back("+VTG");
        oParam2.m_Data_vector.push_back("+ZDA");
        oParam2.m_Data_vector.push_back("+HRP");
        oParam2.m_Data_vector.push_back("+LLQ");
        oParam2.m_Data_vector.push_back("+RBP");
        oParam2.m_Data_vector.push_back("+RBV");
        oParam2.m_Data_vector.push_back("+RBD");
        oParam2.m_Data_vector.push_back("+PUMRD");
        oParam2.m_Data_vector.push_back("+AVR");
        oParam2.m_Data_vector.push_back("+GGK");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ����
        st_ParamInfo oParam3;
        oParam3.m_iIndex = iParamIndex++;
        oParam3.m_strParamName = "Interval";
        oParam3.m_iDataDefaultIndex = 0;

        // ��������
        oParam3.m_Data_vector.push_back("off");
        oParam3.m_Data_vector.push_back("OnChange");
        oParam3.m_Data_vector.push_back("msec10");
        oParam3.m_Data_vector.push_back("msec20");
        oParam3.m_Data_vector.push_back("msec40");
        oParam3.m_Data_vector.push_back("msec50");
        oParam3.m_Data_vector.push_back("msec100");
        oParam3.m_Data_vector.push_back("msec200");
        oParam3.m_Data_vector.push_back("msec500");
        oParam3.m_Data_vector.push_back("sec1");
        oParam3.m_Data_vector.push_back("sec2");
        oParam3.m_Data_vector.push_back("sec5");
        oParam3.m_Data_vector.push_back("sec10");
        oParam3.m_Data_vector.push_back("sec15");
        oParam3.m_Data_vector.push_back("sec30");
        oParam3.m_Data_vector.push_back("sec60");
        oParam3.m_Data_vector.push_back("min2");
        oParam3.m_Data_vector.push_back("min5");
        oParam3.m_Data_vector.push_back("min10");
        oParam3.m_Data_vector.push_back("min15");
        oParam3.m_Data_vector.push_back("min30");
        oParam3.m_Data_vector.push_back("min60");
        oCmdInfo.m_Param_vector.push_back(oParam3);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getNMEAOutput
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getNMEAOutput";
        oCmdInfo.m_strDesc = "�г�NMEA�������";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Stream";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+Stream1...Stream10");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // exeSBFOnce, Cd, Messages
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "exeSBFOnce";
        oCmdInfo.m_strDesc = "ִ��SBFһ�����";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Cd";
        oParam0.m_iDataDefaultIndex = 1;

        // ��������
        oParam0.m_Data_vector.push_back("DSK1");
        oParam0.m_Data_vector.push_back("COM1");
        oParam0.m_Data_vector.push_back("COM2");
        oParam0.m_Data_vector.push_back("COM3");
        oParam0.m_Data_vector.push_back("COM4");
        oParam0.m_Data_vector.push_back("USB1");
        oParam0.m_Data_vector.push_back("USB2");
        oParam0.m_Data_vector.push_back("IP10...IP17");
        oParam0.m_Data_vector.push_back("NTR1");
        oParam0.m_Data_vector.push_back("NTR2");
        oParam0.m_Data_vector.push_back("NTR3");
        oParam0.m_Data_vector.push_back("IPS1");
        oParam0.m_Data_vector.push_back("IPS2");
        oParam0.m_Data_vector.push_back("IPS3");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Messages";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("[SBF List]");
        oParam1.m_Data_vector.push_back("+Measurements");
        oParam1.m_Data_vector.push_back("+GPS");
        oParam1.m_Data_vector.push_back("+GLO");
        oParam1.m_Data_vector.push_back("+GAL");
        oParam1.m_Data_vector.push_back("+GEO");
        oParam1.m_Data_vector.push_back("+CMP");
        oParam1.m_Data_vector.push_back("+PVTCart");
        oParam1.m_Data_vector.push_back("+PVTGeod");
        oParam1.m_Data_vector.push_back("+PVTExtra");
        oParam1.m_Data_vector.push_back("+Attitude");
        oParam1.m_Data_vector.push_back("+Time");
        oParam1.m_Data_vector.push_back("+Status");
        oParam1.m_Data_vector.push_back("+LBand");
        oParam1.m_Data_vector.push_back("+UserGroups");
        oParam1.m_Data_vector.push_back("+Rinex");
        oParam1.m_Data_vector.push_back("+Support");
        oParam1.m_Data_vector.push_back("+RawData");
        oParam1.m_Data_vector.push_back("+GUI");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getSBFOnce
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getSBFOnce";
        oCmdInfo.m_strDesc = "�г�SBFһ���������";

        int iParamIndex = 0;

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // setSBFOutput, Stream, Cd, Messages, Interval
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "setSBFOutput";
        oCmdInfo.m_strDesc = "����SBF�������";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Stream";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+Stream1...Stream10");
        oParam0.m_Data_vector.push_back("+Res1");
        oParam0.m_Data_vector.push_back("+Res2");
        oParam0.m_Data_vector.push_back("+Res3");
        oParam0.m_Data_vector.push_back("+Res4");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ����
        st_ParamInfo oParam1;
        oParam1.m_iIndex = iParamIndex++;
        oParam1.m_strParamName = "Cd";
        oParam1.m_iDataDefaultIndex = 0;

        // ��������
        oParam1.m_Data_vector.push_back("none");
        oParam1.m_Data_vector.push_back("DSK1");
        oParam1.m_Data_vector.push_back("COM1");
        oParam1.m_Data_vector.push_back("COM2");
        oParam1.m_Data_vector.push_back("COM3");
        oParam1.m_Data_vector.push_back("COM4");
        oParam1.m_Data_vector.push_back("USB1");
        oParam1.m_Data_vector.push_back("USB2");
        oParam1.m_Data_vector.push_back("IP10...IP17");
        oParam1.m_Data_vector.push_back("NTR1");
        oParam1.m_Data_vector.push_back("NTR2");
        oParam1.m_Data_vector.push_back("NTR3");
        oParam1.m_Data_vector.push_back("IPS1");
        oParam1.m_Data_vector.push_back("IPS2");
        oParam1.m_Data_vector.push_back("IPS3");
        oCmdInfo.m_Param_vector.push_back(oParam1);

        // ����
        st_ParamInfo oParam2;
        oParam2.m_iIndex = iParamIndex++;
        oParam2.m_strParamName = "Messages";
        oParam2.m_iDataDefaultIndex = 0;

        // ��������
        oParam2.m_Data_vector.push_back("none");
        oParam2.m_Data_vector.push_back("[SBF List]");
        oParam2.m_Data_vector.push_back("+Measurements");
        oParam2.m_Data_vector.push_back("+RawNavBits");
        oParam2.m_Data_vector.push_back("+GPS");
        oParam2.m_Data_vector.push_back("+GLO");
        oParam2.m_Data_vector.push_back("+GAL");
        oParam2.m_Data_vector.push_back("+GEO");
        oParam2.m_Data_vector.push_back("+CMP");
        oParam2.m_Data_vector.push_back("+PVTCart");
        oParam2.m_Data_vector.push_back("+PVTGeod");
        oParam2.m_Data_vector.push_back("+PVTExtra");
        oParam2.m_Data_vector.push_back("+Attitude");
        oParam2.m_Data_vector.push_back("+Time");
        oParam2.m_Data_vector.push_back("+Event");
        oParam2.m_Data_vector.push_back("+DiffCorr");
        oParam2.m_Data_vector.push_back("+Status");
        oParam2.m_Data_vector.push_back("+LBand");
        oParam2.m_Data_vector.push_back("+UserGroups");
        oParam2.m_Data_vector.push_back("+Rinex");
        oParam2.m_Data_vector.push_back("+Support");
        oParam2.m_Data_vector.push_back("+RawData");
        oParam2.m_Data_vector.push_back("+GUI");
        oCmdInfo.m_Param_vector.push_back(oParam2);

        // ����
        st_ParamInfo oParam3;
        oParam3.m_iIndex = iParamIndex++;
        oParam3.m_strParamName = "Interval";
        oParam3.m_iDataDefaultIndex = 0;

        // ��������
        oParam3.m_Data_vector.push_back("off");
        oParam3.m_Data_vector.push_back("OnChange");
        oParam3.m_Data_vector.push_back("msec10");
        oParam3.m_Data_vector.push_back("msec20");
        oParam3.m_Data_vector.push_back("msec40");
        oParam3.m_Data_vector.push_back("msec50");
        oParam3.m_Data_vector.push_back("msec100");
        oParam3.m_Data_vector.push_back("msec200");
        oParam3.m_Data_vector.push_back("msec500");
        oParam3.m_Data_vector.push_back("sec1");
        oParam3.m_Data_vector.push_back("sec2");
        oParam3.m_Data_vector.push_back("sec5");
        oParam3.m_Data_vector.push_back("sec10");
        oParam3.m_Data_vector.push_back("sec15");
        oParam3.m_Data_vector.push_back("sec30");
        oParam3.m_Data_vector.push_back("sec60");
        oParam3.m_Data_vector.push_back("min2");
        oParam3.m_Data_vector.push_back("min5");
        oParam3.m_Data_vector.push_back("min10");
        oParam3.m_Data_vector.push_back("min15");
        oParam3.m_Data_vector.push_back("min30");
        oParam3.m_Data_vector.push_back("min60");
        oCmdInfo.m_Param_vector.push_back(oParam3);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

    // getSBFOutput, Stream
    {
        // ����
        st_CmdInfo oCmdInfo;
        oCmdInfo.m_iIndex = iCmdIndex++;
        oCmdInfo.m_strCmdName = "getSBFOutput";
        oCmdInfo.m_strDesc = "�г�SBF����Ĳ���";

        int iParamIndex = 0;

        // ����
        st_ParamInfo oParam0;
        oParam0.m_iIndex = iParamIndex++;
        oParam0.m_strParamName = "Stream";
        oParam0.m_iDataDefaultIndex = 0;

        // ��������
        oParam0.m_Data_vector.push_back("+Stream1...Stream10");
        oParam0.m_Data_vector.push_back("+Res1");
        oParam0.m_Data_vector.push_back("+Res2");
        oParam0.m_Data_vector.push_back("+Res3");
        oParam0.m_Data_vector.push_back("+Res4");
        oParam0.m_Data_vector.push_back("all");
        oCmdInfo.m_Param_vector.push_back(oParam0);

        // ���
        m_Cmd_vector.push_back(oCmdInfo);
    }

}


