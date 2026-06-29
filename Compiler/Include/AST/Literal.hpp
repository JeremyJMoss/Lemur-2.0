#pragma once

/* === Imports === */

#include <string>
#include <variant>
#include "AST/ASTNode.hpp"

/* === Constants === */

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

    explicit Literal( LiteralValue val ) 
        : value( std::move( val ) ) {}
    
    void accept(ASTVisitor& v) const override { 
        return v.visit(*this);
    }
};