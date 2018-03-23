#ifndef MYTLSCLIENT_H
#define MYTLSCLIENT_H

#include "tlsclient.h"

class MyTlsClient : public TlsClient
{
public:
    MyTlsClient(std::string serverAddres, unsigned short port) : TlsClient(serverAddres, port) {}

    virtual void onConnect(TLSConnect& connect) override;
};

#endif // MYTLSCLIENT_H
