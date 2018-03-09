#include "tcpserver.h"

TcpServer::~TcpServer()
{
    _socket.close();
}


bool TcpServer::initialize()
{
    if (initialized) return false;

    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0) return false;

    _addres.sin_family = AF_INET;
    _addres.sin_port = htons(_port);
    _addres.sin_addr.s_addr = htonl(INADDR_ANY);

    bool reuseAddr = true;
    ::setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr));

    typedef sockaddr* pSockaddr; // Для более читаемого приведения
    if(bind(_socket, pSockaddr(&_addres), sizeof(_addres)) < 0)
    {
        close(_socket);
        return false;
    }

    listen(_socket, 5);

    initialized = true;
    return true;
}

void TcpServer::start()
{
    while(true)
    {
        auto connect = accept();
        if (connect == nullptr) {
            break;
        }
        _threads.push_back(std::make_shared<std::thread>(&TcpServer::onConnect, this, connect));
    }
}

void TcpServer::stop()
{
    _socket.close();
    for (auto&& thread : _threads)
    {
        thread->join();
    }
}


std::shared_ptr<Connect<TCP>> TcpServer::accept()
{
    sockaddr_in clientAddres;

    socklen_t addresLen = 0;
    typedef sockaddr* pSockaddr; // Для более читаемого приведения
    Socket socket = ::accept(_socket, pSockaddr(&_addres), &addresLen);
    if (socket < 0) {
        std::cout << "Error in Accept!" << std::endl;
        return nullptr;
    }

    return std::make_shared<Connect<TCP>>(socket, clientAddres);
}
