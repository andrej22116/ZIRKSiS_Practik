#ifndef TLSSERVER_H
#define TLSSERVER_H

#include <list>
#include <memory>
#include <thread>

#include "tlsconnect.h"

class TlsServer
{
private:
    std::list<std::thread> _listOfThreads;

    Socket _socket;
    unsigned short _port;
    sockaddr_in _addres;

    SSLContext _sslContext;

public:
    TlsServer(unsigned short port);
    ~TlsServer();

    void start();

    virtual void onConnect(TLSConnect& connect) = 0;

    void onConnectThread(std::shared_ptr<TLSConnect> connect);
};


#endif // TLSSERVER_H
