#include "zvideorxthread.h"
#include <udt4/src/udt.h>
#include <QtGui/QImage>
#include <QDebug>
#include <string.h>
#include <QPoint>
//#define USE_TCP 1
void int2char4(const int intVal,char *pChar)
{
    char *pIntVal=(char*)&intVal;
    *(pChar+0)=*(pIntVal+3);
    *(pChar+1)=*(pIntVal+2);
    *(pChar+2)=*(pIntVal+1);
    *(pChar+3)=*(pIntVal+0);
}
void char42int(const char *pChar,int *pInt)
{
    char *pIntChar=(char*)pInt;
    *(pIntChar+0)=*(pChar+3);
    *(pIntChar+1)=*(pChar+2);
    *(pIntChar+2)=*(pChar+1);
    *(pIntChar+3)=*(pChar+0);
}
ZVideoRxThread::ZVideoRxThread(qint32 nUDPPort)
{
    this->m_bExitFlag=false;
    this->m_nUDPPort=nUDPPort;
    this->m_nRxFrmCnt=0;
}
ZVideoRxThread::~ZVideoRxThread()
{

}
qint32 ZVideoRxThread::ZStartThread(QString ipAddr)
{
    this->m_nRxFrmCnt=0;
    this->m_ipAddr=ipAddr;
    this->start();
    return 0;
}
qint32 ZVideoRxThread::ZStopThread()
{
    this->m_bExitFlag=true;
    return 0;
}
void ZVideoRxThread::run()
{
    // Automatically start up and clean up UDT module.
    if(UDT::ERROR==UDT::startup())
    {
        emit this->ZSigMsg(tr("<error>:startup(),%1").arg(UDT::getlasterror().getErrorMessage()));
        emit this->ZSigDisconnected();
        return;
    }

    struct addrinfo hints, *local, *peer;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
#ifdef USE_TCP
    hints.ai_socktype = SOCK_STREAM;
#else
    hints.ai_socktype = SOCK_DGRAM;
#endif
    QString serverPort=QString::number(this->m_nUDPPort,10);
    if (0!=getaddrinfo(NULL,serverPort.toStdString().c_str(),&hints, &local))
    {
        qDebug()<<"error:incorrect network address.";
        UDT::cleanup();
        emit this->ZSigDisconnected();
        return;
    }

    UDTSOCKET client = UDT::socket(local->ai_family, local->ai_socktype, local->ai_protocol);
    // UDT Options
    //UDT::setsockopt(client, 0, UDT_CC, new CCCFactory<CUDPBlast>, sizeof(CCCFactory<CUDPBlast>));
    //UDT::setsockopt(client, 0, UDT_MSS, new int(9000), sizeof(int));
    //UDT::setsockopt(client, 0, UDT_SNDBUF, new int(10000000), sizeof(int));
    //UDT::setsockopt(client, 0, UDP_SNDBUF, new int(10000000), sizeof(int));
    //UDT::setsockopt(client, 0, UDT_MAXBW, new int64_t(12500000), sizeof(int));
#ifdef WIN32
    UDT::setsockopt(client, 0, UDT_MSS, new int(1052), sizeof(int));
#endif
    freeaddrinfo(local);

    if (0 != getaddrinfo(this->m_ipAddr.toStdString().c_str(),"9000", &hints, &peer))
    {
        emit this->ZSigMsg(tr("<error>:getaddrinfo(),invalid server/peer address."));
        emit this->ZSigDisconnected();
        return;
    }

    // connect to the server, implict bind
    if (UDT::ERROR==UDT::connect(client,peer->ai_addr,peer->ai_addrlen))
    {
        emit this->ZSigMsg(tr("<error>:connect(),%1.").arg(UDT::getlasterror().getErrorMessage()));
        UDT::cleanup();
        emit this->ZSigDisconnected();
        return;
    }
    freeaddrinfo(peer);

    emit this->ZSigMsg(tr("<info>:connected to [%1:%2].").arg(this->m_ipAddr).arg(this->m_nUDPPort));
    emit this->ZSigConnected();

    char *recvBuffer1=new char[1024*1024*2];//2MB.
    char *recvBuffer2=new char[1024*1024*2];//2MB.
    while(!this->m_bExitFlag)
    {
        qint32 nRdBytes;
        bool bExitFlag=false;
        //start to read frame size,width*height.
        //nDiffX,nDiffY,nCostMs.
        char frmSizeBuffer[84];
#ifdef USE_TCP
        if (UDT::ERROR==UDT::recv(client,frmSizeBuffer,sizeof(frmSizeBuffer),0))
        {
            emit this->ZSigMsg(tr("<error>:recv(),%1.").arg(UDT::getlasterror().getErrorMessage()));
            break;
        }

        //qDebug("%02x %02x %02x %02x\n",frmSizeBuffer[0],frmSizeBuffer[1],frmSizeBuffer[2],frmSizeBuffer[3]);
        int nDiffX,nDiffY,nCostMs;
        int nFrm1Size,nFrm1Width,nFrm1Height;
        int nFrm2Size,nFrm2Width,nFrm2Height;
        int nCalibrateX1,nCalibrateY1,nCalibrateX2,nCalibrateY2;
        int x1,y1,w1,h1;
        int x2,y2,w2,h2;
        char42int(&frmSizeBuffer[0],&nDiffX);
        char42int(&frmSizeBuffer[4],&nDiffY);
        char42int(&frmSizeBuffer[8],&nCostMs);

        char42int(&frmSizeBuffer[12],&nFrm1Width);
        char42int(&frmSizeBuffer[16],&nFrm1Height);
        char42int(&frmSizeBuffer[20],&nFrm1Size);

        char42int(&frmSizeBuffer[24],&nFrm2Width);
        char42int(&frmSizeBuffer[28],&nFrm2Height);
        char42int(&frmSizeBuffer[32],&nFrm2Size);

        char42int(&frmSizeBuffer[36],&nCalibrateX1);
        char42int(&frmSizeBuffer[40],&nCalibrateY1);
        char42int(&frmSizeBuffer[44],&nCalibrateX2);
        char42int(&frmSizeBuffer[48],&nCalibrateY2);

        char42int(&frmSizeBuffer[52],&x1);
        char42int(&frmSizeBuffer[56],&y1);
        char42int(&frmSizeBuffer[60],&w1);
        char42int(&frmSizeBuffer[64],&h1);

        char42int(&frmSizeBuffer[68],&x2);
        char42int(&frmSizeBuffer[72],&y2);
        char42int(&frmSizeBuffer[76],&w2);
        char42int(&frmSizeBuffer[80],&h2);

        emit this->ZSigDiffXYT(nDiffX,nDiffY,nCostMs);

        //start to read img1.
        nRdBytes=0;
        while(nRdBytes<nFrm1Size)
        {
            qint32 nRdSingle=UDT::recv(client,recvBuffer1+nRdBytes,nFrm1Size-nRdBytes,0);
            if (UDT::ERROR==nRdSingle)
            {
                emit this->ZSigMsg(tr("<error>:recv(),%1.").arg(UDT::getlasterror().getErrorMessage()));
                bExitFlag=true;
                break;
            }
            nRdBytes+=nRdSingle;
        }
        if(bExitFlag)
        {
            break;
        }

        //start to read img2.
        nRdBytes=0;
        while(nRdBytes<nFrm2Size)
        {
            qint32 nRdSingle=UDT::recv(client,recvBuffer2+nRdBytes,nFrm2Size-nRdBytes,0);
            if (UDT::ERROR==nRdSingle)
            {
                emit this->ZSigMsg(tr("<error>:recv(),%1.").arg(UDT::getlasterror().getErrorMessage()));
                bExitFlag=true;
                break;
            }
            nRdBytes+=nRdSingle;
        }
        if(bExitFlag)
        {
            break;
        }
        //read finished.
        qDebug()<<"read one frame okay!";
        this->m_nRxFrmCnt++;
        emit this->ZSigImgCntChanged(this->m_nRxFrmCnt);

        QImage img1((const uchar*)recvBuffer1,nFrm1Width,nFrm1Height,QImage::Format_RGB888);
        QImage img2((const uchar*)recvBuffer2,nFrm2Width,nFrm2Height,QImage::Format_RGB888);
        QRect rectTemp(x1,y1,w1,h1);
        QRect rectMatched(x2,y2,w2,h2);
        emit this->ZSigNewImg(img1,QPoint(nCalibrateX1,nCalibrateY1),///<
                              img2,QPoint(nCalibrateX2,nCalibrateY2),///<
                              rectTemp,rectMatched);
#else
        if (UDT::ERROR==UDT::recvmsg(client,frmSizeBuffer,sizeof(frmSizeBuffer)))
        {
            emit this->ZSigMsg(tr("<error>:recv(),%1.").arg(UDT::getlasterror().getErrorMessage()));
            break;
        }

        //qDebug("%02x %02x %02x %02x\n",frmSizeBuffer[0],frmSizeBuffer[1],frmSizeBuffer[2],frmSizeBuffer[3]);
        int nDiffX,nDiffY,nCostMs;
        int nFrm1Size,nFrm1Width,nFrm1Height;
        int nFrm2Size,nFrm2Width,nFrm2Height;
        int nCalibrateX1,nCalibrateY1,nCalibrateX2,nCalibrateY2;
        int x1,y1,w1,h1;
        int x2,y2,w2,h2;
        char42int(&frmSizeBuffer[0],&nDiffX);
        char42int(&frmSizeBuffer[4],&nDiffY);
        char42int(&frmSizeBuffer[8],&nCostMs);

        char42int(&frmSizeBuffer[12],&nFrm1Width);
        char42int(&frmSizeBuffer[16],&nFrm1Height);
        char42int(&frmSizeBuffer[20],&nFrm1Size);

        char42int(&frmSizeBuffer[24],&nFrm2Width);
        char42int(&frmSizeBuffer[28],&nFrm2Height);
        char42int(&frmSizeBuffer[32],&nFrm2Size);

        char42int(&frmSizeBuffer[36],&nCalibrateX1);
        char42int(&frmSizeBuffer[40],&nCalibrateY1);
        char42int(&frmSizeBuffer[44],&nCalibrateX2);
        char42int(&frmSizeBuffer[48],&nCalibrateY2);

        char42int(&frmSizeBuffer[52],&x1);
        char42int(&frmSizeBuffer[56],&y1);
        char42int(&frmSizeBuffer[60],&w1);
        char42int(&frmSizeBuffer[64],&h1);

        char42int(&frmSizeBuffer[68],&x2);
        char42int(&frmSizeBuffer[72],&y2);
        char42int(&frmSizeBuffer[76],&w2);
        char42int(&frmSizeBuffer[80],&h2);

        emit this->ZSigDiffXYT(nDiffX,nDiffY,nCostMs);

        //start to read img1.
        nRdBytes=0;
        while(nRdBytes<nFrm1Size)
        {
            qint32 nRdSingle=UDT::recvmsg(client,recvBuffer1+nRdBytes,nFrm1Size-nRdBytes);
            if (UDT::ERROR==nRdSingle)
            {
                emit this->ZSigMsg(tr("<error>:recv(),%1.").arg(UDT::getlasterror().getErrorMessage()));
                bExitFlag=true;
                break;
            }
            nRdBytes+=nRdSingle;
        }
        if(bExitFlag)
        {
            break;
        }

        //start to read img2.
        nRdBytes=0;
        while(nRdBytes<nFrm2Size)
        {
            qint32 nRdSingle=UDT::recvmsg(client,recvBuffer2+nRdBytes,nFrm2Size-nRdBytes);
            if (UDT::ERROR==nRdSingle)
            {
                emit this->ZSigMsg(tr("<error>:recv(),%1.").arg(UDT::getlasterror().getErrorMessage()));
                bExitFlag=true;
                break;
            }
            nRdBytes+=nRdSingle;
        }
        if(bExitFlag)
        {
            break;
        }
        //read finished.
        qDebug()<<"read one frame okay!";
        this->m_nRxFrmCnt++;
        emit this->ZSigImgCntChanged(this->m_nRxFrmCnt);

        QImage img1((const uchar*)recvBuffer1,nFrm1Width,nFrm1Height,QImage::Format_RGB888);
        QImage img2((const uchar*)recvBuffer2,nFrm2Width,nFrm2Height,QImage::Format_RGB888);
        QRect rectTemp(x1,y1,w1,h1);
        QRect rectMatched(x2,y2,w2,h2);
        emit this->ZSigNewImg(img1,QPoint(nCalibrateX1,nCalibrateY1),///<
                              img2,QPoint(nCalibrateX2,nCalibrateY2),///<
                              rectTemp,rectMatched);
#endif
    }
    UDT::close(client);
    UDT::cleanup();
    emit this->ZSigMsg(tr("<info>:disconnect from [%1:%2].").arg(this->m_ipAddr).arg(this->m_nUDPPort));
    emit this->ZSigDisconnected();
    delete []recvBuffer1;
    delete []recvBuffer2;
    this->exit(0);
}
