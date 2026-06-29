#pragma once

/* === Imports === */

#include "AST/ASTNode.hpp"
#include "AST/Block.hpp"
#include "AST/VariableDeclaration.hpp"

/* === For Loop === */

struct ForLoop : Statement 
{
    const VariableDeclaration* loopVar;
    const Expression* iterable;
    const Expression* step;
    const Expression* condition;
    const Block* body = nullptr;

    ForLoop( 
        const VariableDeclaration* loopVar, 
        const Expression* iterable, 
        const Expression* step, 
        const Expression* where, 
        const Block* body 
    ) : loopVar( loopVar ), iterable( iterable ), 
        step( step ), condition( where ), 
        body( body ) {}

    void accept(ASTVisitor& v) const override { 
        return v.visit(*this);
    }
};