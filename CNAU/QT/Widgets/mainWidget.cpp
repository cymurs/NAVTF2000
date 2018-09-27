#include "mainWidget.h"

#include <Windows.h>

#include <QtGui>

// --------------------------------------------------------------------------------------
extern QString GetPageNameOnIndex(int iIndex);      // 获取功能页名称
extern QString GetPageItemNameOnIndex(int iIndex);  // 获取功能项名称

// --------------------------------------------------------------------------------------
// main widget
// --------------------------------------------------------------------------------------
CMainWidget::CMainWidget(QWidget* parent) : QWidget(parent)
{
    CFuncTool::Init();

    InitPageCheckData();
    CreateWidgets();
    FillPageCheckInitToCtrl();

    setWindowTitle(tr("功能管理软件 Navtf V1.0"));
}
CMainWidget::~CMainWidget()
{
    CFuncTool::UnInit();
}

void CMainWidget::InitPageCheckData()
{
    // 0. PAGE_INDEX_OF_UPDATE 
    CFuncTool::AddOnePage(m_oPageCheck_Init, PAGE_INDEX_OF_UPDATE);

    // 1. PAGE_INDEX_OF_SIGNAL
    CFuncTool::AddOnePage(m_oPageCheck_Init, PAGE_INDEX_OF_SIGNAL);

    // 2. PAGE_INDEX_OF_STATUS
    CFuncTool::AddOnePage(m_oPageCheck_Init, PAGE_INDEX_OF_STATUS);

    // 3. PAGE_INDEX_OF_ADJUST
    CFuncTool::AddOnePage(m_oPageCheck_Init, PAGE_INDEX_OF_ADJUST);

    // 4. PAGE_INDEX_OF_GNSS
    CFuncTool::AddOnePage(m_oPageCheck_Init, PAGE_INDEX_OF_GNSS);
}

void CMainWidget::FillPageCheckInitToCtrl()
{
    for(int i = 0; i < m_oPageCheck_Init.GetPageNums(); ++i)
    {
        m_pPage_ListWidget->addItem(GetPageNameOnIndex(m_oPageCheck_Init.m_PageData_vector[i].m_iPageIndex));
    }
}

void CMainWidget::CreateWidgets()
{
    QVBoxLayout* pVLayout = new QVBoxLayout();
    pVLayout->addWidget(CreateOptionGroup());
    pVLayout->addWidget(CreateFileGroup());

    m_pInfoShow_Label = new QLabel(tr(""));
    m_pInfoShow_Label->setStyleSheet(tr("font-size: 20px"));
    pVLayout->addWidget(m_pInfoShow_Label);

    setLayout(pVLayout);
}

