#include "tlsserver.h"
#include <iostream>

TlsServer::TlsServer(unsigned short port) : _port(port), _sslContext(SSLContext::SSL_Server)
{
    _socket = createSocketTCP();
    if (!_socket)
    {
        throw nullptr;
    }

    _addres.sin_family = AF_INET;
    _addres.sin_port = htons(port);
    _addres.sin_addr.s_addr = htonl(INADDR_ANY);

    bool reuseAddr = true;
    ::setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr));

    typedef sockaddr* pSockaddr; // Для более читаемого приведения
    if(bind(_socket, pSockaddr(&_addres), sizeof(_addres)) < 0)
    {
        throw nullptr;
    }
}
TlsServer::~TlsServer(){ _socket.close(); }


void TlsServer::start()
{
    listen(_socket, 5);

    while (true)
    {
        try
        {
            sockaddr_in clientAddres;
            socklen_t sockLen = sizeof(clientAddres);
            typedef sockaddr* pSockaddr; // Для более читаемого приведения
            Socket clientSocket = accept(_socket, pSockaddr(&clientAddres), &sockLen);
            if (!clientSocket)
            {
                continue;
            }

            auto connect = std::make_shared<TLSConnect>(_sslContext, clientSocket, clientAddres);
            connect->accept();

            _listOfThreads.push_back(std::thread(&TlsServer::onConnectThread, this, connect));
        }
        catch(SSLException ex)
        {
            std::cerr << ex.what() << std::endl;
        }
        catch(...)
        {
            std::cerr << "Exception on accept!" << std::endl;
        }
    }
}


void TlsServer::onConnectThread(std::shared_ptr<TLSConnect> connect)
{
    try
    {
        onConnect(*connect);
    }
    catch(SSLException ex)
    {
        std::cerr << ex.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "Exception on connect!" << std::endl;
    }
}
