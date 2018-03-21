#ifndef TLSSERVER_H
#define TLSSERVER_H

#include <list>
#include <thread>

#include "sslclasses.h"

class TlsServer
{
private:
    std::list<std::thread> _listOfThreads;

    Socket _socket;
    unsigned short _port;
    sockaddr_in _addres;

public:
    TlsServer(unsigned short port) : _port(port) {}

    bool start();

    virtual void onConnect() = 0;

};

#endif // TLSSERVER_H
