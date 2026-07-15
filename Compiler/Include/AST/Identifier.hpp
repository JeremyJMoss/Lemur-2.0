#pragma once

/* === Dependencies ===*/

#include <string>

/* === Imports === */

#include "AST/ASTNode.hpp"

/* === Identifier === */

struct Identifier : Expression 
{
    std::string_view name;
    
    Identifier( std::string_view value ) 
        : name( value ) {}
    
    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }
};
