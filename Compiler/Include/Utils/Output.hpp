#pragma once
#include <iostream>
#include <string>

namespace Console 
{
    inline constexpr const char* Red    = "\033[31m";
    inline constexpr const char* Green  = "\033[32m";
    inline constexpr const char* Yellow = "\033[33m";
    inline constexpr const char* Blue   = "\033[34m";
    inline constexpr const char* White  = "\033[37m";
    inline constexpr const char* Reset  = "\033[0m";
    inline constexpr const char* Bold   = "\033[1m";
}

class Output 
{
    public:
        // User-friendly messages
        static void success( const std::string& msg, std::size_t indent = 0 );
        
        static void info( const std::string& msg, std::size_t indent = 0 );

        static void error( const std::string& msg );
        
        // Quiet mode support
        static void setQuiet( bool quiet ) { s_quiet = quiet; }
        static bool isQuiet() { return s_quiet; }
        
    private:
        static inline bool s_quiet = false;
};