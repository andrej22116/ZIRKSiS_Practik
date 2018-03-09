#ifndef SERVER_H
#define SERVER_H

#include "socketapi.h"
#include "socket.h"

class Server
{    
protected:
    Socket _socket;
    unsigned short _port;
    sockaddr_in _addres;

public:
    Server(unsigned short port) : _port(port) {}
    virtual ~Server() {}

    /* =================================== *
     * Инициализация сервера - обязательно *
     * =================================== */
    virtual bool initialize() = 0;


    /* ======================================================== *
     * Работа сервера определяется субклассами - не обязательно *
     * ======================================================== */
    virtual void start() {}
    virtual void stop() {}
};

#endif // SERVER_H
