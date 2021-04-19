#ifndef LOG_H
#define LOG_H
#include <iostream>
#include <type_traits>
#include <QDebug>
namespace ssh
{
    enum class LogLevel
    {
        TRACE_ = 0,
        DEBUG_ = 1,
        INFO_,
        WARN_,
        ERROR_,
        FATAL_
    };

    class Log
    {
        using LogLevelType = std::underlying_type<LogLevel>::type;

    public:
        Log(std::ostream &os, LogLevel level);
        ~Log();
        std::ostream &stream()
        {
            return m_stream;
        }

    private:
        std::ostream &m_stream;
        static const char *LogLevelStr[6];
    };
}
/*
#define LOG_TRACE ssh::Log(std::cout, ssh::LogLevel::TRACE_).stream()

#define LOG_DEBUG ssh::Log(std::cout, ssh::LogLevel::DEBUG_).stream()

#define LOG_INFO ssh::Log(std::cout, ssh::LogLevel::INFO_).stream()

#define LOG_WARN ssh::Log(std::cout, ssh::LogLevel::WARN_).stream()

#define LOG_ERROR ssh::Log(std::cout, ssh::LogLevel::ERROR_).stream()

#define LOG_FATAL ssh::Log(std::cout, ssh::LogLevel::FATAL_).stream()
*/


#define LOG_DEBUG qDebug()
#define LOG_ERROR qDebug()
#define LOG_INFO qDebug()
#define LOG_WARN qDebug()

#endif
