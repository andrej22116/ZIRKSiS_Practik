#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include "tcpserver.h"

class MyTcpServer : public TcpServer
{
private:

public:
    MyTcpServer(unsigned short port) : TcpServer(port) {}
    ~MyTcpServer() {}

protected:
    virtual void onConnect(std::shared_ptr<Connect<TCP>> connect) override;
};

#endif // MYTCPSERVER_H
