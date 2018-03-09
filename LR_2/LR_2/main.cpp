#include <iostream>
#include "socketapi.h"
//#include <server.h>


#include "connect.h"
#include "mytcpserver.h"
#include "log.h"

void startServer(std::string password, unsigned int port);
void startClient(const char* serverAddres, unsigned int port);

std::string sockaddrToIp(sockaddr_in addres);

int main()
{
    std::cout << "Enter [client/server]:> ";
    std::string program;
    std::cin >> program;
    if (program == "server")
    {
        Log::Message("PROGRAMM", "Start server!");
        MyTcpServer server(25565);
        if (!server.initialize())
        {
            Log::Error("PROGRAMM", "Problem in initialize server!");
            return -1;
        }
        server.start();
    }
    else if (program == "client")
    {
        Log::Message("PROGRAMM", "Start client!");
        startClient("127.0.0.1", 25565);
    }
    else
    {
        Log::Error("PROGRAMM", "Invalid value!");
        return -1;
    }
    return 0;
}

void startServer(std::string password, unsigned int port)
{
    std::cout << "[Server][Message]:> Create socket!" << std::endl;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cout << "[Server][Error]:> Get socket error!" << std::endl;
        return;
    }

    sockaddr_in addres;
    addres.sin_family = AF_INET;
    addres.sin_port = htons(port);
    addres.sin_addr.s_addr = htonl(INADDR_ANY);

    std::cout << "[Server][Message]:> Set socket option [SO_REUSEADDR = true]" << std::endl;
    {
        bool reuseAddr = true;
        ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr));
    }

    typedef sockaddr* pSockaddr; // Для более читаемого приведения
    std::cout << "[Server][Message]:> Bind socket!" << std::endl;
    {
        if(bind(sock, pSockaddr(&addres), sizeof(addres)) < 0)
        {
            std::cout << "[Server][Error]:> Bind socket error!" << std::endl;
            close(sock);
            return;
        }

        listen(sock, 5);
    }

    std::cout << "[Server][Message]:> Listening socket!" << std::endl;
    while (1)
    {
        socklen_t len;
        sockaddr_in clientAddres;
        int connect = accept(sock, pSockaddr(&clientAddres), &len);
        if (connect <= 0)
        {
            std::cout << "[Server][Error]:> Connect accept error!" << std::endl;
            return;
        }

        Connect client(connect, clientAddres);
        std::cout << "LOG::> New connect! Addres: " << client.getIP() << ":" << client.getPort() << std::endl;
        std::string msg;
        client >> msg;
        //recvString(msg, connect);
        std::cout << "[Server][Message]:> Client sended: \"" << msg << "\"" << std::endl;

        std::cout << "[Server][Message]:> Close connect!" << std::endl;
        //close(connect);
    }


    std::cout << "[Server][Message]:> Close socket!" << std::endl;
    close(sock);
}

void startClient(const char* serverAddres, unsigned int port)
{
    std::cout << "[Client][Message]:> Create socket!" << std::endl;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        std::cout << "[Client][Error]:> Get socket error!" << std::endl;
        return;
    }

    sockaddr_in addres;
    addres.sin_family = AF_INET;
    addres.sin_port = htons(port);
    if (inet_pton(AF_INET, serverAddres, &(addres.sin_addr)) <= 0)
    {
        std::cout << "[Client][Error]:> Convert server IP-addres error!" << std::endl;
        return;
    }

    Connect serv(sock, addres);
    std::cout << "LOG::> Server addres: " << serv.getIP() << ":" << serv.getPort() << std::endl;

    std::cout << "[Client][Message]:> Connecting to server!" << std::endl;
    typedef sockaddr* pSockaddr; // Для более читаемого приведения
    if(connect(sock, pSockaddr(&addres), sizeof(addres)) < 0)
    {
        std::cout << "[Client][Error]:> Connect error!" << std::endl;
        close(sock);
        return;
    }

    std::cout << "\nCommands:> \n"
              << "login:> autorization in server;\n"
              << "exit:> exit from programm." << std::endl;
    while(true)
    {
        std::cout << "Enter command:> ";
        std::string command;
        std::cin >> command;
        std::cin.ignore();

        if (command == "exit") { break; }
        else if (command == "login")
        {
            std::cout << "Enter paswword:> ";
            std::string password;
            std::getline(std::cin, password);

            //sendString(password, sock);
            serv << password;
            //send(sock, password.c_str(), sizeof(char) * password.length(), 0);

        }
    }


    std::cout << "[Client][Message]:> Close socket!" << std::endl;
    close(sock);
}
