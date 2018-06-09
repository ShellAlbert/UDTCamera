#include <QApplication>
#include <zvideorxthread.h>
#include <zimgdisplayer.h>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ZVideoRxThread videoRx;
    ZImgDisplayer imgDisp;
    QObject::connect(&videoRx,SIGNAL(ZSigNewImg(QImage)),&imgDisp,SLOT(ZSlotShowImg(QImage)));

    imgDisp.show();
    videoRx.ZStartThread();
    return app.exec();
}
