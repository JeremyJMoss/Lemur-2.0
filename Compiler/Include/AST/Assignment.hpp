#pragma once

/* === Imports === */

#include "AST/ASTNode.hpp"
#include "AST/Identifier.hpp"

/* === Assignment === */

struct Assignment : Expression 
{
    const Expression* identifier;
    const Expression* value;

    Assignment( Expression* id, Expression* val )
    : identifier( id ), value( val ) {};
    
    void accept( ASTVisitor& v ) const override 
    { 
        return v.visit( *this );
    }
};