#include "zmainwindow.h"

ZMainWindow::ZMainWindow(QWidget *parent) : QWidget(parent)
{
    this->m_llIP=new QLabel(tr("IP"));
    this->m_cbIP=new QComboBox;
    this->m_cbIP->setEditable(true);
    this->m_tbConnect=new QToolButton;
    this->m_tbConnect->setText(tr("Connect"));
    connect(this->m_tbConnect,SIGNAL(clicked(bool)),this,SLOT(ZSlotConnect()));

    this->m_hLayoutCfg=new QHBoxLayout;
    this->m_hLayoutCfg->addWidget(this->m_llIP);
    this->m_hLayoutCfg->addWidget(this->m_cbIP);
    this->m_hLayoutCfg->addWidget(this->m_tbConnect);
    this->m_hLayoutCfg->addStretch(1);

    this->m_disp1=new ZImgDisplayer(9000);
    this->m_disp2=new ZImgDisplayer(9001);
    connect(this->m_disp1,SIGNAL(ZSigMsg(QString)),this,SLOT(ZSlotMsg(QString)));
    connect(this->m_disp2,SIGNAL(ZSigMsg(QString)),this,SLOT(ZSlotMsg(QString)));
    this->m_hLayout=new QHBoxLayout;
    this->m_hLayout->addWidget(this->m_disp1);
    this->m_hLayout->addWidget(this->m_disp2);

    this->m_teLog=new QTextEdit;
    this->m_teLog->document()->setMaximumBlockCount(500);
    this->m_vLayout=new QVBoxLayout;
    this->m_vLayout->addLayout(this->m_hLayoutCfg);
    this->m_vLayout->addLayout(this->m_hLayout);
    this->m_vLayout->addWidget(this->m_teLog);
    this->setLayout(this->m_vLayout);
}
ZMainWindow::~ZMainWindow()
{
    delete this->m_llIP;
    delete this->m_cbIP;
    delete this->m_tbConnect;
    delete this->m_hLayoutCfg;

    delete this->m_disp1;
    delete this->m_disp2;
    delete this->m_hLayout;
    delete this->m_teLog;
    delete this->m_vLayout;
}
void ZMainWindow::ZSlotConnect()
{
    this->m_disp1->m_videoRx->ZStartThread();
    this->m_disp2->m_videoRx->ZStartThread();
}
void ZMainWindow::ZSlotMsg(const QString &msg)
{
    this->m_teLog->append(msg);
}
