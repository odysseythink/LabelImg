#include "main_win.h"
#include "settings.h"
#include "libs/shape.h"

#include <QApplication>
#include <QMetaType>

int main(int argc, char *argv[])
{
    qRegisterMetaType<Shape>("Shape");
    qRegisterMetaType<Shape>("Shape&");
    QApplication a(argc, argv);
    qInstallMessageHandler(LogHandler);
    qDebug() << Settings::APP_NAME << " starting......";
    MainWin w;
    w.show();
    int nret = a.exec();
    delete Settings::GetInstance();
    return nret;
}
