#ifndef ZMAINWINDOW_H
#define ZMAINWINDOW_H

#include <QWidget>
#include <zimgdisplayer.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QComboBox>
#include <QToolButton>
#include <QProgressBar>
#include <QTimer>
class ZMainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ZMainWindow(QWidget *parent = nullptr);
    ~ZMainWindow();
signals:

public slots:
    void ZSlotConnect();
    void ZSlotMsg(const QString &msg);
    void ZSlotConnected();
    void ZSlotDisconnected();
    void ZSlotDiffXYT(qint32 x,qint32 y,qint32 t);
    void ZSlotNewImgArrived(const QImage &img1,const QPoint pt1,const QImage &img2,const QPoint pt2,QRect rectTemp,QRect rectMatched);
private:
    QLabel *m_llIP;
    QComboBox *m_cbIP;
    QToolButton *m_tbConnect;
    QProgressBar *m_barMatched;
    QLabel *m_llCalibrateXY;
    QHBoxLayout *m_hLayoutCfg;

    ZImgDisplayer *m_disp1;
    ZImgDisplayer *m_disp2;
    QHBoxLayout *m_hLayout;
    QTextEdit *m_teLog;
    QLabel *m_llDiffXY;
    QHBoxLayout *m_hLayoutBtm;
    QVBoxLayout *m_vLayout;

private:
    ZVideoRxThread *m_videoRx;
};

#endif // ZMAINWINDOW_H
