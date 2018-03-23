#ifndef MYTLSSERVER_H
#define MYTLSSERVER_H

#include "tlsserver.h"


class MyTlsServer : public TlsServer
{
public:
    MyTlsServer(unsigned short port) : TlsServer(port) {}

    virtual void onConnect(TLSConnect& connect) override;
};

#endif // MYTLSSERVER_H
