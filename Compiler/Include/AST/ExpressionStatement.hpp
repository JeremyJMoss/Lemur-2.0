#pragma once

/* === Imports === */
#include "AST/ASTNode.hpp"

/* === ExpressionStatement === */

struct ExpressionStatement : Statement 
{
    const Expression* expression;

    explicit ExpressionStatement( const Expression* expr ) 
        : expression( expr ) {}

    ASTNodeType type() const override { return ASTNodeType::ExpressionStatement; }
};