#include "MainWindow.h"
#include <QApplication>
#include <QTranslator>
#include <QDebug>
#include <QProcess>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    int e=a.exec();
    if(e==931)
    {
        QProcess::startDetached(qApp->applicationFilePath());
        return 0;
    }

    w.show();
    return a.exec();
}
