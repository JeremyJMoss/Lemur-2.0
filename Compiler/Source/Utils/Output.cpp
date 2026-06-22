#include "Utils/Output.hpp"

void Output::success( const std::string& msg, std::size_t indent ) 
{
    auto indentation = std::string( indent * 4, ' ' );
    if ( !s_quiet ) 
    {
        std::cout << indentation << Console::Green << "✓ " << msg << Console::Reset << std::endl;
    }
}

void Output::info( const std::string& msg, std::size_t indent ) 
{
    auto indentation = std::string( indent * 4, ' ' );
    if ( !s_quiet )
    {
        std::cout << indentation << Console::White << msg << Console::Reset << std::endl;
    }
}