#ifndef TLSCONNECT_H
#define TLSCONNECT_H

#include "sslclasses.h"

class TLSConnect : public SSLConnect
{
private:
    std::string _stringPort;
    std::string _stringIpAddres;

    sockaddr_in _addres;

public:
    TLSConnect(SSLContext& sslContext, Socket sock, const sockaddr_in& addres);

    ~TLSConnect() {};

    const std::string& getIP_str();
    const std::string& getPort_str();

    unsigned int getIP();
    unsigned short getPort();

private:
    void getIpAndPort();
};


int operator >> (TLSConnect& connect, std::string& msg);
int operator << (TLSConnect& connect, const std::string& msg);


#endif // TLSCONNECT_H
