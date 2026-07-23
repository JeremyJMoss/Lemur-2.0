#pragma once

/* === Dependencies ===*/

#include <vector>

/* === Imports === */

#include "AST/ASTNode.hpp"

/* === Block === */

struct BlockStatement : Statement 
{
    Block* block;

    explicit BlockStatement( Block* block ) 
        : block( block ) {}

    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }
};