#ifndef MYTLSSERVER_H
#define MYTLSSERVER_H

#include <map>
#include <mutex>
#include "tlsserver.h"


class MyTlsServer : public TlsServer
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
    MyTlsServer(unsigned short port, std::string password) : TlsServer(port), _password(password) {}

    void setIpLockTime(unsigned int seconds);
    unsigned int getIpLockTime();

protected:
    virtual void onConnect(TLSConnect& connect) override;
};

#endif // MYTLSSERVER_H
