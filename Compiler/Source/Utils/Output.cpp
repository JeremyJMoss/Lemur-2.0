#include "Utils/Output.hpp"
#include <iostream>

void Output::success( std::string_view msg, std::size_t indent ) 
{
    auto indentation = std::string( indent * 4, ' ' );
    if ( !s_quiet ) 
    {
        std::cout << indentation << Console::Green << "✓ " << msg << Console::Reset << std::endl;
    }
}

void Output::info( std::string_view msg, std::size_t indent ) 
{
    auto indentation = std::string( indent * 4, ' ' );
    if ( !s_quiet )
    {
        std::cout << indentation << Console::White << msg << Console::Reset << std::endl;
    }
}

void Output::error( std::string_view error ) {
    std::cerr << Console::Red << error << Console::Reset << std::endl;
}