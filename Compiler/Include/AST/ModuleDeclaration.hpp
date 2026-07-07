#pragma once

/* === Imports === */

#include "AST/ASTNode.hpp"

/* === Forward Declaration === */

struct Identifier;

/* === Module Declaration === */

struct ModuleDeclaration : Statement {
    const Identifier* identifier;

    ModuleDeclaration( const Identifier* identifier )
        : identifier( identifier ) {}

    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }
};
