#pragma once

/* === Imports === */

#include <vector>
#include "AST/ASTNode.hpp"
#include "AST/ParsedType.hpp"
#include "AST/Parameter.hpp"
#include "AST/Identifier.hpp"
#include "AST/Block.hpp"

/* === FunctionDeclaration === */

struct FunctionDeclaration : Declaration
{
    const Identifier* identifier;
    const ParsedType* returnType;
    std::vector<const Parameter*> parameters;
    const Block* body;
    bool hasImplementation;

    explicit FunctionDeclaration(
        const Identifier* identifier, 
        const ParsedType* returnType, 
        std::vector<const Parameter*> parameters, 
        const Block* body,
        bool functionHasBody = true
    ) : identifier( identifier ), 
        returnType( returnType ), 
        parameters( std::move( parameters ) ), 
        body( body ), 
        hasImplementation( functionHasBody ) {};

    void accept(ASTVisitor& v) const override { 
        return v.visit(*this);
    }
};