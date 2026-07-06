#pragma once

/* === Imports === */

#include "AST/ASTNode.hpp"

/* === Forward Declarations === */

struct Identifier;
struct ParsedType;

/* === VariableDeclaration === */

struct VariableDeclaration : Declaration 
{
    const Identifier* identifier;
    const ParsedType* varType;
    bool locked;
    const Expression* initialiser;

    explicit VariableDeclaration( 
        const Identifier* id, 
        const ParsedType* type, 
        bool lock, 
        const Expression* initialiserStmt
    ) : identifier( id ), 
        varType( type ), 
        locked( lock ), 
        initialiser( initialiserStmt ) {}

    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }
};