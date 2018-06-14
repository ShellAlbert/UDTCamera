#include <QApplication>
#include <QFile>
#include <zmainwindow.h>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QFile fileQSS(":/skin/ui_default.qss");
    if(fileQSS.open(QIODevice::ReadOnly))
    {
        QString qssData=fileQSS.readAll();
        app.setStyleSheet(qssData);
        fileQSS.close();
    }
    ZMainWindow mainWin;
    mainWin.show();
    return app.exec();
}
