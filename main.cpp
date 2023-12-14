#include "main_win.h"
#include "settings.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qInstallMessageHandler(LogHandler);
    qDebug() << Settings::APP_NAME << " starting......";
    MainWin w;
    w.show();
    int nret = a.exec();
    delete Settings::GetInstance();
    return nret;
}
