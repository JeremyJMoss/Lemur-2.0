#pragma once

/* === Imports === */

#include <vector>
#include <AST/ASTNode.hpp>

/* === Forward Declarations === */

struct ParsedType;
struct Parameter;
struct Block;

/* === FunctionLiteral === */

struct FunctionLiteral : Expression 
{
    const ParsedType* returnType;
    const std::vector<Parameter*> parameters;
    const Block* body;

    FunctionLiteral( 
        const ParsedType* retType, 
        std::vector<Parameter*> params, 
        const Block* block 
    ) : returnType( retType ), 
        parameters( std::move( params ) ), 
        body( block ) {};

    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }
};