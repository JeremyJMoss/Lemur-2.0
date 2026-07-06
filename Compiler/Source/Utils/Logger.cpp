#include "Utils/Logger.hpp"
#include <algorithm>
#include <sstream>

LogLevel Logger::s_logLevel = LogLevel::INFO;

const std::unordered_map<std::string_view, LogLevel> Logger::s_levelMap = {
    { "error",   LogLevel::ERROR },
    { "warn",    LogLevel::WARN },
    { "info",    LogLevel::INFO },
    { "debug",   LogLevel::DEBUG },
    { "trace",   LogLevel::TRACE }
};

LogLevel Logger::parseLogLevel( const std::string& levelStr ) 
{
    std::string lower = levelStr;
    std::transform( lower.begin(), lower.end(), lower.begin(), ::tolower );

    auto it = s_levelMap.find( lower );
    if ( it != s_levelMap.end() )
        return it->second;

    // Default if unrecognized
    return LogLevel::INFO;
}

void Logger::print(
    LogLevel level,
    std::string_view msg,
    std::string_view funcName,
    std::span<const Attribute> attributes
)
{
    if (!s_shouldLog)
        return;

    ensureLogFileOpen();
    
    std::ostringstream fileStream;

    fileStream << "Fn: '" << funcName << "' | Msg: '" << msg << "'";

    // Attributes
    for ( const auto& attribute : attributes ) 
    {
        fileStream << " | " << attribute.key << ": " << attribute.val;
    }

    // Add newlines
    fileStream << '\n';

    // File output
    if ( s_logFile.is_open() ) 
    {
        std::string filePrefix = "[" + toString( level ) + "] ";
        std::string fileFullMsg = filePrefix + fileStream.str();
        s_logFile << fileFullMsg;
    }
}