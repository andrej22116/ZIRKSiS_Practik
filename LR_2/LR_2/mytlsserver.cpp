#include "mytlsserver.h"
#include "log.h"

void MyTlsServer::onConnect(TLSConnect& connect)
{
    std::string clientIP = connect.getIP_str();

    Log::Message("MyTlsServer", "New connect: " + clientIP);
    std::string clientMessage;
    if (ipIsLocked(connect.getIP()))
    {
        Log::Warning("MyTlsServer", "This IP(" + clientIP + ") is locked!");

        connect << "!";
        connect >> clientMessage;
        connect << "Your address has been blocked. Wait " + std::to_string(getBlockingTime(connect.getIP())) + "s.";
        return;
    }
    else
    {
        connect << "+";
    }

    Log::Message("MyTlsServer", clientIP + " - logs in");
    for (int i = 0; i < 3; i++)
    {
        std::string userPassword;
        int bytes = connect >> userPassword;
        if (bytes < 0) return;

        if (userPassword == _password)
        {
            Log::Warning("MyTlsServer", clientIP + " - Correct password!");
            connect << "+";

            break;
        }

        if (i < 2)
        {
            Log::Warning("MyTlsServer", clientIP + " - Wrong password!");
            connect << "-";
        }
        else
        {
            lockAddres(connect.getIP());
            Log::Warning("MyTlsServer", clientIP + " - Locked!");
            connect << "!";
            connect >> clientMessage;
            connect << "Your address has been blocked. Wait " + std::to_string(_ipLockTime) + "s.";
            return;
        }
    }
}



void MyTlsServer::setIpLockTime(unsigned int seconds) { _ipLockTime = seconds; }
unsigned int MyTlsServer::getIpLockTime() { return _ipLockTime; }

bool MyTlsServer::ipIsLocked(unsigned int ip)
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

void MyTlsServer::lockAddres(unsigned int ip)
{
    auto timeNow = ::time(nullptr);
    _mutexForMap.lock();
    _mapOfLockedAddresses[ip] = std::make_pair<long long int, long long int>(timeNow, timeNow + _ipLockTime);
    _mutexForMap.unlock();
}

void MyTlsServer::unlockAddres(unsigned int ip)
{
    _mutexForMap.lock();
    _mapOfLockedAddresses.erase(ip);
    _mutexForMap.unlock();
}

int MyTlsServer::getBlockingTime(unsigned int ip)
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
