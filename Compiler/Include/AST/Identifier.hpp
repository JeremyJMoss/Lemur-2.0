#pragma once

/* === Imports === */

#include <string>
#include "AST/ASTNode.hpp"

/* === Identifier === */

struct Identifier : Expression 
{
    const std::string name;
    explicit Identifier( std::string value ) 
        : name( std::move( value ) ) {}
    
    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }
};
