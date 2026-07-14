#pragma once

/* === Imports === */

#include <optional>
#include "AST/ASTNode.hpp"

/* === Forward Declarations === */

struct Identifier;

/* === ImportedSymbol === */

struct ImportedSymbol : ASTNode
{
    Identifier* name;
    std::optional<Identifier*> alias;

    ImportedSymbol( Identifier* name, std::optional<Identifier*> alias ) 
        : name( name ),
          alias( alias ) {}

    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }
};