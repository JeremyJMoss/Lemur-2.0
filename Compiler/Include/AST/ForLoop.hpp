#pragma once

/* === Imports === */

#include "AST/ASTNode.hpp"

/* === Forward Declarations === */

struct VariableDeclaration;
struct Block;

/* === For Loop === */

struct ForLoop : Statement 
{
    const VariableDeclaration* loopVar;
    const Expression* iterable;
    const Expression* step;
    const Expression* condition;
    const Block* body;

    ForLoop( 
        const VariableDeclaration* loopVar, 
        const Expression* iterable, 
        const Expression* step, 
        const Expression* condition, 
        const Block* body 
    ) : loopVar( loopVar ), 
        iterable( iterable ), 
        step( step ), 
        condition( condition ), 
        body( body ) {}

    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }
};