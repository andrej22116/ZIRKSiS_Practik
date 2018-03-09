#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include "connect.h"

class Client
{
protected:
    Socket _socket;
    unsigned short _port;
    sockaddr_in _addres;

public:
    Client(unsigned short port) : _port(port) {}
    virtual ~Client() { _socket.close(); }

    /* =================================== *
     * Инициализация клиента - обязательно *
     * =================================== */
    virtual bool initialize(std::string serverIP) = 0;


    /* =============================== *
     * Запуск клиента - не обязательно *
     * =============================== */
    virtual void start() {}
};

#endif // CLIENT_H
