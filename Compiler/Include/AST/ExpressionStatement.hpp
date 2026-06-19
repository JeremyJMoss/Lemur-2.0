#pragma once

/* === Imports === */
#include <memory>
#include "AST/ASTNode.hpp"

/* === ExpressionStatement === */

struct ExpressionStatement : Statement 
{
    std::unique_ptr<Expression> expression;

    explicit ExpressionStatement( std::unique_ptr<Expression>&& expr ) 
        : expression( std::move( expr ) ) {}

    ASTNodeType type() const override { return ASTNodeType::ExpressionStatement; }
};