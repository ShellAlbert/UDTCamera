#include <QCoreApplication>
#include <udt4/src/udt.h>
#include <zservicethread.h>
#include <unistd.h>
#include <string.h>
#include <QDebug>
//#define USE_TCP 1
int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);

    // Automatically start up and clean up UDT module.
    UDT::startup();

    //udt tx demo.
    addrinfo hints;
    addrinfo* res;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
#ifdef USE_TCP
    hints.ai_socktype = SOCK_STREAM;
#else
    hints.ai_socktype = SOCK_DGRAM;
#endif

    std::string service("9000");
    if (0!=getaddrinfo(NULL, service.c_str(), &hints, &res))
    {
        qDebug()<<"illegal port number or port is busy.";
        UDT::cleanup();
        return 0;
    }

    UDTSOCKET serv=UDT::socket(res->ai_family,res->ai_socktype,res->ai_protocol);

#ifdef WIN32
   int mss=1052;
   UDT::setsockopt(serv,0,UDT_MSS,&mss,sizeof(int));
#endif

    // UDT Options
    //UDT::setsockopt(serv, 0, UDT_CC, new CCCFactory<CUDPBlast>, sizeof(CCCFactory<CUDPBlast>));
    //UDT::setsockopt(serv, 0, UDT_MSS, new int(9000), sizeof(int));
    //UDT::setsockopt(serv, 0, UDT_RCVBUF, new int(10000000), sizeof(int));
    //UDT::setsockopt(serv, 0, UDP_RCVBUF, new int(10000000), sizeof(int));

    if(UDT::ERROR==UDT::bind(serv,res->ai_addr,res->ai_addrlen))
    {
        qDebug()<<"error:"<<UDT::getlasterror().getErrorMessage();
        return -1;
    }

    freeaddrinfo(res);

    qDebug()<<"Server is ready at port:"<<service.c_str();
    if (UDT::ERROR==UDT::listen(serv,10))
    {
        qDebug()<<"error:listen(),"<<UDT::getlasterror().getErrorMessage();
        return -1;
    }

    sockaddr_storage clientaddr;
    int addrlen = sizeof(clientaddr);

    UDTSOCKET sockClient;

    while (1)
    {
        if (UDT::INVALID_SOCK==(sockClient=UDT::accept(serv,(sockaddr*)&clientaddr,&addrlen)))
        {
            qDebug()<<"error:accept(),"<<UDT::getlasterror().getErrorMessage();
            return -1;
        }

        char clienthost[NI_MAXHOST];
        char clientservice[NI_MAXSERV];
        getnameinfo((sockaddr *)&clientaddr,addrlen,clienthost,sizeof(clienthost),clientservice,sizeof(clientservice), NI_NUMERICHOST|NI_NUMERICSERV);
        qDebug()<<"new connection:"<<clienthost<<":"<<clientservice;
        //CreateThread(NULL, 0, recvdata, new UDTSOCKET(sockClient), 0, NULL);
        //when client connects send captured images to it.
        ZServiceThread *newService=new ZServiceThread(sockClient);
        newService->ZStartThread();
        QObject::connect(newService,SIGNAL(finished()),newService,SLOT(deleteLater()));
    }
    UDT::close(serv);
    //UDT::cleanup();
    return a.exec();
}
