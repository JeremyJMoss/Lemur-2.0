#pragma once

/* === Imports === */

#include <vector>
#include "AST/ParsedType.hpp"
#include "AST/Parameter.hpp"
#include "AST/Block.hpp"

/* === FunctionLiteral === */

struct FunctionLiteral : Expression 
{
    const ParsedType* returnType;
    std::vector<const Parameter*> parameters;
    const Block* body;

    explicit FunctionLiteral( 
        const ParsedType* retType, 
        std::vector<const Parameter*> params, 
        const Block* block 
    ) : returnType( retType ), 
        parameters( std::move( params ) ), 
        body( block ) {};

    void accept(ASTVisitor& v) const override { 
        return v.visit(*this);
    }
};