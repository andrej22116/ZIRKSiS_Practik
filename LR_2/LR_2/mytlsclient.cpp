#include "mytlsclient.h"
#include "log.h"

void MyTlsClient::onConnect(TLSConnect& connect)
{
    Log::Message("Cleint", "Try send message to server...");

    connect.connect();
    connect << "Hello, server!";

    Log::Message("Cleint", "Message was sended!");
    Log::Message("Cleint", "Whait server message...");

    std::string serverMessage;
    connect >> serverMessage;

    Log::Message("Cleint", "Server sended: \"" + serverMessage + "\"");


    Log::Message("Client", "Sertificate --> Subject name:" + connect.getSubjectName());
    Log::Message("Client", "Sertificate --> Issuer name:" + connect.getIssuerName());
}
