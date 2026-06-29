#pragma once

/* === Imports === */

#include "AST/ASTNode.hpp"
#include "AST/Identifier.hpp"

/* === Assignment === */

struct Assignment : Expression 
{
    const Expression* identifier;
    const Expression* value;

    explicit Assignment( Expression* id, Expression* val )
    : identifier( id ), value( val ) {};

    ASTNodeType type() const override { return ASTNodeType::Assignment; }
};