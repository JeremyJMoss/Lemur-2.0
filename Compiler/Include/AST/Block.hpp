#pragma once

/* === Imports === */

#include <vector>
#include "AST/ASTNode.hpp"

/* === Block === */

struct Block : Statement 
{
    const std::vector<Statement*> statements;

    explicit Block( std::vector<Statement*> statements ) 
        : statements( std::move( statements ) ) {}

    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }
};
