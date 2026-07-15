#pragma once

/* === Dependencies ===*/

#include <vector>

/* === Imports === */

#include "AST/ASTNode.hpp"

/* === Forward Declarations === */

struct Identifier;
struct ParsedType;
struct Parameter;
struct Block;

/* === FunctionDeclaration === */

struct FunctionDeclaration : Declaration
{
    const Identifier* identifier;
    const ParsedType* returnType;
    const std::vector<Parameter*> parameters;
    const Block* body;
    bool hasImplementation;
    bool isEntry;

    explicit FunctionDeclaration(
        const Identifier* identifier, 
        const ParsedType* returnType, 
        std::vector<Parameter*> parameters, 
        const Block* body,
        bool functionHasBody = true,
        bool isEntry = false
    ) : identifier( identifier ), 
        returnType( returnType ), 
        parameters( std::move( parameters ) ), 
        body( body ), 
        hasImplementation( functionHasBody ),
        isEntry( isEntry ) {};

    void accept( ASTVisitor& v ) const override 
    { 
        v.visit(*this);
    }
};