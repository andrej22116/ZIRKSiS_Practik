#ifndef LOG_H
#define LOG_H

#include <iostream>

class Log
{
private:
    Log() {}
    Log(const Log&) {}

    Log& operator = (const Log&) = delete;

    enum TextColor {
        Green,
        Blue,
        Red,
        Pink,
        Yellow,
        White,
        DarkGreen,
        DarkBlue,
        DarkRed,
        DarkPink,
        DarkYellow,
        DarkWhite
    };

    void setColor(TextColor color);

public:
    enum LogStatus {
        LogMessage,
        LogWarning,
        LogError
    };

    static Log& get();
    static void Message(const std::string& sender, const std::string& msg);
    static void Warning(const std::string& sender, const std::string& msg);
    static void Error(const std::string& sender, const std::string& msg);

    void msg(const std::string& sender, const std::string& msg, LogStatus status = LogMessage);
};

#endif // LOG_H
