/* === Main Import === */

#include "Logging/LogLevel.hpp"

/* === Log Level methods === */

std::string_view toString( LogLevel level )
{
    switch( level )
    {
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::TRACE: return "TRACE";
        default:              return "UNKNOWN";
    }
}