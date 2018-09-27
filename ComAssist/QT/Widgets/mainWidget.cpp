#include "stdafx.h"
#include "mainWidget.h"

#include <QtGui>

#include "../../../Common/Function/Function.h"
#include "../../../Common/Define/DefineFile.h"

#include "../../../Libase/Libase.h"

#include "updateTab.h"
#include "statusTab.h"
#include "imageTab.h"
#include "adjustTab.h"
#include "gnssTab.h"

#pragma comment(lib, "Libase.lib")

// --------------------------------------------------------------------------------------
st_PageCheck g_oPageCheck;                      // ����Ȩ��

// �汾����
int g_iLargeVer = 2;
int g_iSmallVer = 3;
int g_iYear = 2016;
int g_iMonth = 5;
int g_iDay = 4;

// --------------------------------------------------------------------------------------
// main widget
// --------------------------------------------------------------------------------------
CMainWidget::CMainWidget(QWidget* parent) : QWidget(parent)
{
    CFuncTool::Init();

    // ��ȡ����Ȩ�޹����ļ�����Ҫ����ǰ��
    ReadAndParseFuncFile();

    m_pMyComBasic = CreateComBasic();

    m_pTabWidget = new QTabWidget(this);
    m_pUpdateTab = new CUpdateTab(this);
    m_pStatusTab = new CStatusTab(this);
    m_pMyImageTab = new CMyImageTab(this);
    m_pMyAdjustTab = new CMyAdjustTab(this);
    m_pGNSSTab = new CGNSSTab(this);

    m_pUpdateTab->SetComBasic(m_pMyComBasic);
    m_pStatusTab->SetComBasic(m_pMyComBasic);

    if(true == g_oPageCheck.UpdateTabVisiable())
    {
        m_pTabWidget->addTab(m_pUpdateTab, tr("ͨ��/����"));
    }
    else
    {
        m_pUpdateTab->hide();
    }
    if(true == g_oPageCheck.SignalTabVisiable())
    {
        m_pTabWidget->addTab(m_pMyImageTab, tr("�ź�ѡ��"));
    }
    else
    {
        m_pMyImageTab->hide();
    }
    if(true == g_oPageCheck.StatusTabVisiable())
    {
        m_pTabWidget->addTab(m_pStatusTab, tr("״̬/���"));
    }
    else
    {
        m_pStatusTab->hide();
    }
    if(true == g_oPageCheck.AdjustTabVisiable())
    {
        m_pTabWidget->addTab(m_pMyAdjustTab, tr("�豸����"));
    }
    else
    {
        m_pMyAdjustTab->hide();
    }
    if(true == g_oPageCheck.GnssTabVisiable())
    {
        m_pTabWidget->addTab(m_pGNSSTab, tr("GNSS"));
    }
    else
    {
        m_pGNSSTab->hide();
    }
    
    m_pMyImageTab->SetComNetTab(m_pUpdateTab);
    m_pMyAdjustTab->SetComNetTab(m_pUpdateTab);
    m_pGNSSTab->SetComNetTab(m_pUpdateTab);

    m_pUpdateTab->Set_MyImageTab(m_pMyImageTab);
    m_pUpdateTab->Set_StatusTab(m_pStatusTab);
    m_pUpdateTab->Set_MyAdjustTab(m_pMyAdjustTab);
    m_pUpdateTab->Set_GNSSTab(m_pGNSSTab);
    
    QScrollArea* pScrollArea = new QScrollArea(this);
    pScrollArea->setWidget(m_pTabWidget);
    pScrollArea->setWidgetResizable(true);

    QVBoxLayout* pMainVLayout = new QVBoxLayout(this);
    pMainVLayout->addWidget(pScrollArea);
    setLayout(pMainVLayout);

    if(true == g_oPageCheck.IsAdmin())
    {
        setWindowTitle(tr("Navtf ������� V%1.%2 -- ���԰汾").arg(g_iLargeVer).arg(g_iSmallVer));
    }
    else
    {
        setWindowTitle(tr("Navtf ������� V%1.%2").arg(g_iLargeVer).arg(g_iSmallVer));
    }

    // INI, size
    int iWidth = CFuncTool::GetWindowWidth_SysINI();
    int iHeight = CFuncTool::GetWindowHeight_SysINI();
    bool bMaximized = CFuncTool::GetWindowMax_SysINI();
    resize(iWidth, iHeight);
    if(true == bMaximized)
    {
        showMaximized();
    }
    else
    {
        show();
    }

    // ��ʾ�Ƿ����ڲ�ʹ�õİ汾���ڲ�ʹ�õĲ�������
    if(true == g_oPageCheck.IsAdmin())
    {
        QMessageBox::information(this, tr("ע��"), tr("�����ڲ�ʹ�õĵ��԰汾����ֹ����"));
    }
}
CMainWidget::~CMainWidget()
{
    CFuncTool::UnInit();
}

QSize CMainWidget::sizeHint() const
{
    return QSize(1024, 768);
}

void CMainWidget::paintEvent(QPaintEvent*)
{
    // Style Sheet
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

// ��ȡ����Ȩ�޹����ļ�
void CMainWidget::ReadAndParseFuncFile()
{
    // ��ȡ��������·��: ��ʽ d:/work/mms/
    char chPath[256] = {0};
    CFuncTool::GetMoudlePath(chPath, sizeof(chPath));

    // ��ʼ��Ϊһ������ҳ
    CFuncTool::AddOnePage(g_oPageCheck, PAGE_INDEX_OF_UPDATE);

    char chFile[256] = {0};
    sprintf_s(chFile, sizeof(chFile), "%s%s", chPath, "*.nav");
    if(true == CFuncTool::GetFirstFileOnFolder(chFile, chFile, sizeof(chFile)))
    {
        char chFileUse[256] = {0};
        sprintf_s(chFileUse, sizeof(chFileUse), "%s%s", chPath, chFile);
        if(true == CFuncTool::ParseFileToPageCheck(chFileUse, g_oPageCheck))
        {
            ;
        }
    }
}

void CMainWidget::resizeEvent(QResizeEvent* resizeEvent)
{
    const QSize size = resizeEvent->size();

    // INI, size
    bool bMaximized = isMaximized();
    bool bIsFull = isFullScreen();
    CFuncTool::SetWindowMax_SysINI(bMaximized);
    if((false == bMaximized) && (false == bIsFull))
    {
        CFuncTool::SetWindowWidth_SysINI(size.width());
        CFuncTool::SetWindowHeight_SysINI(size.height());
    }
}

void CMainWidget::closeEvent(QCloseEvent* closeEvent)
{
    // ѯ��
    QMessageBox qesBox(QMessageBox::Question, tr("�˳�"), tr("�Ƿ�ȷ��Ҫ�˳�?"), QMessageBox::NoButton, this);
    qesBox.addButton(tr("��"), QMessageBox::YesRole);
    qesBox.addButton(tr("��"), QMessageBox::NoRole);

    if(0 == qesBox.exec())
    {
        hide();
        //BeforeClose();
        m_pUpdateTab->CloseFunction();

        closeEvent->accept();
    }
    else 
    {
        closeEvent->ignore();
    }

}
