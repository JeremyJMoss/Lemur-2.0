#pragma once

/* === Imports === */

#include <memory>
#include "AST/ASTNode.hpp"
#include "AST/Identifier.hpp"

/* === Assignment === */

struct Assignment : Expression 
{
    std::unique_ptr<Expression> identifier;
    std::unique_ptr<Expression> value;

    explicit Assignment( std::unique_ptr<Expression>&& id, std::unique_ptr<Expression>&& val )
    : identifier( std::move( id ) ), value( std::move ( val ) ) {};

    ASTNodeType type() const override { return ASTNodeType::Assignment; }
};