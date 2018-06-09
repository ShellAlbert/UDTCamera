#ifndef ZSERVICETHREAD_H
#define ZSERVICETHREAD_H

#include <QThread>
#include <udt4/src/udt.h>
#include <QImage>
class ZServiceThread : public QThread
{
    Q_OBJECT
public:
    ZServiceThread(UDPSOCKET socket);
    ~ZServiceThread();

    qint32 ZStartThread();
    qint32 ZStopThread();
private:
    void ZTxImgOut(const QImage &preview);
protected:
    void run();
private:
    UDPSOCKET m_socket;
    bool m_bExitFlag;
};

#endif // ZSERVICETHREAD_H
