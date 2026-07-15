#pragma once

/* === Imports ===*/

#include "AST/ASTNode.hpp"

/* === Range Expression === */

struct Range : Expression 
{
    const Expression* start;
    const Expression* end;
    bool inclusive; // true for `to`, false for `until`

    explicit Range(
        const Expression* start, 
        const Expression* end, 
        bool incl
    ) : start( start ), 
        end( end ), 
        inclusive( incl ) {}

    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }
};