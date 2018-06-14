#ifndef ZVIDEORXTHREAD_H
#define ZVIDEORXTHREAD_H

#include <QThread>
#include <QPoint>
#include <QRect>
class ZVideoRxThread : public QThread
{
    Q_OBJECT
public:
    ZVideoRxThread(qint32 nUDPPort);
    ~ZVideoRxThread();

    qint32 ZStartThread(QString ipAddr);
    qint32 ZStopThread();
protected:
    void run();

signals:
    void ZSigNewImg(const QImage &img1,const QPoint calibrateXY1,///<
                    const QImage &img2,const QPoint calibrateXY2,///<
                    QRect rectTemp,QRect rectMatched);
    void ZSigDiffXYT(qint32 diffX,qint32 diffY,qint32 costMs);
    void ZSigImgCntChanged(qint32 nCnt);
    void ZSigConnected();
    void ZSigDisconnected();
    void ZSigMsg(const QString &msg);
private:
    bool m_bExitFlag;
    qint32 m_nUDPPort;
    QString m_ipAddr;
    qint32 m_nRxFrmCnt;
};

#endif // ZVIDEORXTHREAD_H
