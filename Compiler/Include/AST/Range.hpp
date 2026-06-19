#pragma once

/* === Imports === */

#include <memory>
#include "AST/ASTNode.hpp"

/* === Range Expression === */

struct Range : Expression 
{
    std::unique_ptr<Expression> start;
    std::unique_ptr<Expression> end;
    bool inclusive; // true for `to`, false for `until`

    explicit Range(
        std::unique_ptr<Expression>&& start, 
        std::unique_ptr<Expression>&& end, bool incl
    ) : start( std::move( start ) ), end( std::move (end ) ), 
        inclusive( incl ) {};

        ASTNodeType type() const override { return ASTNodeType::Range; }
};