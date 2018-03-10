#include "mytcpserver.h"
#include "log.h"

void MyTcpServer::onConnect(std::shared_ptr<Connect<TCP>> connect)
{
    Log::Message("MyTcpServer", "New connect: " + connect->getStringIP());
    std::string clientMessage;
    if (ipIsLocked(connect->getIP()))
    {
        Log::Warning("MyTcpServer", "This IP(" + connect->getStringIP() + ") is locked!");

        auto timeNow = ::time(nullptr);
        *connect << "!"; // Сервер завершил обработку соединения.
        *connect >> clientMessage;
        *connect << "Your address has been blocked. Wait " + std::to_string(getBlockingTime(connect->getIP())) + "s.";
        return;
    }
    else
    {
        *connect << "+";
    }

    Log::Message("MyTcpServer", connect->getStringIP() + " - logs in");
    bool invalidPassword = true;
    for (int i = 0; i < 3; i++)
    {
        std::string userPassword;
        int bytes = *connect >> userPassword;
        if (bytes < 0) return;

        if (userPassword == _password)
        {
            Log::Warning("MyTcpServer", connect->getStringIP() + " - Correct password!");
            *connect << "+";

            invalidPassword = false;
            break;
        }

        if (i < 2)
        {
            Log::Warning("MyTcpServer", connect->getStringIP() + " - Wrong password!");
            *connect << "-";
        }
        else
        {
            lockAddres(connect->getIP());
            Log::Warning("MyTcpServer", connect->getStringIP() + " - Locked!");
            *connect << "!"; // Сервер завершил обработку соединения.
            *connect >> clientMessage;
            *connect << "Your address has been blocked. Wait " + std::to_string(_ipLockTime) + "s.";
            return;
        }
    }
}

void MyTcpServer::setIpLockTime(unsigned int seconds) { _ipLockTime = seconds; }
unsigned int MyTcpServer::getIpLockTime() { return _ipLockTime; }

bool MyTcpServer::ipIsLocked(unsigned int ip)
{
    _mutexForMap.lock();
    auto lockedAddresIterator = _mapOfLockedAddresses.find(ip);
    if (lockedAddresIterator == _mapOfLockedAddresses.end())
    {
        _mutexForMap.unlock();
        return false;
    }
    _mutexForMap.unlock();

    auto timeNow = ::time(nullptr);
    if (timeNow >= lockedAddresIterator->second.second)
    {
        unlockAddres(ip);
        return false;
    }

    return true;
}

void MyTcpServer::lockAddres(unsigned int ip)
{
    auto timeNow = ::time(nullptr);
    _mutexForMap.lock();
    _mapOfLockedAddresses[ip] = std::make_pair<long long int, long long int>(timeNow, timeNow + _ipLockTime);
    _mutexForMap.unlock();
}

void MyTcpServer::unlockAddres(unsigned int ip)
{
    _mutexForMap.lock();
    _mapOfLockedAddresses.erase(ip);
    _mutexForMap.unlock();
}

int MyTcpServer::getBlockingTime(unsigned int ip)
{
    int res = 0;
    auto timeNow = ::time(nullptr);
    _mutexForMap.lock();
    auto lockedAddresIterator = _mapOfLockedAddresses.find(ip);
    if (lockedAddresIterator != _mapOfLockedAddresses.end())
    {
        res = lockedAddresIterator->second.second - timeNow;
    }
    _mutexForMap.unlock();
    return res;
}
