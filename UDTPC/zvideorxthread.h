#ifndef ZVIDEORXTHREAD_H
#define ZVIDEORXTHREAD_H

#include <QThread>
class ZVideoRxThread : public QThread
{
    Q_OBJECT
public:
    ZVideoRxThread(qint32 nUDPPort);
    ~ZVideoRxThread();

    qint32 ZStartThread();
    qint32 ZStopThread();
protected:
    void run();

signals:
    void ZSigNewImg(const QImage &img);
    void ZSigDisconnected();
    void ZSigMsg(const QString &msg);
private:
    bool m_bExitFlag;
    qint32 m_nUDPPort;
};

#endif // ZVIDEORXTHREAD_H
