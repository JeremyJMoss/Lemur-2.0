#pragma once

/* === Imports === */

#include "AST/ASTNode.hpp"

/* === Forward Declarations === */

struct QualifiedName;

/* === Module Declaration === */

struct ModuleDeclaration : Statement {
    QualifiedName* name;

    ModuleDeclaration( QualifiedName* name )
        : name( name ) {}

    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }
};
