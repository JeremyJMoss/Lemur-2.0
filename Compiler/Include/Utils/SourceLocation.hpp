#pragma once

#include <string>
#include <cstddef>

struct SourceLocation 
{
    std::size_t line;
    std::size_t column;
    std::string toString() const { 
        return std::to_string( line ) + ":" + std::to_string( column + 1 ); 
    }
};

struct SourceRange 
{
    SourceLocation start;
    SourceLocation end;
    std::size_t fileId;

    std::string toString() const { 
        // If start and end are the same line/column, just show one position
        if ( start.line == end.line && start.column == end.column ) 
        {
            return start.toString();
        }
        else if ( start.line == end.line )
        {
            return std::to_string( start.line ) + ":" + std::to_string( start.column ) + "-" + std::to_string( end.column );
        }

        return start.toString() + " - " + end.toString();
    }
};