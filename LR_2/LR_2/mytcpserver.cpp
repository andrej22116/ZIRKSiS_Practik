#include "mytcpserver.h"
#include "log.h"

void MyTcpServer::onConnect(std::shared_ptr<Connect<TCP>> connect)
{
    for (int i = 0; i < 3; i++)
    {
        auto[bts, msg] = connect->revc();
        Log::Message("MyServer", "Bits: " + std::to_string(bts) + ", Msg: \"" + msg + "\"");
        if (msg == "shootdown")
        {
            std::thread([&](){ this->stop(); });
        }
    }
}
