#include "mytcpclient.h"
#include "log.h"

void MyTcpClient::onConnect(std::shared_ptr<Connect<TCP>> connect)
{
    std::string serverMessage;
    int bytes = *connect >> serverMessage;
    if (bytes < 0)
    {
        Log::Error("MyTcpClient", "Disconected from server!");
        return;
    }
    else if (serverMessage == "+")
    {
        Log::Message("MyTcpClient", "Connected!");
    }
    else if (serverMessage == "!")
    {
        *connect << "WTF?";
        *connect >> serverMessage;
        Log::Warning("MyTcpClient", serverMessage);
        return;
    }
    else
    {
        Log::Error("MyTcpClient", "Error in connect!");
        return;
    }

    while (true)
    {
        std::cout << "Enter password:> ";
        std::string password;
        std::getline(std::cin, password);

        *connect << password;
        bytes = *connect >> serverMessage;

        if (bytes < 0)
        {
            Log::Error("MyTcpClient", "Disconected from server!");
            return;
        }
        else if (serverMessage == "!")
        {
            *connect << "WTF?";
            *connect >> serverMessage;
            Log::Warning("MyTcpClient", serverMessage);
            return;
        }
        else if (serverMessage == "-")
        {
            Log::Error("MyTcpClient", "Invalid password!");
        }
        else if (serverMessage == "+")
        {
            Log::Message("MyTcpClient", "Correct password!");
            break;
        }
    }
}
