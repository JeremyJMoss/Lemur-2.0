#pragma once

/* === Dependencies ===*/

#include "AST/ASTNode.hpp"

/* === Forward Declarations === */

struct Block;

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

    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }
};