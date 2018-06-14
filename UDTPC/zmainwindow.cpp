#include "zmainwindow.h"
#include <QMessageBox>
#include <QRect>
ZMainWindow::ZMainWindow(QWidget *parent) : QWidget(parent)
{
    this->m_llIP=new QLabel(tr("图像处理板IP"));
    this->m_cbIP=new QComboBox;
    this->m_cbIP->setEditable(true);
    this->m_tbConnect=new QToolButton;
    this->m_tbConnect->setText(tr("连接"));
    connect(this->m_tbConnect,SIGNAL(clicked(bool)),this,SLOT(ZSlotConnect()));
    this->m_barMatched=new QProgressBar;
    this->m_barMatched->setOrientation(Qt::Horizontal);
    this->m_barMatched->setRange(0,100);
    this->m_barMatched->setValue(0);
    this->m_llCalibrateXY=new QLabel;
    this->m_llCalibrateXY->setAlignment(Qt::AlignCenter);
    this->m_llCalibrateXY->setText(tr("校正中心:(0,0)/(0,0)"));

    this->m_hLayoutCfg=new QHBoxLayout;
    this->m_hLayoutCfg->addWidget(this->m_llIP);
    this->m_hLayoutCfg->addWidget(this->m_cbIP);
    this->m_hLayoutCfg->addWidget(this->m_tbConnect);
    this->m_hLayoutCfg->addWidget(this->m_barMatched);
    this->m_hLayoutCfg->addWidget(this->m_llCalibrateXY);

    this->m_disp1=new ZImgDisplayer;
    this->m_disp2=new ZImgDisplayer;
    connect(this->m_disp1,SIGNAL(ZSigMsg(QString)),this,SLOT(ZSlotMsg(QString)));
    connect(this->m_disp2,SIGNAL(ZSigMsg(QString)),this,SLOT(ZSlotMsg(QString)));

    this->m_videoRx=new ZVideoRxThread(9000);
    connect(this->m_videoRx,SIGNAL(ZSigNewImg(QImage,QPoint,QImage,QPoint,QRect,QRect)),this,SLOT(ZSlotNewImgArrived(QImage,QPoint,QImage,QPoint,QRect,QRect)));
    connect(this->m_videoRx,SIGNAL(ZSigMsg(QString)),this,SLOT(ZSlotMsg(QString)));
    connect(this->m_videoRx,SIGNAL(ZSigConnected()),this,SLOT(ZSlotConnected()));
    connect(this->m_videoRx,SIGNAL(ZSigDisconnected()),this,SLOT(ZSlotDisconnected()));
    connect(this->m_videoRx,SIGNAL(ZSigDiffXYT(qint32,qint32,qint32)),this,SLOT(ZSlotDiffXYT(qint32,qint32,qint32)));

    connect(this->m_videoRx,SIGNAL(ZSigImgCntChanged(qint32)),this->m_disp1,SLOT(ZSlotImgCntChanged(qint32)));
    connect(this->m_videoRx,SIGNAL(ZSigImgCntChanged(qint32)),this->m_disp2,SLOT(ZSlotImgCntChanged(qint32)));

    this->m_hLayout=new QHBoxLayout;
    this->m_hLayout->addWidget(this->m_disp1);
    this->m_hLayout->addWidget(this->m_disp2);

    this->m_teLog=new QTextEdit;
    this->m_teLog->document()->setMaximumBlockCount(500);
    this->m_llDiffXY=new QLabel;
    this->m_llDiffXY->setAlignment(Qt::AlignCenter);
    this->m_hLayoutBtm=new QHBoxLayout;
    this->m_hLayoutBtm->addWidget(this->m_teLog);
    this->m_hLayoutBtm->addWidget(this->m_llDiffXY);
    this->m_vLayout=new QVBoxLayout;
    this->m_vLayout->addLayout(this->m_hLayoutCfg);
    this->m_vLayout->addLayout(this->m_hLayout);
    this->m_vLayout->addLayout(this->m_hLayoutBtm);
    this->setLayout(this->m_vLayout);

    this->m_llDiffXY->setText("<font size=\"30\">X:0\nY:0\nT:0ms</font>");
}
ZMainWindow::~ZMainWindow()
{
    delete this->m_llIP;
    delete this->m_cbIP;
    delete this->m_tbConnect;
    delete this->m_barMatched;
    delete this->m_llCalibrateXY;
    delete this->m_hLayoutCfg;

    delete this->m_disp1;
    delete this->m_disp2;
    delete this->m_hLayout;
    delete this->m_teLog;
    delete this->m_llDiffXY;
    delete this->m_hLayoutBtm;
    delete this->m_vLayout;
}
void ZMainWindow::ZSlotConnect()
{
    if(this->m_tbConnect->text()==tr("连接"))
    {
        QString serverIP=this->m_cbIP->currentText();
        if(serverIP.isEmpty())
        {
            QMessageBox::critical(this,tr("错误"),tr("请输入图像处理板IP!"));
            return;
        }
        this->m_videoRx->ZStartThread(serverIP);
        //this->m_disp2->m_videoRx->ZStartThread();
    }else{
        this->m_videoRx->ZStopThread();
        //this->m_disp2->m_videoRx->ZStopThread();
    }
}
void ZMainWindow::ZSlotMsg(const QString &msg)
{
    this->m_teLog->append(msg);
}
void ZMainWindow::ZSlotConnected()
{
    this->m_cbIP->setEnabled(false);
    this->m_tbConnect->setText(tr("断开"));
}
void ZMainWindow::ZSlotDisconnected()
{
    this->m_cbIP->setEnabled(true);
    this->m_tbConnect->setText(tr("连接"));
}
void ZMainWindow::ZSlotDiffXYT(qint32 x,qint32 y,qint32 t)
{
    this->m_llDiffXY->setText(tr("<font size=\"30\">X:%1\nY:%2\nT:%3ms</font>").arg(x).arg(y).arg(t));
}
void ZMainWindow::ZSlotNewImgArrived(const QImage &img1,const QPoint pt1,const QImage &img2,const QPoint pt2,QRect rectTemp,QRect rectMatched)
{
    this->m_disp1->ZUpdateNewImg(img1,pt1,rectTemp);
    this->m_disp2->ZUpdateNewImg(img2,pt2,rectMatched);
    this->m_llCalibrateXY->setText(tr("校正中心:(%1,%2)/(%3,%4)").arg(pt1.x()).arg(pt1.y()).arg(pt2.x()).arg(pt2.y()));
}
