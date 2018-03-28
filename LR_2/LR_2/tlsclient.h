#ifndef TLSCLIENT_H
#define TLSCLIENT_H

#include "tlsconnect.h"

class TlsClient
{
private:

    Socket _socket;
    unsigned short _port;
    sockaddr_in _addres;

    SSLContext _sslContext;

public:
    TlsClient(std::string serverAddres, unsigned short port);
    ~TlsClient();

    void start();

protected:
    virtual void onConnect(TLSConnect& connect) = 0;
};

#endif // TLSCLIENT_H
