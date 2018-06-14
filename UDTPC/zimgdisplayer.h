#ifndef ZIMGDISPLAYER_H
#define ZIMGDISPLAYER_H

#include <QtWidgets/QWidget>
#include <QtGui/QImage>
#include <QVector>
#include <QTimer>
#include <zvideorxthread.h>
#include <QTimer>
class ZImgDisplayer:public QWidget
{
    Q_OBJECT
public:
    ZImgDisplayer();
    ~ZImgDisplayer();

    void ZUpdateNewImg(QImage img,QPoint pt,QRect rect);
public slots:
    void ZSlotImgCntChanged(qint32 nCnt);
signals:
    void ZSigMsg(const QString &msg);
protected:
    void paintEvent(QPaintEvent *e);
    QSize sizeHint() const;
private slots:
    void ZSlotCalcRecvFPS();
private:
    QImage m_img;
public:
    qint32 m_nImgCnt;

private:
    QTimer *m_timer;
    qint32 m_nImgCntOld;
    qint32 m_nRecvFps;

    QPoint m_calibrateCenter;
    QRect m_rect;
};

#endif // ZIMGDISPLAYER_H
