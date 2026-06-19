#pragma once

/* === Imports === */

#include <memory>
#include "AST/ASTNode.hpp"
#include "AST/Block.hpp"

/* === IfConditional === */

struct IfConditional : Statement 
{
    std::unique_ptr<Expression> condition;
    std::unique_ptr<IfConditional> elseStatement;
    std::unique_ptr<Block> then;
    
    IfConditional(
        std::unique_ptr<Expression>&& condition, 
        std::unique_ptr<IfConditional>&& elseStatement, 
        std::unique_ptr<Block>&& body
    ) : condition( std::move( condition ) ), elseStatement( std::move( elseStatement ) ), 
        then( std::move( body ) ) {};

    ASTNodeType type() const override { return ASTNodeType::IfConditional; }
};