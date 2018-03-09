#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "client.h"

class TcpClient : public Client
{
public:
    TcpClient(unsigned short port) : Client(port) {}

    virtual bool initialize(const std::string serverIP) override;

    virtual void start() override;

protected:
    virtual void onConnect(std::shared_ptr<Connect<TCP>> connect) = 0;
};

#endif // TCPCLIENT_H
