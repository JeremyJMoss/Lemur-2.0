#pragma once

/* === Imports === */

#include <string>
#include "AST/ASTNode.hpp"

/* === Qualified Name === */

struct QualifiedName : ASTNode
{
    std::string name;

    QualifiedName( std::string name )
        : name( std::move( name ) )
    {}

    void accept( ASTVisitor& v ) const override
    {
        v.visit(*this);
    }
};