QGroupBox* CMainWidget::CreateOptionGroup()
{
    QGroupBox* pGroup = new QGroupBox(tr("功能选择"));

    m_pPage_ListWidget = new QListWidget();
    m_pPage_ListWidget->setSortingEnabled(true);
    m_pPageAdd_Button = new QPushButton(tr("使 用"));
    m_pPageAdd_Button->setMaximumSize(QSize(60, 50));
    m_pPageAdd_Button->setMinimumSize(QSize(60, 50));
    m_pPageAdd_Button->setEnabled(false);
    m_pPageUse_ListWidget = new QListWidget();
    m_pPageUse_ListWidget->setSortingEnabled(true);
    m_pPageUseClear_Button = new QPushButton(tr("清 空"));
    m_pPageUseClear_Button->setMaximumSize(QSize(60, 50));
    m_pPageUseClear_Button->setMinimumSize(QSize(60, 50));
    m_pPageUseClear_Button->setEnabled(false);

    connect(m_pPage_ListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(PageClicked_Slot(QListWidgetItem*)));
    connect(m_pPageAdd_Button, SIGNAL(clicked()), this, SLOT(PageAdd_Slot()));
    connect(m_pPageUse_ListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(PageUseClicked_Slot(QListWidgetItem*)));
    connect(m_pPageUseClear_Button, SIGNAL(clicked()), this, SLOT(PageUseClear_Slot()));

    QVBoxLayout* pVLayout1 = new QVBoxLayout();
    pVLayout1->addWidget(m_pPage_ListWidget);
    pVLayout1->addWidget(m_pPageAdd_Button);
    pVLayout1->addWidget(m_pPageUse_ListWidget);
    pVLayout1->addWidget(m_pPageUseClear_Button);

    m_pPageItem_ListWidget = new QListWidget();
    m_pPageItem_ListWidget->setSortingEnabled(true);
    m_pPageItemAdd_Button = new QPushButton(tr("使 用"));
    m_pPageItemAdd_Button->setMaximumSize(QSize(60, 50));
    m_pPageItemAdd_Button->setMinimumSize(QSize(60, 50));
    m_pPageItemAdd_Button->setEnabled(false);
    m_pPageItemUse_ListWidget = new QListWidget();
    m_pPageItemUse_ListWidget->setSortingEnabled(true);
    m_pPageItemUseClear_Button = new QPushButton(tr("清 空"));
    m_pPageItemUseClear_Button->setMaximumSize(QSize(60, 50));
    m_pPageItemUseClear_Button->setMinimumSize(QSize(60, 50));
    m_pPageItemUseClear_Button->setEnabled(false);

    connect(m_pPageItem_ListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(PageItemClicked_Slot(QListWidgetItem*)));
    connect(m_pPageItemAdd_Button, SIGNAL(clicked()), this, SLOT(PageItemAdd_Slot()));
    connect(m_pPageItemUseClear_Button, SIGNAL(clicked()), this, SLOT(PageItemUseClear_Slot()));

    QVBoxLayout* pVLayout2 = new QVBoxLayout();
    pVLayout2->addWidget(m_pPageItem_ListWidget);
    pVLayout2->addWidget(m_pPageItemAdd_Button);
    pVLayout2->addWidget(m_pPageItemUse_ListWidget);
    pVLayout2->addWidget(m_pPageItemUseClear_Button);

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addLayout(pVLayout1);
    pHLayout->addLayout(pVLayout2);
    pGroup->setLayout(pHLayout);
    return pGroup;
}

