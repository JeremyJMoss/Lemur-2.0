#pragma once

/* === Imports === */

#include <vector>
#include "AST/ASTNode.hpp"
#include "AST/Identifier.hpp"

/* === FunctionCall === */

struct FunctionCall : Expression 
{
    const Expression* callee;
    std::vector<const Expression*> arguments;

    explicit FunctionCall( const Expression* callee, std::vector<const Expression*> args )
        : callee( callee ), arguments( std::move( args ) ) {};

    ASTNodeType type() const override { return ASTNodeType::FunctionCall; }
};
