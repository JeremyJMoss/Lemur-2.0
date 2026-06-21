#pragma once

/* === Imports === */

#include <memory>
#include "AST/ASTNode.hpp"

/* === Return === */

struct Return : Statement 
{
    std::unique_ptr<Expression> value;

    explicit Return( std::unique_ptr<Expression>&& val ) 
        : value( std::move( val ) ) {}

    ASTNodeType type() const override { return ASTNodeType::Return; }
};