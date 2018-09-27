#ifndef _MAINWIDGET_H_
#define _MAINWIDGET_H_

#include <QWidget>

// --------------------------------------------------------------------------------------
// main widget
// --------------------------------------------------------------------------------------
class QTabWidget;
class CStatusTab;
class CUpdateTab;
class CMyImageTab;
class CMyAdjustTab;
class CGNSSTab;

class CMyComBasic;

// --------------------------------------------------------------------------------------
class CMainWidget : public QWidget
{
    Q_OBJECT

public:
    CMainWidget(QWidget* parent = 0);
    ~CMainWidget();

public:
    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent*);
    void closeEvent(QCloseEvent*);

private:
    // 读取分析权限功能文件
    void ReadAndParseFuncFile();

private:
    QTabWidget* m_pTabWidget;
    CStatusTab* m_pStatusTab;
    CUpdateTab* m_pUpdateTab;
    CMyImageTab* m_pMyImageTab;
    CMyAdjustTab* m_pMyAdjustTab;
    CGNSSTab* m_pGNSSTab;

private:
    CMyComBasic* m_pMyComBasic;
};

#endif
