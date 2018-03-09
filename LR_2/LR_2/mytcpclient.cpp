#include "mytcpclient.h"
#include "log.h"

void MyTcpClient::onConnect(std::shared_ptr<Connect<TCP>> connect)
{
    for (int i = 0; i < 3; i++)
    {
        std::cout << "Enter message:> ";

        std::string msg;
        std::getline(std::cin, msg);

        *connect << msg;
    }
}
