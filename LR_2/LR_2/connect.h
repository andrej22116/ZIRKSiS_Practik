#ifndef CONNECT_H
#define CONNECT_H

#include <iostream>
#include <string.h>
#include <memory>
#include "socketapi.h"
#include "socket.h"

enum ConnectType {
    TCP,
    UDP
};

template<ConnectType type = TCP>
class Connect
{
private:
    Socket _socket;

    std::string _port;
    std::string _ipAddres;

    sockaddr_in _addres;

public:
    Connect(Socket sock, const sockaddr_in& addres) : _socket(sock), _addres(addres)
    {
        getIpAndPort();
    }
    Connect(Socket sock, const sockaddr& addres) : _socket(sock), _addres(static_cast<sockaddr_in>(addres))
    {
        getIpAndPort();
    }

    ~Connect()
    {
        _socket.close();
    }

    const std::string& getIP()
    {
        return _ipAddres;
    }
    const std::string& getPort()
    {
        return _port;
    }

    int send(const std::string& msg)
    {
        if (type == TCP)
        {
            return ::send(_socket, msg.c_str(), sizeof(char) * msg.length(), 0);
        }
        else if (type == UDP)
        {
            typedef sockaddr* p_sockaddr;
            return ::sendto(_socket, msg.c_str(), sizeof(char) * msg.length(), 0, p_sockaddr(&_addres), sizeof(_addres));
        }
    }
    std::pair<int, std::string> revc()
    {
        const int bufSize = 64;
        std::pair<int, std::string> res = {0, ""};
        while (true)
        {
            int dataSize = 0;
            char buf[bufSize + 1] = { 0 };

            if (type == TCP)
            {
                dataSize = ::recv(_socket, buf, sizeof(char) * bufSize, 0);
            }
            else if (type == UDP)
            {
                socklen_t sizeOfAddres = sizeof(_addres);
                typedef sockaddr* p_sockaddr;
                dataSize = ::recvfrom(_socket, buf, sizeof(char) * bufSize, 0, p_sockaddr(&_addres), &sizeOfAddres);
            }

            res.first += dataSize;
            res.second += buf;
            if (dataSize < bufSize)
            {
                return res;
            }
        }
    }

private:
    void getIpAndPort()
    {
        _port.resize(16);
        _ipAddres.resize(32);

        _ipAddres = inet_ntoa(_addres.sin_addr);
        _port = std::to_string(ntohs(_addres.sin_port));
    }
};

template<ConnectType type>
int operator >> (Connect<type>& connect, std::string& msg)
{
    auto res = connect.revc();
    msg = res.second;
    return res.first;
}
template<ConnectType type>
int operator << (Connect<type>& connect, const std::string& msg)
{
    return connect.send(msg);
}

#endif // CONNECT_H
