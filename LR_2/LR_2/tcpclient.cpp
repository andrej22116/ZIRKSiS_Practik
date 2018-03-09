#include "tcpclient.h"

bool TcpClient::initialize(const std::string serverIP)
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0)
    {
        return false;
    }

    _addres.sin_family = AF_INET;
    _addres.sin_port = htons(_port);
    if (inet_pton(AF_INET, serverIP.c_str(), &(_addres.sin_addr)) <= 0)
    {
        return false;
    }

    return true;
}

void TcpClient::start()
{
    typedef sockaddr* pSockaddr; // Для более читаемого приведения
    if(connect(_socket, pSockaddr(&_addres), sizeof(_addres)) < 0)
    {
        return;
    }

    auto connect = std::make_shared<Connect<TCP>>(_socket, _addres);
    onConnect(connect);
}
