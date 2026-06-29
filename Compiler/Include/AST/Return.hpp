#pragma once

/* === Imports === */

#include "AST/ASTNode.hpp"

/* === Return === */

struct Return : Statement 
{
    const Expression* value;

    explicit Return( const Expression* val ) 
        : value( val ) {}

    ASTNodeType type() const override { return ASTNodeType::Return; }
};