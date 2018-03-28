#include "tlsclient.h"

TlsClient::TlsClient(std::string serverAddres, unsigned short port) : _port(port), _sslContext(SSLContext::SSL_Client)
{

    _socket = createSocketTCP();
    if (!_socket)
    {
        throw nullptr;
    }

    _addres.sin_family = AF_INET;
    _addres.sin_port = htons(port);
    if (inet_pton(AF_INET, serverAddres.c_str(), &(_addres.sin_addr)) <= 0)
    {
        throw nullptr;
    }
}

TlsClient::~TlsClient(){ _socket.close(); }

void TlsClient::start()
{
    typedef sockaddr* pSockaddr; // Для более читаемого приведения
    if(connect(_socket, pSockaddr(&_addres), sizeof(_addres)) != 0)
    {
        throw nullptr;
    }

    TLSConnect connect(_sslContext, _socket, _addres);
    connect.connect();
    onConnect(connect);
}
