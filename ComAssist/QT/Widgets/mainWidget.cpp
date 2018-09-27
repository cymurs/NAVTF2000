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
st_PageCheck g_oPageCheck;                      // 功能权限

// 版本设置
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

    // 读取分析权限功能文件：需要放在前面
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
        m_pTabWidget->addTab(m_pUpdateTab, tr("通信/升级"));
    }
    else
    {
        m_pUpdateTab->hide();
    }
    if(true == g_oPageCheck.SignalTabVisiable())
    {
        m_pTabWidget->addTab(m_pMyImageTab, tr("信号选择"));
    }
    else
    {
        m_pMyImageTab->hide();
    }
    if(true == g_oPageCheck.StatusTabVisiable())
    {
        m_pTabWidget->addTab(m_pStatusTab, tr("状态/监控"));
    }
    else
    {
        m_pStatusTab->hide();
    }
    if(true == g_oPageCheck.AdjustTabVisiable())
    {
        m_pTabWidget->addTab(m_pMyAdjustTab, tr("设备调整"));
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
        setWindowTitle(tr("Navtf 管理软件 V%1.%2 -- 调试版本").arg(g_iLargeVer).arg(g_iSmallVer));
    }
    else
    {
        setWindowTitle(tr("Navtf 管理软件 V%1.%2").arg(g_iLargeVer).arg(g_iSmallVer));
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

    // 提示是否是内部使用的版本，内部使用的不允许发布
    if(true == g_oPageCheck.IsAdmin())
    {
        QMessageBox::information(this, tr("注意"), tr("这是内部使用的调试版本，禁止发布"));
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

// 读取分析权限功能文件
void CMainWidget::ReadAndParseFuncFile()
{
    // 获取程序所在路径: 形式 d:/work/mms/
    char chPath[256] = {0};
    CFuncTool::GetMoudlePath(chPath, sizeof(chPath));

    // 初始化为一个功能页
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
    // 询问
    QMessageBox qesBox(QMessageBox::Question, tr("退出"), tr("是否确定要退出?"), QMessageBox::NoButton, this);
    qesBox.addButton(tr("是"), QMessageBox::YesRole);
    qesBox.addButton(tr("否"), QMessageBox::NoRole);

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
