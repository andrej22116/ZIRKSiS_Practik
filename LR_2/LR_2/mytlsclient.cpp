#include "mytlsclient.h"
#include "log.h"

void MyTlsClient::onConnect(TLSConnect& connect)
{
    std::string serverMessage;
    int bytes = connect >> serverMessage;

    if (serverMessage == "+")
    {
        Log::Message("MyTlsClient", "Connected!");
    }
    else if (serverMessage == "!")
    {
        connect << "WTF?";
        connect >> serverMessage;
        Log::Warning("MyTlsClient", serverMessage);
        return;
    }

    while (true)
    {
        std::cout << "Enter password:> ";
        std::string password;
        std::getline(std::cin, password);
        if (password == "")
        {
            continue;
        }

        connect << password;
        bytes = connect >> serverMessage;

        if (serverMessage == "!")
        {
            connect << "WTF?";
            connect >> serverMessage;
            Log::Warning("MyTlsClient", serverMessage);
            return;
        }
        else if (serverMessage == "-")
        {
            Log::Error("MyTlsClient", "Invalid password!");
        }
        else if (serverMessage == "+")
        {
            Log::Message("MyTlsClient", "Correct password!");
            break;
        }
    }
}
