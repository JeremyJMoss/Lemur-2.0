#pragma once

/* === Imports === */

#include "AST/ParsedType.hpp"
#include "AST/ASTNode.hpp"

/* === Parameter === */

struct Parameter : ASTNode
{
    const Identifier* identifier;
    const ParsedType* paramType;
    const Expression* defaultValue;

    Parameter( 
        const Identifier* name, 
        const ParsedType* type, 
        const Expression* defaultValue = nullptr
    ) : identifier( name ), 
        paramType( type ), 
        defaultValue( defaultValue ) {}

    void accept(ASTVisitor& v) const override { 
        return v.visit(*this);
    }
};