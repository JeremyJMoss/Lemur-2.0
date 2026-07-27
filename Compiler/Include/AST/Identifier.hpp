#pragma once

/* === Dependencies ===*/

#include <string>

/* === Imports === */

#include "AST/ASTNode.hpp"

/* === Identifier === */

struct Identifier : Expression 
{
    std::string name;
    
    Identifier( std::string value ) 
        : name( std::move( value ) ) {}
    
    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }
};
