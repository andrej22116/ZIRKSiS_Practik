#ifndef TLSCONNECT_H
#define TLSCONNECT_H

//#include <openssl/ssl.h>
//#include "socket.h"
//#include "log.h"
/*
class TLSConnect
{
private:
    Socket _socket;

    std::string _stringPort;
    std::string _stringIpAddres;

    sockaddr_in _addres;

    SSL* _ssl;

public:
    TLSConnect(Socket sock, const sockaddr_in& addres, SSL_CTX* ssl_context);
    TLSConnect(Socket sock, const sockaddr& addres, SSL_CTX* ssl_context);

    ~TLSConnect();

    const std::string& getStringIP();
    const std::string& getStringPort();

    unsigned int getIP();
    unsigned short getPort();

    int send(const std::string& msg);
    std::pair<int, std::string> revc();

private:
    void getIpAndPort();
    void initTLS(SSL_CTX* ssl_context);
};


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
*/

#endif // TLSCONNECT_H
