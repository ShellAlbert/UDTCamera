#ifndef ZIMGDISPLAYER_H
#define ZIMGDISPLAYER_H

#include <QtWidgets/QWidget>
#include <QtGui/QImage>
#include <QVector>
#include <QTimer>
#include <zvideorxthread.h>
class ZImgDisplayer:public QWidget
{
    Q_OBJECT
public:
    ZImgDisplayer(qint32 nUDPPort);
    ~ZImgDisplayer();

public slots:
    void ZSlotShowImg(QImage img);
signals:
    void ZSigMsg(const QString &msg);
protected:
    void paintEvent(QPaintEvent *e);
    QSize sizeHint() const;
private:
    QImage m_img;
public:
    ZVideoRxThread *m_videoRx;
};

#endif // ZIMGDISPLAYER_H
