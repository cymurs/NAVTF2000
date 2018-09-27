#include <QApplication>
#include <QTextCodec>
#include <QDesktopWidget>

#include "QT/Widgets/mainWidget.h"

// --------------------------------------------------------------------------------------
// main
// --------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    QApplication::addLibraryPath("./plugins");

    QApplication mainApp(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("system"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("system"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("system"));

    CMainWidget* pMainWidget = new CMainWidget();
    pMainWidget->show();

    QDesktopWidget* pDesk = mainApp.desktop();
    pMainWidget->move((pDesk->width()-pMainWidget->width())/2, (pDesk->height()-pMainWidget->height())/2);

    return mainApp.exec();
}
