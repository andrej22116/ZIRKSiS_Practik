#ifndef SOCKET_H
#define SOCKET_H

#include <memory>

#include "socketapi.h"

struct Socket
{
    Socket() {}
#ifdef __linux__
    int socket = 0;
    Socket(int socket) : socket(socket) {}
    inline operator int() { return this->socket; }
    inline Socket& operator = (int sock) { socket = sock; return *this; }
    inline void close() { ::close(socket); }
#elif _WIN32
    SOCKET socket = 0;
    Socket(SOCKET socket) : socket(socket) {}
    inline operator SOCKET() { return this->socket; }
    inline Socket& operator = (SOCKET sock) { socket = sock; return *this; }
    inline void close() { ::closesocket(socket); }
#endif
    Socket& operator = (const Socket& sock) { this->socket = sock.socket; return *this; }
};


struct AutoSocket
{
    Socket socket;
    AutoSocket() {}
    AutoSocket(Socket socket) : socket(socket) {}
    AutoSocket& operator = (const Socket& sock) { socket = sock.socket; return *this; }
    ~AutoSocket() { socket.close(); }

#ifdef __linux__
    inline AutoSocket& operator = (int sock) { socket = Socket(sock); return *this; }
    inline AutoSocket(int socket) : socket(socket) {}
    inline operator int() { return this->socket.socket; }
#elif _WIN32
    inline AutoSocket& operator = (SOCKET sock) { socket = Socket(sock); return *this; }
    inline AutoSocket(SOCKET socket) : socket(socket) {}
    inline operator SOCKET() { return this->socket.socket; }
#endif
    inline operator Socket() { return socket; }

    inline AutoSocket& operator = (const AutoSocket& sock) = delete;
};

inline Socket createSocketTCP() { return Socket(::socket(AF_INET, SOCK_STREAM, 0)); }
inline Socket createSocketUDP() { return Socket(::socket(AF_INET, SOCK_DGRAM, 0)); }

#endif // SOCKET_H
