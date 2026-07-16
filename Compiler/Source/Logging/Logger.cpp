/* === Main Import === */

#include "Logging/Logger.hpp"

/* === Imports === */

#include "Logging/LogLevel.hpp"
#include "Logging/Attribute.hpp"

/* === Dependencies === */

#include <string>
#include <format>
#include <algorithm>
#include <unordered_map>
#include <span>
#include <sstream>
#include <source_location>

/* === Logger Fields === */

LogLevel Logger::s_logLevel = LogLevel::INFO;

const std::unordered_map<std::string_view, LogLevel> Logger::s_levelMap = {
    { "error",   LogLevel::ERROR },
    { "warn",    LogLevel::WARN },
    { "info",    LogLevel::INFO },
    { "debug",   LogLevel::DEBUG },
    { "trace",   LogLevel::TRACE }
};

/* === Logger Methods === */

void Logger::setFile( const fs::path& filepath ) {
    if ( filepath.has_parent_path() )
        fs::create_directories(filepath.parent_path());

    if ( s_logFile.is_open() ) {
        s_logFile.close();
    }

    s_logFile.open( filepath, std::ios::app );
}

void Logger::error( 
    std::string_view msg, 
    std::span<const Attribute> attributes,
    const std::source_location& loc 
) 
{
    if ( s_logLevel >= LogLevel::ERROR ) 
    {
        print( LogLevel::ERROR, msg, extractFunctionName( loc.function_name() ), attributes );
    }
}

void Logger::warn( 
    std::string_view msg, 
    std::span<const Attribute> attributes,
    const std::source_location& loc 
) 
{
    if ( s_logLevel >= LogLevel::WARN ) 
    {
        print( LogLevel::WARN, msg, extractFunctionName( loc.function_name() ), attributes );
    }
}

void Logger::info( 
    std::string_view msg,
    std::span<const Attribute> attributes,
    const std::source_location& loc
) 
{
    if ( s_logLevel >= LogLevel::INFO ) 
    {
        print( LogLevel::INFO, msg, extractFunctionName( loc.function_name() ), attributes );
    }
}

void Logger::debug( 
    std::string_view msg, 
    std::span<const Attribute> attributes,
    const std::source_location& loc
) 
{
    if ( s_logLevel >= LogLevel::DEBUG ) 
    {
        print( LogLevel::DEBUG, msg, extractFunctionName( loc.function_name() ), attributes );
    }
}

void Logger::trace(
    std::string_view msg,
    std::span<const Attribute> attributes,
    const std::source_location& loc
) 
{
    if ( s_logLevel >= LogLevel::TRACE ) 
    {
        print( LogLevel::TRACE, msg, extractFunctionName( loc.function_name() ), attributes );
    }
}

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
    if ( !s_shouldLog )
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
        std::string filePrefix = std::format("[{}]", toString( level ) );
        std::string fileFullMsg = filePrefix + fileStream.str();
        s_logFile << fileFullMsg;
    }
}

void Logger::ensureLogFileOpen() {
    if ( !s_logFile.is_open() ) {
        setFile();
    }
}

std::string Logger::extractFunctionName( const char* prettyFn ) {
    std::string s( prettyFn );

    // Strip return type by finding first space before class::function
    auto parenPos = s.find( '(' );
    if ( parenPos != std::string::npos ) {
        auto beforeParen = s.substr( 0, parenPos );

        auto lastSpace = beforeParen.rfind( ' ' );
        if ( lastSpace != std::string::npos ) {
            return beforeParen.substr( lastSpace + 1 ); 
        }
        return beforeParen; // fallback
    }
    return s;
}