QGroupBox* CMainWidget::CreateFileGroup()
{
    QGroupBox* pGroup = new QGroupBox(tr("文件保存"));

    QLabel* pFileNameLabel = new QLabel(tr("文件名称"));
    m_pFileName_Edit = new QLineEdit();
    m_pFileName_Edit->setMinimumHeight(30);
    m_pFileName_Edit->setStyleSheet("font-size: 20px");

    QLabel* pFileVerLabel = new QLabel(tr("文件版本"));
    m_pFileVer_Edit = new QLineEdit();
    m_pFileVer_Edit->setMinimumHeight(30);
    m_pFileVer_Edit->setStyleSheet("font-size: 20px");

    m_pAdmin_CheckBox = new QCheckBox(tr("内部管理员"));
    connect(m_pAdmin_CheckBox, SIGNAL(clicked()), this, SLOT(AdminCheck_Slot()));

    QHBoxLayout* pHLayout0 = new QHBoxLayout();
    pHLayout0->addWidget(pFileNameLabel);
    pHLayout0->addWidget(m_pFileName_Edit);
    pHLayout0->addStretch();
    pHLayout0->addWidget(pFileVerLabel);
    pHLayout0->addWidget(m_pFileVer_Edit);
    pHLayout0->addStretch();
    pHLayout0->addWidget(m_pAdmin_CheckBox);

    m_pFileOpen_Button = new QPushButton(tr("打开文件..."));
    m_pFileOpen_Button->setMaximumSize(QSize(80, 30));
    m_pFileOpen_Button->setMinimumSize(QSize(80, 30));
    m_pFileOpenPath_Edit = new QLineEdit();
    m_pFileOpenPath_Edit->setReadOnly(true);
    m_pFileOpenPath_Edit->setMinimumHeight(30);
    m_pFileOpenPath_Edit->setStyleSheet("font-size: 20px");
    m_pFileOpenRead_Button = new QPushButton(tr("读 取"));
    m_pFileOpenRead_Button->setMaximumSize(QSize(60, 30));
    m_pFileOpenRead_Button->setMinimumSize(QSize(60, 30));
    m_pFileOpenRead_Button->setEnabled(false);

    connect(m_pFileOpen_Button, SIGNAL(clicked()), this, SLOT(SelectOneFile_Slot()));
    connect(m_pFileOpenRead_Button, SIGNAL(clicked()), this, SLOT(FileOpenRead_Slot()));

    QHBoxLayout* pHLayout = new QHBoxLayout();
    pHLayout->addWidget(m_pFileOpen_Button);
    pHLayout->addWidget(m_pFileOpenPath_Edit);
    pHLayout->addWidget(m_pFileOpenRead_Button);

    m_pPathSelect_Button = new QPushButton(tr("选择路径..."));
    m_pPathSelect_Button->setMaximumSize(QSize(80, 30));
    m_pPathSelect_Button->setMinimumSize(QSize(80, 30));
    m_pPathSelect_Edit = new QLineEdit();
    m_pPathSelect_Edit->setReadOnly(true);
    m_pPathSelect_Edit->setMinimumHeight(30);
    m_pPathSelect_Edit->setStyleSheet("font-size: 20px");
    m_pPathFileSave_Button = new QPushButton(tr("保 存"));
    m_pPathFileSave_Button->setMaximumSize(QSize(60, 30));
    m_pPathFileSave_Button->setMinimumSize(QSize(60, 30));
    m_pPathFileSave_Button->setEnabled(false);

    connect(m_pPathSelect_Button, SIGNAL(clicked()), this, SLOT(SelectOnePath_Slot()));
    connect(m_pPathFileSave_Button, SIGNAL(clicked()), this, SLOT(PathFileSave_Slot()));

    // INI
    float fVer = CFuncTool::GetCNAUFileVersion();
    char chPath[256] = {0};
    sprintf_s(chPath, sizeof(chPath), "%0.1f", fVer);
    m_pFileVer_Edit->setText(chPath);
    
    CFuncTool::GetCNAUFilePath(chPath, sizeof(chPath));
    m_pPathSelect_Edit->setText(chPath);

    QHBoxLayout* pHLayout2 = new QHBoxLayout();
    pHLayout2->addWidget(m_pPathSelect_Button);
    pHLayout2->addWidget(m_pPathSelect_Edit);
    pHLayout2->addWidget(m_pPathFileSave_Button);

    QVBoxLayout* pVLayout = new QVBoxLayout();
    pVLayout->addLayout(pHLayout0);
    pVLayout->addLayout(pHLayout);
    pVLayout->addLayout(pHLayout2);
    pGroup->setLayout(pVLayout);
    return pGroup;
}

void CMainWidget::SelectOneFile_Slot()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("选择一个文件"),"",tr("功能文件 (*.nav)"));
    if(false == fileName.isEmpty())
    {
        m_pFileOpenPath_Edit->setText(fileName);
        m_pFileOpenRead_Button->setEnabled(true);
    }
}
void CMainWidget::SelectOnePath_Slot()
{
    QString strPath = QFileDialog::getExistingDirectory(this, tr("选择保存目录"));
    if(false == strPath.isEmpty())
    {
        m_pPathSelect_Edit->setText(strPath);
        m_pPathFileSave_Button->setEnabled(true);
    }
}

void CMainWidget::PageClicked_Slot(QListWidgetItem* pListItem)
{
    if(NULL == pListItem) return;

    m_pPageItemAdd_Button->setEnabled(false);
    m_pPageAdd_Button->setEnabled(true);
    m_pPageItem_ListWidget->clear();        // 清

    bool bAdmin = false;
    if(true == m_pAdmin_CheckBox->isChecked()) bAdmin = true;

    int iPageIndex = GetPageOrItemIndex(pListItem->text());
    if(iPageIndex < 0) return;

    int iItemIndex = 0;
    for(int i = 0; i < m_oPageCheck_Init.GetPageNums(); ++i)
    {
        st_PageData& PD = m_oPageCheck_Init.m_PageData_vector[i];
        if(iPageIndex == PD.m_iPageIndex)
        {
            for(int j = 0; j < PD.GetPageItemNums(); ++j)
            {
                iItemIndex = PD.m_PageItem_vector[j].m_iPageItemIndex;
                if((false == bAdmin) && ((iItemIndex & 0xFF) >= 0x80)) continue;        // 不是管理员，不使用管理功能
                m_pPageItem_ListWidget->addItem(GetPageItemNameOnIndex(iItemIndex));
            }
            break;
        }
    }
}

