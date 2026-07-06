#pragma once

/* === Imports === */

#include "AST/ASTNode.hpp"

/* === Break === */

struct Break : Statement 
{
    Break() = default;

    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }
};
