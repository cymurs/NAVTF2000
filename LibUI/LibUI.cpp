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
static bool g_bCreated = false;             // �Ƿ��Ѿ���������
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
// �رմ���
bool CloseQtWidgetAsUse()
{
    if(NULL == g_pMainWidget) return false;

    return g_pMainWidget->close();
}

// --------------------------------------------------------------------------------------
// ��ʾ���ڣ�parent=0 ��ʾ��Ϊ������ʹ�ã��������Ӵ���
void ShowQtWidgetAsQt(QApplication& myApp, QWidget* parent/* = 0*/)
{
    // ������һ��
    if(true == g_bCreated) return;
    g_bCreated = true;

    SetStyleSheetFunc(myApp);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("system"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("system"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("system"));

    g_pMainWidget = new CMainWidget(parent);
    if(0 != parent) g_pMainWidget->setWindowTitle("Navtf UI���");
    g_pMainWidget->show();

    if(0 == parent)             // ������ʾ
    {
        QDesktopWidget* pDesk = myApp.desktop();
        g_pMainWidget->move((pDesk->width()-g_pMainWidget->width())/2, (pDesk->height()-g_pMainWidget->height())/2);
    }
}

// �Ӵ��ڣ��ƶ�
void ResizeQtWidgetAsQt(QRect& rect)
{
    if(NULL == g_pMainWidget) return;

    g_pMainWidget->setGeometry(rect);
}

// --------------------------------------------------------------------------------------
// ��ʾ����
void ShowQtWidgetAsWin( HWND parent)
{
    // ������һ��
    if(true == g_bCreated) return;
    g_bCreated = true;

    // ��ʼ������Ϣ����
    QMfcApp::pluginInstance( );

    // Win
    QWinWidget win( parent );
    win.showCentered();

    SetStyleSheetFunc(*qApp);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("system"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("system"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("system"));

    g_pMainWidget = new CMainWidget();
    g_pMainWidget->setWindowTitle("Navtf UI���");
    g_pMainWidget->show();
}

// �ƶ�����
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


