#ifndef SOCKET_H
#define SOCKET_H

#include "socketapi.h"

struct Socket
{
    Socket() {}
#ifdef __linux__
    int socket;
    Socket(int socket) : socket(socket) {}
    operator int() { return this->socket; }
    Socket& operator = (int sock) { socket = sock; return *this; }
    void close() { ::close(socket); }
#elif _WIN32
    SOCKET socket;
    Socket(SOCKET socket) : socket(socket) {}
    operator SOCKET() { return this->socket; }
    Socket& operator = (SOCKET sock) { socket = sock; return *this; }
    void close() { ::closesocket(socket); }
#endif
    Socket& operator = (const Socket& sock) { this->socket = sock.socket; return *this; }
};

#endif // SOCKET_H
