#pragma once

/* === Dependencies === */

#include <source_location>
#include <string>
#include <filesystem>
#include <span>
#include <unordered_map>
#include <fstream>

namespace fs = std::filesystem;

/* === Imports === */

#include "Logging/LogLevel.hpp"
#include "Logging/Attribute.hpp"

/* === Logger === */

class Logger 
{
    public:
        static void setLevel( LogLevel level ) { s_logLevel = level; }

        static LogLevel getLevel() { return s_logLevel; }

        static void setShouldLog( bool shouldLog ) { s_shouldLog = shouldLog; }

        static LogLevel parseLogLevel( const std::string& levelStr );

        static void setFile( const fs::path& filepath = DefaultLogFile );

        static void error( 
            std::string_view msg, 
            std::span<const Attribute> attributes = {},
            const std::source_location& loc = std::source_location::current()  
        );

        static void warn( 
            std::string_view msg, 
            std::span<const Attribute> attributes = {},
            const std::source_location& loc = std::source_location::current() 
        );

        static void info( 
            std::string_view msg,
            std::span<const Attribute> attributes = {},
            const std::source_location& loc = std::source_location::current()
        );

        static void debug( 
            std::string_view msg, 
            std::span<const Attribute> attributes = {},
            const std::source_location& loc = std::source_location::current()
        );

        static void trace(
            std::string_view msg,
            std::span<const Attribute> attributes = {},
            const std::source_location& loc = std::source_location::current()
        );

    private:
        static LogLevel s_logLevel;
        static inline bool s_shouldLog = false;
        static inline std::ofstream s_logFile;
        static const std::unordered_map<std::string_view, LogLevel> s_levelMap;
        static constexpr std::string_view DefaultLogFile = "logs/compiler.log";

        static void print(
            LogLevel level,
            std::string_view msg,
            std::string_view funcName,
            std::span<const Attribute> attributes = {}
        );

        static void ensureLogFileOpen();

        static std::string extractFunctionName( const char* prettyFn );
};