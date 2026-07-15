#pragma once

/* === Dependencies ===*/

#include <string>
#include <variant>

/* === Imports === */

#include "AST/ASTNode.hpp"

/* === Variant Declaration === */

using LiteralValue = std::variant<
    std::string, 
    char, 
    int, 
    float, 
    bool, 
    std::monostate
>;

/* === Literal === */

struct Literal : Expression 
{
    LiteralValue value;

    // avoiding extra copies with multiple constructors
    explicit Literal( const LiteralValue& val )
        : value( val ) {}
    
    explicit Literal( LiteralValue&& val )
        : value( std::move( val ) ) {}
    
    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }
};