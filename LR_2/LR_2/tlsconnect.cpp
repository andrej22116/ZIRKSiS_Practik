#include "tlsconnect.h"

TLSConnect::TLSConnect(SSLContext& sslContext, Socket sock, const sockaddr_in& addres)
    : _addres(addres), SSLConnect(sslContext, sock)
{
    getIpAndPort();
}

const std::string& TLSConnect::getIP_str()
{
    return _stringIpAddres;
}
const std::string& TLSConnect::getPort_str()
{
    return _stringPort;
}

unsigned int TLSConnect::getIP()
{
    return _addres.sin_addr.s_addr;
}
unsigned short TLSConnect::getPort()
{
    return _addres.sin_port;
}

void TLSConnect::getIpAndPort()
{
    _stringPort.resize(16);
    _stringIpAddres.resize(32);

    _stringIpAddres = inet_ntoa(_addres.sin_addr);
    _stringPort = std::to_string(ntohs(_addres.sin_port));
}


int operator >> (TLSConnect& connect, std::string& msg)
{
    auto res = connect.revc();
    msg = res.second;
    return res.first;
}
int operator << (TLSConnect& connect, const std::string& msg)
{
    return connect.send(msg);
}
