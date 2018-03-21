#include "tlsconnect.h"
/*
TLSConnect::TLSConnect(Socket sock, const sockaddr_in& addres, SSL_CTX* ssl_context)
    : _socket(sock), _addres(addres)
{
    getIpAndPort();
    initTLS(ssl_context);

}

TLSConnect::TLSConnect(Socket sock, const sockaddr& addres, SSL_CTX* ssl_context)
    : _socket(sock)//, _addres(static_cast<sockaddr_in>(addres))
{
    getIpAndPort();
    initTLS(ssl_context);
}

TLSConnect::~TLSConnect()
{
    SSL_free(_ssl);
    _socket.close();
}

const std::string& TLSConnect::getStringIP()
{
    return _stringIpAddres;
}
const std::string& TLSConnect::getStringPort()
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

int TLSConnect::send(const std::string& msg)
{
    return ::SSL_write(_ssl, msg.c_str(), sizeof(char) * msg.length());
}
std::pair<int, std::string> TLSConnect::revc()
{

    const int bufSize = 64;
    std::pair<int, std::string> res = {0, ""};
    while (true)
    {
        int dataSize = 0;
        char buf[bufSize + 1] = { 0 };

        dataSize = SSL_read(_ssl, buf, sizeof(char) * bufSize);

        res.first += dataSize;
        res.second += buf;
        if (dataSize < bufSize)
        {
            return res;
        }
    }
}

void TLSConnect::getIpAndPort()
{
    _stringPort.resize(16);
    _stringIpAddres.resize(32);

    _stringIpAddres = inet_ntoa(_addres.sin_addr);
    _stringPort = std::to_string(ntohs(_addres.sin_port));
}


void TLSConnect::initTLS(SSL_CTX* ssl_context)
{
    _ssl = SSL_new(ssl_context);
    SSL_set_fd(_ssl, _socket);
}
*/
