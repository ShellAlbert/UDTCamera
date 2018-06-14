#include "zservicethread.h"
#include <QDebug>
#include <QCamera>
#include <QCameraImageCapture>
#include "cameraframegrabber.h"
#include <QImage>
#include<opencv2/core/core.hpp>
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QDateTime>
void int2char4(const int intVal,char *pChar)
{
    char *pIntVal=(char*)&intVal;
    *(pChar+0)=*(pIntVal+3);
    *(pChar+1)=*(pIntVal+2);
    *(pChar+2)=*(pIntVal+1);
    *(pChar+3)=*(pIntVal+0);
}
//##### cv::Mat ---> QImage #####
QImage cvMat_to_QImage(const cv::Mat &mat ) {
    switch ( mat.type() )
    {
    case CV_8UC4:// 8-bit, 4 channel
    {
        QImage image( mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB32 );
        return image;
    }

    case CV_8UC3:// 8-bit, 3 channel
    {
        QImage image( mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888 );
        return image.rgbSwapped();
    }

    case CV_8UC1:// 8-bit, 1 channel
    {
        static QVector<QRgb>  sColorTable;
        // only create our color table once
        if ( sColorTable.isEmpty() )
        {
            for ( int i = 0; i < 256; ++i )
                sColorTable.push_back( qRgb( i, i, i ) );
        }
        QImage image( mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8 );
        image.setColorTable( sColorTable );
        return image;
    }

    default:
        qDebug("Image format is not supported: depth=%d and %d channels\n", mat.depth(), mat.channels());
        break;
    }
    return QImage();
}


//##### QImage ---> cv::Mat #####
cv::Mat QImage_to_cvMat( const QImage &image, bool inCloneImageData = true ) {
    switch ( image.format() )
    {
    // 8-bit, 4 channel
    case QImage::Format_RGB32:
    {
        cv::Mat mat( image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine() );
        return (inCloneImageData ? mat.clone() : mat);
    }

        // 8-bit, 3 channel
    case QImage::Format_RGB888:
    {
        if ( !inCloneImageData )
        {
            qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning since we use a temporary QImage";
        }
        QImage swapped = image.rgbSwapped();
        return cv::Mat( swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine() ).clone();
    }

        // 8-bit, 1 channel
    case QImage::Format_Indexed8:
    {
        cv::Mat  mat( image.height(), image.width(), CV_8UC1, const_cast<uchar*>(image.bits()), image.bytesPerLine() );

        return (inCloneImageData ? mat.clone() : mat);
    }

    default:
        qDebug("Image format is not supported: depth=%d and %d format\n", image.depth(), image.format());
        break;
    }

    return cv::Mat();
}
ZServiceThread::ZServiceThread(UDPSOCKET socket)
{
    this->m_socket=socket;
    this->m_bExitFlag=false;
}
ZServiceThread::~ZServiceThread()
{
    qDebug()<<"service thread destroyed.";
}
qint32 ZServiceThread::ZStartThread()
{
    this->start();
    return 0;
}
qint32 ZServiceThread::ZStopThread()
{
    this->m_bExitFlag=true;
    return 0;
}
void ZServiceThread::run()
{
    cv::VideoCapture capture(0);
    qint32 width=capture.get(CV_CAP_PROP_FRAME_WIDTH);
    qint32 height=capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    qDebug("width*height=%d,%d\n",width,height);
    while(!this->m_bExitFlag)
    {
        //get a frome from capture.
        cv::Mat frame;
        capture>>frame;
        if(NULL==frame.data)
        {
            qDebug()<<"captured null image!";
        }else{
            QImage newImg=cvMat_to_QImage(frame);
            qint64 nStart=QDateTime::currentDateTime().toMSecsSinceEpoch();

            UDT::TRACEINFO trace;
            UDT::perfmon(this->m_socket,&trace);
            this->ZTxImgOut(newImg);
            UDT::perfmon(this->m_socket,&trace);
            qDebug()<< "speed = " << trace.mbpsSendRate << "Mbits/sec";

            qint64 nEnd=QDateTime::currentDateTime().toMSecsSinceEpoch();
            qDebug("cost ms:%ld\n",nEnd-nStart);
        }
        this->msleep(10);
    }
    UDT::close(this->m_socket);
    this->exit(0);
}
void ZServiceThread::ZTxImgOut(const QImage &preview)
{
    qDebug()<<"get new frame.";
    char *pTxData=(char*)preview.constBits();
    qint32 nTxTotal=preview.sizeInBytes();
    //send total bytes first.
    char txTotalBuffer[4];
    int2char4(nTxTotal,txTotalBuffer);
#ifdef USE_TCP
    if(UDT::ERROR==UDT::send(this->m_socket,txTotalBuffer,sizeof(txTotalBuffer),0))
    {
        qDebug()<<"error:send(),"<<UDT::getlasterror().getErrorMessage();
        this->m_bExitFlag=true;
        return;
    }
#else
    if(UDT::ERROR==UDT::sendmsg(this->m_socket,txTotalBuffer,sizeof(txTotalBuffer),-1,true))
    {
        qDebug()<<"error:send(),"<<UDT::getlasterror().getErrorMessage();
        this->m_bExitFlag=true;
        return;
    }
#endif
    qDebug("%d,%x\n",nTxTotal,nTxTotal);
    qDebug("%02x %02x %02x %02x\n",txTotalBuffer[0],txTotalBuffer[1],txTotalBuffer[2],txTotalBuffer[3]);

#ifdef USE_TCP
    qint32 nTxBytes=0;
    while(nTxBytes<nTxTotal)
    {
        qint32 nTxSingle=UDT::send(this->m_socket,pTxData+nTxBytes,nTxTotal-nTxBytes,0);
        if(UDT::ERROR==nTxSingle)
        {
            qDebug()<<"error:send(),"<<UDT::getlasterror().getErrorMessage();
            this->m_bExitFlag=true;
            return;
        }
        nTxBytes+=nTxSingle;
    }
#else
    qint32 nTxBytes=0;
    while(nTxBytes<nTxTotal)
    {
        qint32 nTxSingle=UDT::sendmsg(this->m_socket,pTxData+nTxBytes,nTxTotal-nTxBytes,-1,true);
        if(UDT::ERROR==nTxSingle)
        {
            qDebug()<<"error:send(),"<<UDT::getlasterror().getErrorMessage();
            this->m_bExitFlag=true;
            return;
        }
        nTxBytes+=nTxSingle;
    }
#endif
    /*
    const qint32 nSingleFrame=1024;
    qint32 nTxTimes=nTxTotal/nSingleFrame;
    qint32 nTxRestBytes=nTxTotal%nSingleFrame;
    qDebug("single frame:%d,Tx Times:%d,rest Bytes:%d\n",nSingleFrame,nTxTimes,nTxRestBytes);
    for(qint32 i=0;i<nTxTimes;i++)
    {
        if(UDT::ERROR==UDT::send(this->m_socket,pTxData+i*nSingleFrame,nSingleFrame,0))
        {
            qDebug()<<"error:send(),"<<UDT::getlasterror().getErrorMessage();
            this->m_bExitFlag=true;
            return;
        }
        qDebug("Tx time:%d\n",i);
    }
    if(nTxRestBytes>0)
    {
        if(UDT::ERROR==UDT::send(this->m_socket,pTxData+nTxTimes*nSingleFrame,nTxRestBytes,0))
        {
            qDebug()<<"error:send(),"<<UDT::getlasterror().getErrorMessage();
            this->m_bExitFlag=true;
            return;
        }
        qDebug("Tx rest okay\n");
    }
*/
}
