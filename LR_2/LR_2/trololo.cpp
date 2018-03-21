#include <iostream>
#include <memory>

#include "sslclasses.h"
#include "log.h"

bool server(unsigned short port);
bool client(std::string serverAddres, unsigned short port);

int main()
{
    std::cout << "Enter [client/server]:> ";
    std::string program;
    std::cin >> program;
    std::cin.ignore();

    if (program == "server")
    {
        Log::Message("PROGRAMM", "Start server!");
        if (!server(25565))
        {
            Log::Error("PROGRAMM", "Error in server work!");
        }
    }
    else if (program == "client")
    {
        Log::Message("PROGRAMM", "Start client!");
        if (!client("127.0.0.1", 25565))
        {
            Log::Error("PROGRAMM", "Error in client work!");
        }
    }
    else
    {
        Log::Error("PROGRAMM", "Invalid value!");
        return -1;
    }
    return 0;
}

bool server(unsigned short port)
{
    SSLInitializer::initOpenSSL();
    SSLContext sslContext(SSLContext::SSL_Server/*, "cert.pem", "key.pem"*/);

    AutoSocket serverSocket = createSocketTCP();
    if (!serverSocket.socket)
    {
        return false;
    }

    sockaddr_in serverAddres;
    serverAddres.sin_family = AF_INET;
    serverAddres.sin_port = htons(port);
    serverAddres.sin_addr.s_addr = htonl(INADDR_ANY);

    bool reuseAddr = true;
    ::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr));

    typedef sockaddr* pSockaddr; // Для более читаемого приведения
    if(bind(serverSocket, pSockaddr(&serverAddres), sizeof(serverAddres)) < 0)
    {
        return false;
    }

    listen(serverSocket, 5);

    while (true)
    {
        Log::Message("Server", "Whait connect...");
        try
        {
            sockaddr_in clientAddres;
            socklen_t sockLen = sizeof(clientAddres);
            AutoSocket clientSocket = accept(serverSocket, pSockaddr(&clientAddres), &sockLen);
            if (!clientSocket)
            {
                return false;
            }

            SSLConnect sslConnect(sslContext, clientSocket);
            sslConnect.accept();

            Log::Message("Server", "Whait client message...");

            std::string clientMessage;
            sslConnect >> clientMessage;

            Log::Message("Server", "Cleint sended: \"" + clientMessage + "\"");
            Log::Message("Server", "Try send message to client...");

            sslConnect << "Hello, client!";

            Log::Message("Server", "Message was sended!");
        }
        catch(SSLException ex)
        {
            Log::Error("Server", ex.what());
        }
    }
    Log::Message("Server", "Stop server!");

    return true;
}


bool client(std::string serverAddres, unsigned short port)
{
    SSLInitializer::initOpenSSL();
    SSLContext sslContext(SSLContext::SSL_Client);

    AutoSocket clientSocket = createSocketTCP();
    if (!clientSocket)
    {
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, serverAddres.c_str(), &(serverAddr.sin_addr)) <= 0)
    {
        return false;
    }

    typedef sockaddr* pSockaddr; // Для более читаемого приведения
    if(connect(clientSocket, pSockaddr(&serverAddr), sizeof(serverAddr)) != 0)
    {
        return false;
    }


    try
    {
        Log::Message("Cleint", "Try send message to server...");

        SSLConnect sslConnect(sslContext, clientSocket);
        sslConnect.connect();
        sslConnect << "Hello, server!";

        Log::Message("Cleint", "Message was sended!");
        Log::Message("Cleint", "Whait server message...");

        std::string serverMessage;
        sslConnect >> serverMessage;

        Log::Message("Cleint", "Server sended: \"" + serverMessage + "\"");


        Log::Message("Client", "Sertificate --> Subject name:" + sslConnect.getSubjectName());
        Log::Message("Client", "Sertificate --> Issuer name:" + sslConnect.getIssuerName());
    }
    catch(SSLException ex)
    {
        Log::Error("Client", ex.what());
    }

    return true;
}
