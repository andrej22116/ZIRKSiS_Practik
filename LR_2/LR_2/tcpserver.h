#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <list>
#include <memory>
#include <thread>

#include "server.h"
#include "connect.h"

class TcpServer : public Server
{
private:
    bool initialized = false;
    std::list<std::shared_ptr<std::thread>> _threads;

public:
    TcpServer(unsigned short port) : Server(port){}
    ~TcpServer();

    virtual bool initialize() final;
    virtual void start() final;
    virtual void stop() final;

protected:
    virtual void onConnect(std::shared_ptr<Connect<TCP>> connect) = 0;

private:
    std::shared_ptr<Connect<TCP>> accept();
};

#endif // TCPSERVER_H
