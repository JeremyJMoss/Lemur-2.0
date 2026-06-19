#pragma once

/* === Imports === */

#include <memory>
#include <vector>
#include "AST/ASTNode.hpp"
#include "AST/Identifier.hpp"

/* === FunctionCall === */

struct FunctionCall : Expression 
{
    std::unique_ptr<Expression> callee;
    std::vector<std::unique_ptr<Expression>> params;

    explicit FunctionCall( std::unique_ptr<Expression>&& _callee, std::vector<std::unique_ptr<Expression>>&& _params )
        : callee( std::move( _callee ) ), params( std::move( _params ) ) {};

    ASTNodeType type() const override { return ASTNodeType::FunctionCall; }
};
