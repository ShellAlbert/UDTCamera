#include "zimgdisplayer.h"
#include <QtGui/QPainter>
ZImgDisplayer::ZImgDisplayer(qint32 nUDPPort)
{
    this->m_videoRx=new ZVideoRxThread(nUDPPort);
    connect(this->m_videoRx,SIGNAL(ZSigNewImg(QImage)),this,SLOT(ZSlotShowImg(QImage)));
    connect(this->m_videoRx,SIGNAL(ZSigMsg(QString)),this,SIGNAL(ZSigMsg(QString)));
}
ZImgDisplayer::~ZImgDisplayer()
{
    this->m_videoRx->ZStopThread();
    delete this->m_videoRx;
}
void ZImgDisplayer::ZSlotShowImg(QImage img)
{
    this->m_img=img;
    this->update();
}
void ZImgDisplayer::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    if(this->m_img.isNull())
    {
        painter.fillRect(QRectF(0,0,this->width(),this->height()),Qt::black);
        return;
    }
    painter.drawImage(0,0,this->m_img);
}
QSize ZImgDisplayer::sizeHint() const
{
    return QSize(640,480);
}
