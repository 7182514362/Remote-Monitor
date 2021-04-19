#include "Log.h"
using namespace ssh;

const char *Log::LogLevelStr[6] = {
    "TRACE ",
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL "};

Log::Log(std::ostream &os, LogLevel level) : m_stream(os)
{
    m_stream << (LogLevelStr[static_cast<LogLevelType>(level)]);
}

Log::~Log()
{
    m_stream << '\n';
}