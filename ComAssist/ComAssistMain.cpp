#include "stdafx.h"
#include <QApplication>
#include <QTextCodec>
#include <QFile>

#include "QT/Widgets/mainWidget.h"

// --------------------------------------------------------------------------------------
void SetStyleSheetFunc()
{
    qApp->setStyleSheet("");
    QFile file(":/comassist.qss");
    file.open(QFile::ReadOnly);
    qApp->setStyleSheet(file.readAll());
}

// --------------------------------------------------------------------------------------
// main
// --------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    QApplication mainApp(argc, argv);
    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("system"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("system"));
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("system"));

    SetStyleSheetFunc();

    CMainWidget* pMainWidget = new CMainWidget();
    pMainWidget->show();

    return mainApp.exec();
}
