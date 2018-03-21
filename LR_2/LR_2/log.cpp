#include "log.h"

void Log::setColor(TextColor color)
{
#ifdef __linux__
    switch(color)
    {
        case Red: { std::cout << "\x1B[31m"; } break;
        case Green: { std::cout << "\x1B[32m"; } break;
        case Yellow: { std::cout << "\x1B[33m"; } break;
        case Blue: { std::cout << "\x1B[34m"; } break;
        case Pink: { std::cout << "\x1B[35m"; } break;
        case White: { std::cout << "\x1B[37m"; } break;
        case DarkWhite: { std::cout << "\x1B[0m"; } break;
    default:
        break;
    }
#endif
}

Log& Log::get()
{
    static Log log;
    return log;
}

void Log::msg(const std::string& sender, const std::string& msg, LogStatus status)
{
    setColor(DarkWhite);
    std::cout << "[" << sender << "][";
    if (status == LogError)
    {
        setColor(Red);
        std::cout << "ERROR";
    }
    else if (status == LogWarning)
    {
        setColor(Yellow);
        std::cout << "WARNING";
    }
    else
    {
        setColor(Blue);
        std::cout << "MESSAGE";
    }
    setColor(DarkWhite);
    std::cout << "]#> " << msg << std::endl;
}


void Log::Message(const std::string& sender, const std::string& msg)
{
    get().msg(sender, msg, LogMessage);
}
void Log::Warning(const std::string& sender, const std::string& msg)
{
    get().msg(sender, msg, LogWarning);
}
void Log::Error(const std::string& sender, const std::string& msg)
{
    get().msg(sender, msg, LogError);
}
