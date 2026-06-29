#pragma once

/* === Imports === */

#include "AST/ASTNode.hpp"
#include "AST/Block.hpp"

/* === IfConditional === */

struct IfConditional : Statement 
{
    const Expression* condition;
    const IfConditional* elseStatement;
    const Block* then;
    
    IfConditional(
        const Expression* condition, 
        const IfConditional* elseStatement, 
        const Block* body
    ) : condition( condition ), 
        elseStatement( elseStatement ), 
        then( body ) {};

    ASTNodeType type() const override { return ASTNodeType::IfConditional; }
};