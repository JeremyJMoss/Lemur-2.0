#pragma once

/* === Imports === */

#include <vector>
#include "AST/ASTNode.hpp"

/* === FunctionCall === */

struct FunctionCall : Expression 
{
    const Expression* callee;
    const std::vector<Expression*> arguments;

    explicit FunctionCall( 
        const Expression* callee, 
        std::vector<Expression*> args 
    ) : callee( callee ), 
        arguments( std::move( args ) ) {}

    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }
};
