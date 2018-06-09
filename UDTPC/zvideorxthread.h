#ifndef ZVIDEORXTHREAD_H
#define ZVIDEORXTHREAD_H

#include <QThread>
class ZVideoRxThread : public QThread
{
    Q_OBJECT
public:
    ZVideoRxThread();
    ~ZVideoRxThread();

    qint32 ZStartThread();
    qint32 ZStopThread();
protected:
    void run();

signals:
    void ZSigNewImg(const QImage &img);
    void ZSigDisconnected();
private:
    bool m_bExitFlag;
};

#endif // ZVIDEORXTHREAD_H