void CMainWidget::PageUseClicked_Slot(QListWidgetItem* pListItem)
{
    if(NULL == pListItem) return;

    m_pPageItemUse_ListWidget->clear();

    int iPageIndex = GetPageOrItemIndex(pListItem->text());
    if(iPageIndex < 0) return;
    for(int i = 0; i < m_oPageCheck_Use.GetPageNums(); ++i)
    {
        st_PageData& PD = m_oPageCheck_Use.m_PageData_vector[i];
        if(iPageIndex == PD.m_iPageIndex)
        {
            for(int j = 0; j < PD.GetPageItemNums(); ++j)
            {
                if(true == PD.m_PageItem_vector[j].m_bPageItemUse)
                {
                    m_pPageItemUse_ListWidget->addItem(GetPageItemNameOnIndex(PD.m_PageItem_vector[j].m_iPageItemIndex));
                }
            }
            break;
        }
    }
}

int CMainWidget::GetPageOrItemIndex(QString& strIndexName)
{
    int iIndex = -1;
    QStringList listText = strIndexName.split("_", QString::SkipEmptyParts);
    if(listText.count() > 0)
    {
        iIndex = listText[0].toInt();
    }
    return iIndex;
}

void CMainWidget::PageItemClicked_Slot(QListWidgetItem* pListItem)
{
    if(NULL == pListItem) return;

    m_pPageItemAdd_Button->setEnabled(true);
}

// 添加 页
void CMainWidget::PageAdd_Slot()
{
    QListWidgetItem* pListItem = m_pPage_ListWidget->currentItem();
    if(NULL == pListItem) return;

    // 一次有效
    m_pAdmin_CheckBox->setEnabled(false);
    m_oPageCheck_Use.m_bAdmin = m_pAdmin_CheckBox->isChecked();

    m_pPageAdd_Button->setEnabled(false);
    m_pPageUseClear_Button->setEnabled(true);
    m_pPathFileSave_Button->setEnabled(true);

    int iPageIndex = GetPageOrItemIndex(pListItem->text());
    if(iPageIndex < 0) return;

    CFuncTool::AddOnePage(m_oPageCheck_Use, iPageIndex);

    // 显示
    ShowPageUse();
}

// 添加 页功能项
void CMainWidget::PageItemAdd_Slot()
{
    QListWidgetItem* pPageItemCurItem = m_pPageItem_ListWidget->currentItem();
    if(NULL == pPageItemCurItem) return;
    QListWidgetItem* pPageCurItem = m_pPage_ListWidget->currentItem();
    if(NULL == pPageCurItem) return;

    m_pPageItemAdd_Button->setEnabled(false);
    m_pPageItemUseClear_Button->setEnabled(true);

    int iPageIndex = GetPageOrItemIndex(pPageCurItem->text());
    int iPageItemIndex = GetPageOrItemIndex(pPageItemCurItem->text());

    CFuncTool::AddOnePageItem(m_oPageCheck_Use, iPageIndex, iPageItemIndex, true);

    // 显示
    ShowPageItemUse(iPageIndex);
}

