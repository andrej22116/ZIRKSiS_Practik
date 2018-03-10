#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <mutex>
#include <map>
#include "tcpserver.h"

class MyTcpServer : public TcpServer
{
private:
    unsigned int _ipLockTime = 300;

    std::mutex _mutexForMap;
    std::map<unsigned int, std::pair<long long int, long long int>> _mapOfLockedAddresses;

    bool ipIsLocked(unsigned int ip);
    void lockAddres(unsigned int ip);
    void unlockAddres(unsigned int ip);
    int getBlockingTime(unsigned int ip);

    std::string _password;

public:
    MyTcpServer(unsigned short port, std::string password) : TcpServer(port), _password(password) {}
    ~MyTcpServer() {}

    void setIpLockTime(unsigned int seconds);
    unsigned int getIpLockTime();

protected:
    virtual void onConnect(std::shared_ptr<Connect<TCP>> connect) override;
};

#endif // MYTCPSERVER_H
