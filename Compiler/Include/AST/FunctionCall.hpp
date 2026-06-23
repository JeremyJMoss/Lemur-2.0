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
    std::vector<std::unique_ptr<Expression>> arguments;

    explicit FunctionCall( std::unique_ptr<Expression>&& callee, std::vector<std::unique_ptr<Expression>>&& args )
        : callee( std::move( callee ) ), arguments( std::move( args ) ) {};

    ASTNodeType type() const override { return ASTNodeType::FunctionCall; }
};
