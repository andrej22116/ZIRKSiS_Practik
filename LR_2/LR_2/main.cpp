#include <iostream>

#include "mytcpserver.h"
#include "mytcpclient.h"
#include "log.h"

int main()
{
    std::cout << "Enter [client/server]:> ";
    std::string program;
    std::cin >> program;
    std::cin.ignore();

    if (program == "server")
    {
        Log::Message("PROGRAMM", "Start server!");
        MyTcpServer server(25565, "123640");
        if (!server.initialize())
        {
            Log::Error("PROGRAMM", "Problem in initialize server!");
            return -1;
        }
        server.setIpLockTime(60);
        server.start();
    }
    else if (program == "client")
    {
        Log::Message("PROGRAMM", "Start client!");
        MyTcpClient client(25565);
        if (!client.initialize("127.0.0.1"))
        {
            Log::Error("PROGRAMM", "Problem in initialize client!");
            return -1;
        }
        client.start();
    }
    else
    {
        Log::Error("PROGRAMM", "Invalid value!");
        return -1;
    }
    return 0;
}
