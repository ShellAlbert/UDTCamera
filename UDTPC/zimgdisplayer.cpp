#include "zimgdisplayer.h"
#include <QtGui/QPainter>
ZImgDisplayer::ZImgDisplayer()
{
    this->m_nImgCnt=0;

    //use this timer to calculate the recive frame rates.
    this->m_nImgCntOld=0;
    this->m_nRecvFps=0;
    this->m_timer=new QTimer;
    connect(this->m_timer,SIGNAL(timeout()),this,SLOT(ZSlotCalcRecvFPS()));
    this->m_timer->start(1000);
}
ZImgDisplayer::~ZImgDisplayer()
{

}
void ZImgDisplayer::ZUpdateNewImg(QImage img,QPoint pt,QRect rect)
{
    this->m_img=img;
    this->m_calibrateCenter=pt;
    this->m_rect=rect;
    this->update();
}
void ZImgDisplayer::ZSlotImgCntChanged(qint32 nCnt)
{
    this->m_nImgCnt=nCnt;
}
void ZImgDisplayer::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter painter(this);
    if(this->m_img.isNull())
    {
        painter.fillRect(QRectF(0,0,this->width(),this->height()),Qt::black);
        QPen pen(Qt::green,4);
        painter.setPen(pen);
        painter.drawLine(QPointF(0,0),QPointF(this->width(),this->height()));
        painter.drawLine(QPointF(this->width(),0),QPointF(0,this->height()));
        return;
    }
    painter.drawImage(QRectF(0,0,this->width(),this->height()),this->m_img);
    QPen pen(Qt::green,4);
    painter.setPen(pen);
    QFont font=painter.font();
    font.setPointSize(20);
    painter.setFont(font);
    QString msgInfo;
    msgInfo+=QString::number(this->m_nImgCnt,10);
    msgInfo+="/";
    msgInfo+=QString::number(this->m_nRecvFps,10);
    msgInfo+="fps";
    qint32 nMsgInfoW=painter.fontMetrics().width(msgInfo);
    qint32 nMsgInfoH=painter.fontMetrics().height();
    painter.drawText(QRect(0,0,nMsgInfoW,nMsgInfoH),msgInfo);

    //draw the calibrate center.
    qint32 nCrossW=20;
    qint32 nCrossH=20;
    QPoint ptTop,ptBottom;
    if(this->m_calibrateCenter.y()-nCrossH<0)
    {
        ptTop=QPoint(this->m_calibrateCenter.x(),this->m_calibrateCenter.y());
    }else{
        ptTop=QPoint(this->m_calibrateCenter.x(),this->m_calibrateCenter.y()-nCrossH);
    }
    if(this->m_calibrateCenter.y()+nCrossH>this->height())
    {
        ptBottom=QPoint(this->m_calibrateCenter.x(),this->m_calibrateCenter.y());
    }else{
        ptBottom=QPoint(this->m_calibrateCenter.x(),this->m_calibrateCenter.y()+nCrossH);
    }
    painter.drawLine(ptTop,ptBottom);

    QPoint ptLeft,ptRight;
    if(this->m_calibrateCenter.x()-nCrossW<0)
    {
        ptLeft=QPoint(this->m_calibrateCenter.x(),this->m_calibrateCenter.y());
    }else{
        ptLeft=QPoint(this->m_calibrateCenter.x()-nCrossW,this->m_calibrateCenter.y());
    }
    if(this->m_calibrateCenter.x()+nCrossW>this->width())
    {
        ptRight=QPoint(this->m_calibrateCenter.x(),this->m_calibrateCenter.y());
    }else{
        ptRight=QPoint(this->m_calibrateCenter.x()+nCrossW,this->m_calibrateCenter.y());
    }
    painter.drawLine(ptLeft,ptRight);

    //draw the rectangle.
    painter.drawRect(this->m_rect);
}
QSize ZImgDisplayer::sizeHint() const
{
    return QSize(640,480);
}
void ZImgDisplayer::ZSlotCalcRecvFPS()
{
    this->m_nRecvFps=this->m_nImgCnt-this->m_nImgCntOld;
    this->m_nImgCntOld=this->m_nImgCnt;
}
