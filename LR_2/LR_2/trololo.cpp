#include <iostream>
#include <memory>

#include "mytlsserver.h"
#include "mytlsclient.h"
#include "log.h"

int main()
{
    std::cout << "Enter [client/server]:> ";
    std::string program;
    std::cin >> program;
    std::cin.ignore();

    SSLInitializer::initOpenSSL();
    if (program == "server")
    {
        Log::Message("PROGRAMM", "Start server!");
        MyTlsServer server(25565, "parol");
        server.setIpLockTime(30);
        server.start();
    }
    else if (program == "client")
    {
        Log::Message("PROGRAMM", "Start client!");
        MyTlsClient client("127.0.0.1", 25565);
        client.start();
    }
    else
    {
        Log::Error("PROGRAMM", "Invalid value!");
        return -1;
    }
    return 0;
}
