#include "mytlsserver.h"
#include "log.h"

void MyTlsServer::onConnect(TLSConnect& connect)
{
    Log::Message("Server", "Whait client message...");

    std::string clientMessage;
    connect >> clientMessage;

    Log::Message("Server", "Cleint sended: \"" + clientMessage + "\"");
    Log::Message("Server", "Try send message to client...");

    connect << "Hello, client!";

    Log::Message("Server", "Message was sended!");
}
