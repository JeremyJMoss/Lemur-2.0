#pragma once

/* === Imports === */
#include "AST/ASTNode.hpp"

/* === ExpressionStatement === */

struct ExpressionStatement : Statement 
{
    const Expression* expression;

    explicit ExpressionStatement( const Expression* expr ) 
        : expression( expr ) {}

    void accept(ASTVisitor& v) const override { 
        return v.visit(*this);
    }
};