void CMainWidget::ShowPageUse()
{
    m_pPageUse_ListWidget->clear();
    for(int i = 0; i < m_oPageCheck_Use.GetPageNums(); ++i)
    {
        st_PageData& PD = m_oPageCheck_Use.m_PageData_vector[i];
        if(true == PD.m_bPageUse)
        {
            m_pPageUse_ListWidget->addItem(GetPageNameOnIndex(PD.m_iPageIndex));
        }
    }
}
void CMainWidget::ShowPageItemUse(int iPageIndex)
{
    m_pPageItemUse_ListWidget->clear();
    for(int i = 0; i < m_oPageCheck_Use.GetPageNums(); ++i)
    {
        st_PageData& PD = m_oPageCheck_Use.m_PageData_vector[i];
        if((true == PD.m_bPageUse) && (iPageIndex == PD.m_iPageIndex))
        {
            for(int j = 0; j < PD.GetPageItemNums(); ++j)
            {
                if(true == PD.m_PageItem_vector[j].m_bPageItemUse)
                {
                    m_pPageItemUse_ListWidget->addItem(GetPageItemNameOnIndex(PD.m_PageItem_vector[j].m_iPageItemIndex));
                }
            }
            break;
        }
    }
}

void CMainWidget::PageUseClear_Slot()
{
    // 清除 页
    for(int i = 0; i < m_oPageCheck_Use.GetPageNums(); ++i)
    {
        st_PageData& PD = m_oPageCheck_Use.m_PageData_vector[i];
        PD.Clear();
    }
    m_oPageCheck_Use.Clear();

    // 显示
    m_pPageUse_ListWidget->clear();
    m_pPageItemUse_ListWidget->clear();
}
void CMainWidget::PageItemUseClear_Slot()
{
    // 置 false
    for(int i = 0; i < m_oPageCheck_Use.GetPageNums(); ++i)
    {
        st_PageData& PD = m_oPageCheck_Use.m_PageData_vector[i];
        for(int j = 0; j < PD.GetPageItemNums(); ++j)
        {
            PD.m_PageItem_vector[j].m_bPageItemUse = false;
        }
    }

    // 显示
    m_pPageItemUse_ListWidget->clear();
}

void CMainWidget::AdminCheck_Slot()
{
    m_pAdmin_CheckBox->setEnabled(false);
    m_pFileName_Edit->setText(tr("Admin"));
}

void CMainWidget::FileOpenRead_Slot()
{
    // 清
    PageUseClear_Slot();

    QString strFile = m_pFileOpenPath_Edit->text();
    if(strFile.isEmpty()) return;

    m_pFileOpenRead_Button->setEnabled(false);

    if(true == CFuncTool::ParseFileToPageCheck(strFile.toLatin1().data(), m_oPageCheck_Use))
    {
        m_pAdmin_CheckBox->setChecked(m_oPageCheck_Use.m_bAdmin);
        m_pInfoShow_Label->setText(tr("文件读取成功"));
    }
    else
    {
        m_pFileOpenRead_Button->setEnabled(true);
        m_pInfoShow_Label->setText(tr("错误，文件打开失败"));
    }

    // 显示
    ShowPageUse();
}

