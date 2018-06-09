#include "zvideorxthread.h"
#include <udt4/src/udt.h>
#include <QtGui/QImage>
#include <QDebug>
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
ZVideoRxThread::ZVideoRxThread()
{
    this->m_bExitFlag=false;
}
ZVideoRxThread::~ZVideoRxThread()
{

}
qint32 ZVideoRxThread::ZStartThread()
{
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
    UDT::startup();

    struct addrinfo hints, *local, *peer;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    //hints.ai_socktype = SOCK_DGRAM;

    if (0!=getaddrinfo(NULL,"9000",&hints, &local))
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

    if (0 != getaddrinfo("127.0.0.1","9000", &hints, &peer))
    {
        qDebug()<<"incorrect server/peer address.";
        emit this->ZSigDisconnected();
        return;
    }

    // connect to the server, implict bind
    if (UDT::ERROR==UDT::connect(client,peer->ai_addr,peer->ai_addrlen))
    {
        qDebug()<<"connect:"<<UDT::getlasterror().getErrorMessage();
        UDT::cleanup();
        emit this->ZSigDisconnected();
        return;
    }
    qDebug()<<"socket connected.";

    freeaddrinfo(peer);

    char *recvBuffer=new char[1024*1024*2];//2MB.

    while(!this->m_bExitFlag)
    {
        bool bExitFlag=false;
        //start to read frame size.
        char frmSizeBuffer[4];
        if (UDT::ERROR==UDT::recv(client,frmSizeBuffer,sizeof(frmSizeBuffer),0))
        {
            qDebug()<<"error:recv(),"<<UDT::getlasterror().getErrorMessage();
            break;
        }
        qDebug("%02x %02x %02x %02x\n",frmSizeBuffer[0],frmSizeBuffer[1],frmSizeBuffer[2],frmSizeBuffer[3]);
        int nFrmSizeBytes;
        char42int(frmSizeBuffer,&nFrmSizeBytes);
        qDebug("frmSize:%d,%x\n",nFrmSizeBytes,nFrmSizeBytes);

        //qint32 nRdSingle=UDT::recv(client,recvBuffer,nFrmSizeBytes,0);
        //start to read frame body.
        qint32 nRdBytes=0;
        while(nRdBytes<nFrmSizeBytes)
        {
            qint32 nRdSingle=UDT::recv(client,recvBuffer+nRdBytes,nFrmSizeBytes-nRdBytes,0);
            if (UDT::ERROR==nRdSingle)
            {
                qDebug()<<"error:recv(),"<<UDT::getlasterror().getErrorMessage();
                bExitFlag=true;
                break;
            }
            nRdBytes+=nRdSingle;
        }
        if(bExitFlag)
        {
            break;
        }
        qDebug()<<"read one frame okay!";
        QImage img((const uchar*)recvBuffer,640,480,QImage::Format_RGB888);
        emit this->ZSigNewImg(img);
    }
    qDebug()<<"socket closed!";
    emit this->ZSigDisconnected();
    delete []recvBuffer;
    UDT::close(client);
    UDT::cleanup();
    this->exit(0);
}
