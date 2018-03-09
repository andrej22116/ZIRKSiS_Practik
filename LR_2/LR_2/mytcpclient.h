#ifndef MYTCPCLIENT_H
#define MYTCPCLIENT_H

#include "tcpclient.h"

class MyTcpClient : public TcpClient
{
public:
    MyTcpClient(unsigned short port) : TcpClient(port) {}

protected:
    virtual void onConnect(std::shared_ptr<Connect<TCP>> connect) override;
};

#endif // MYTCPCLIENT_H
