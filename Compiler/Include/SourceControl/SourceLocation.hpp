#pragma once

/* === Dependencies ===*/

#include <string>

/* === Imports === */

#include "Core/Ids.hpp"

/* === Forward Declarations === */

class Token;

/* === Source Location === */

struct SourceLocation 
{
    std::size_t line;
    std::size_t column;
    std::string toString() const { 
        return std::to_string( line ) + ":" + std::to_string( column + 1 ); 
    }
};

/* === Source Range === */

struct SourceRange 
{
    SourceLocation start;
    SourceLocation end;
    FileId fileId{};

    static SourceRange getLocation( const Token& startToken, const Token& endToken );

    static SourceRange getLocation( const Token& token );

    std::string toString() const;
};