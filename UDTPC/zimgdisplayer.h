#ifndef ZIMGDISPLAYER_H
#define ZIMGDISPLAYER_H

#include <QtWidgets/QWidget>
#include <QtGui/QImage>
#include <QVector>
#include <QTimer>
class ZImgDisplayer:public QWidget
{
    Q_OBJECT
public:
    ZImgDisplayer();

public slots:
    void ZSlotShowImg(QImage img);
private:

protected:
    void paintEvent(QPaintEvent *e);

private:
    QVector<QImage> m_vecImg;
    QTimer *m_timer;
    QImage m_img;
};

#endif // ZIMGDISPLAYER_H
