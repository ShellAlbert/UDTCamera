#include "zimgdisplayer.h"
#include <QtGui/QPainter>
ZImgDisplayer::ZImgDisplayer()
{
//    this->m_timer=new QTimer;
//    connect(this->m_timer,SIGNAL(timeout()),this,SLOT(update()));
//    this->m_timer->start(30);
}
void ZImgDisplayer::ZSlotShowImg(QImage img)
{
//    if(this->m_vecImg.size()>30)
//    {
//        this->m_vecImg.removeFirst();
//    }
//    this->m_vecImg.append(img);
    this->m_img=img;
    this->update();
}
void ZImgDisplayer::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
//    if(this->m_vecImg.size()<=0)
//    {
//        return;
//    }
//    QPainter painter(this);
//    painter.drawImage(0,0,this->m_vecImg.first());
//    this->m_vecImg.removeFirst();
    QPainter painter(this);
    painter.drawImage(0,0,this->m_img);
}
