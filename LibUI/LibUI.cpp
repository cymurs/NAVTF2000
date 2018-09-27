#include "LibUI.h"

#include "QT/Base/qmfcapp.h"
#include "QT/Base/qwinwidget.h"

#include <QTextCodec>
#include <QApplication>
#include <QTextCodec>
#include <QDesktopWidget>
#include <QFile>


#include "../ComAssist/QT/Widgets/mainWidget.h"

// --------------------------------------------------------------------------------------
static bool g_bCreated = false;             // 是否已经创建窗口
static CMainWidget* g_pMainWidget = NULL;

// --------------------------------------------------------------------------------------
// Style Sheet
static void SetStyleSheetFunc(QApplication& myApp)
{
    myApp.setStyleSheet("");
    QFile file(":/comassist.qss");
    file.open(QFile::ReadOnly);
    myApp.setStyleSheet(file.readAll());
}

// --------------------------------------------------------------------------------------
// 关闭窗口
bool CloseQtWidgetAsUse()
{
    if(NULL == g_pMainWidget) return false;

    return g_pMainWidget->close();
}

// --------------------------------------------------------------------------------------
// 显示窗口：parent=0 表示作为主窗口使用，否则是子窗口
void ShowQtWidgetAsQt(QApplication& myApp, QWidget* parent/* = 0*/)
{
    // 仅创建一次
    if(true == g_bCreated) return;
    g_bCreated = true;

    SetStyleSheetFunc(myApp);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("system"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("system"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("system"));

    g_pMainWidget = new CMainWidget(parent);
    if(0 != parent) g_pMainWidget->setWindowTitle("Navtf UI组件");
    g_pMainWidget->show();

    if(0 == parent)             // 居中显示
    {
        QDesktopWidget* pDesk = myApp.desktop();
        g_pMainWidget->move((pDesk->width()-g_pMainWidget->width())/2, (pDesk->height()-g_pMainWidget->height())/2);
    }
}

// 子窗口：移动
void ResizeQtWidgetAsQt(QRect& rect)
{
    if(NULL == g_pMainWidget) return;

    g_pMainWidget->setGeometry(rect);
}

// --------------------------------------------------------------------------------------
// 显示窗口
void ShowQtWidgetAsWin( HWND parent)
{
    // 仅创建一次
    if(true == g_bCreated) return;
    g_bCreated = true;

    // 初始化：消息中心
    QMfcApp::pluginInstance( );

    // Win
    QWinWidget win( parent );
    win.showCentered();

    SetStyleSheetFunc(*qApp);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("system"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("system"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("system"));

    g_pMainWidget = new CMainWidget();
    g_pMainWidget->setWindowTitle("Navtf UI组件");
    g_pMainWidget->show();
}

// 移动窗口
void MoveQtWidgetAsWin( int left, int top, int right, int bottom)
{
    if(NULL == g_pMainWidget) return;

    QRect rect;
    rect.setLeft(left);
    rect.setTop(top);
    rect.setRight(right);
    rect.setBottom(bottom);
    g_pMainWidget->setGeometry(rect);

    g_pMainWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Popup | Qt::Tool);
    g_pMainWidget->show();
}


