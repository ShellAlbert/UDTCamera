#include <QApplication>
#include <zmainwindow.h>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ZMainWindow mainWin;
    mainWin.show();
    return app.exec();
}
