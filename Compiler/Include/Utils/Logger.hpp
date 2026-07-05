#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <source_location>
#include <span>

namespace fs = std::filesystem;

enum class LogLevel 
{
    ERROR = 0,
    WARN,
    INFO,
    DEBUG,
    TRACE
};

static inline std::string toString( const LogLevel& level )
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

struct Attribute {
    std::string_view key;
    std::string_view val;
};

static inline std::string extractFunctionName( const char* prettyFn ) {
    std::string s( prettyFn );

    // Strip return type by finding first space before class::function
    auto parenPos = s.find('(');
    if (parenPos != std::string::npos) {
        auto beforeParen = s.substr(0, parenPos);

        auto lastSpace = beforeParen.rfind(' ');
        if (lastSpace != std::string::npos) {
            return beforeParen.substr(lastSpace + 1); 
        }
        return beforeParen; // fallback
    }
    return s;
}

class Logger 
{
    public:
        static void setLevel( LogLevel level ) { s_logLevel = level; }

        static LogLevel getLevel() { return s_logLevel; }

        static void setShouldLog( bool shouldLog ) { s_shouldLog = shouldLog; }

        static LogLevel parseLogLevel( const std::string& levelStr );

        static void setFile( fs::path filepath = DefaultLogFile ) {
            fs::path path( filepath );

            if ( path.has_parent_path() )
                fs::create_directories(path.parent_path());

            if ( s_logFile.is_open() ) {
                s_logFile.close();
            }

            s_logFile.open(path, std::ios::app);
        }

        static void error( 
            const std::string& msg, 
            std::span<const Attribute> attributes = {},
            const std::source_location& loc = std::source_location::current()  
        ) 
        {
            if ( s_logLevel >= LogLevel::ERROR ) 
            {
                print( LogLevel::ERROR, msg, extractFunctionName( loc.function_name() ), attributes );
            }
        }

        static void warn( 
            const std::string& msg, 
            std::span<const Attribute> attributes = {},
            const std::source_location& loc = std::source_location::current() 
        ) 
        {
            if ( s_logLevel >= LogLevel::WARN ) 
            {
                print( LogLevel::WARN, msg, extractFunctionName( loc.function_name() ), attributes );
            }
        }

        static void info( 
            const std::string& msg,
            std::span<const Attribute> attributes = {},
            const std::source_location& loc = std::source_location::current()
        ) 
        {
            if ( s_logLevel >= LogLevel::INFO ) 
            {
                print( LogLevel::INFO, msg, extractFunctionName( loc.function_name() ), attributes );
            }
        }

        static void debug( 
            const std::string& msg, 
            std::span<const Attribute> attributes = {},
            const std::source_location& loc = std::source_location::current()
        ) 
        {
            if ( s_logLevel >= LogLevel::DEBUG ) 
            {
                print( LogLevel::DEBUG, msg, extractFunctionName( loc.function_name() ), attributes );
            }
        }

        static void trace(
            const std::string& msg,
            std::span<const Attribute> attributes = {},
            const std::source_location& loc = std::source_location::current()
        ) 
        {
            if ( s_logLevel >= LogLevel::TRACE ) 
            {
                print( LogLevel::TRACE, msg, extractFunctionName( loc.function_name() ), attributes );
            }
        }

    private:
        static LogLevel s_logLevel;
        static inline bool s_shouldLog = false;
        static inline std::ofstream s_logFile;
        static const std::unordered_map<std::string, LogLevel> s_levelMap;
        static constexpr std::string_view DefaultLogFile = "logs/compiler.log";

        static void print(
            LogLevel level,
            const std::string& msg,
            const std::string& funcName,
            std::span<const Attribute> attributes = {}
        );

        static void ensureLogFileOpen() {
            if (!s_logFile.is_open()) {
                setFile();
            }
        }
};