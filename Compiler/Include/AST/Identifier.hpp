#pragma once

/* === Imports === */

#include <string>
#include "AST/ASTNode.hpp"

/* === Identifier === */

struct Identifier : Expression 
{
    std::string name;
    explicit Identifier( const std::string& value ) : name( std::move( value ) ) {}
    
    ASTNodeType type() const override { return ASTNodeType::Identifier; }
};