void CMainWidget::PathFileSave_Slot()
{
    m_pPathFileSave_Button->setEnabled(false);

    QString strName = m_pFileName_Edit->text();
    QString strVer = m_pFileVer_Edit->text();
    if(strName.isEmpty()) strName = tr("navs");
    if(strVer.isEmpty()) strVer = tr("1.0");
    QString strFilePath = tr("%1/%2_V%3.nav").arg(m_pPathSelect_Edit->text()).arg(strName).arg(strVer);
    QString strTxtFile = tr("%1/%2_V%3.txt").arg(m_pPathSelect_Edit->text()).arg(strName).arg(strVer);
    HANDLE hTxtFile = CreateFile(strTxtFile.toLatin1().data(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
    HANDLE hFile = CreateFile(strFilePath.toLatin1().data(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL);
    if(INVALID_HANDLE_VALUE != hFile)
    {
        DWORD dwWrites;
        char chEnter[2] = {0x0D, 0x0A};

        int iAllSize = 4*1024;
        char* pBuf = new char[iAllSize];
        memset(pBuf, 0, iAllSize);

        // Num=5,Admin=0
        int iAdmin = 0;
        if(true == m_oPageCheck_Use.m_bAdmin) iAdmin = 1;
        QString strData = tr("Num=%1,Admin=%2").arg(m_oPageCheck_Use.GetPageNums()).arg(iAdmin);
        memcpy(pBuf+strlen(pBuf), strData.toLatin1().data(), strData.size());
        memcpy(pBuf+strlen(pBuf), chEnter, 2);

        // 文本文件
        char chTxtData[128] = {0};
        if(INVALID_HANDLE_VALUE != hTxtFile)
        {
            if(iAdmin > 0)
            {
                strData = tr("页数量=%1, 内部管理员").arg(m_oPageCheck_Use.GetPageNums());
            }
            else
            {
                strData = tr("页数量=%1").arg(m_oPageCheck_Use.GetPageNums());
            }
            strcpy_s(chTxtData, sizeof(chTxtData), strData.toLatin1().data());
            WriteFile(hTxtFile, chTxtData, strlen(chTxtData), &dwWrites, NULL);
            WriteFile(hTxtFile, chEnter, 2, &dwWrites, NULL);
        }

        for(int i = 0; i < m_oPageCheck_Use.GetPageNums(); ++i)
        {
            st_PageData& PD = m_oPageCheck_Use.m_PageData_vector[i];

            // PIndex=0,PUse=1,PItemNum=3
            int iUse = 0;
            if(true == PD.m_bPageUse) iUse = 1;
            strData = tr("PIndex=%1,PUse=%2,PItemNum=%3").arg(PD.m_iPageIndex).arg(iUse).arg(PD.GetPageItemNums());
            memcpy(pBuf+strlen(pBuf), strData.toLatin1().data(), strData.size());
            memcpy(pBuf+strlen(pBuf), chEnter, 2);

            // 文本文件
            if(INVALID_HANDLE_VALUE != hTxtFile)
            {
                strData = tr("页名称[%1], 项数量=%2").arg(GetPageNameOnIndex(PD.m_iPageIndex)).arg(PD.GetPageItemNums());
                strcpy_s(chTxtData, sizeof(chTxtData), strData.toLatin1().data());
                WriteFile(hTxtFile, chTxtData, strlen(chTxtData), &dwWrites, NULL);
                WriteFile(hTxtFile, chEnter, 2, &dwWrites, NULL);
            }

            for(int j = 0; j < PD.GetPageItemNums(); ++j)
            {
                st_PageItemData& oPID = PD.m_PageItem_vector[j];

                // PItemIndex=1,PItemUse=1
                iUse = 0;
                if(true == oPID.m_bPageItemUse) iUse = 1;
                strData = tr("PItemIndex=%1,PItemUse=%2").arg(oPID.m_iPageItemIndex).arg(iUse);
                memcpy(pBuf+strlen(pBuf), strData.toLatin1().data(), strData.size());
                memcpy(pBuf+strlen(pBuf), chEnter, 2);

                // 文本文件
                if(INVALID_HANDLE_VALUE != hTxtFile)
                {
                    if(true == oPID.m_bPageItemUse)
                    {
                        strData = tr("    项名称[%1]").arg(GetPageItemNameOnIndex(oPID.m_iPageItemIndex));
                        strcpy_s(chTxtData, sizeof(chTxtData), strData.toLatin1().data());
                        WriteFile(hTxtFile, chTxtData, strlen(chTxtData), &dwWrites, NULL);
                        WriteFile(hTxtFile, chEnter, 2, &dwWrites, NULL);
                    }
                }
            }
        }

        // Encrypt
        strData = pBuf;
        QString strEncptAutho = CFuncTool::EncryptToString(strData);
        delete[] pBuf;
        pBuf = NULL;

        WriteFile(hFile, strEncptAutho.toLatin1().data(), strEncptAutho.size(), &dwWrites, NULL);
        CloseHandle(hFile);
        if(INVALID_HANDLE_VALUE != hTxtFile) CloseHandle(hTxtFile);
        m_pInfoShow_Label->setText(tr("文件保存成功"));

        // INI
        CFuncTool::SetCNAUFileVersion(strVer.toFloat());
        CFuncTool::SetCNAUFilePath(m_pPathSelect_Edit->text().toLatin1().data());
    }
    else
    {
        m_pInfoShow_Label->setText(tr("错误，文件保存失败"));
    }

    m_pPathFileSave_Button->setEnabled(true);
}


