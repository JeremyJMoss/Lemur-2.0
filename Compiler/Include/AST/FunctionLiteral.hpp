#pragma once

/* === Imports === */

#include <memory>
#include <vector>
#include "AST/ParsedType.hpp"
#include "Types/FunctionTypeInfo.hpp"
#include "AST/Parameter.hpp"
#include "AST/Block.hpp"

/* === FunctionLiteral === */

struct FunctionLiteral : Expression 
{
    std::unique_ptr<ParsedType> returnType;
    std::vector<std::unique_ptr<Parameter>> parameters;
    std::unique_ptr<Block> body;

    explicit FunctionLiteral( 
        std::unique_ptr<ParsedType>&& retType, 
        std::vector<std::unique_ptr<Parameter>>&& params, 
        std::unique_ptr<Block>&& block 
    ) : returnType( std::move( retType ) ), parameters( std::move( params ) ), 
        body( std::move( block ) ) {};

    ASTNodeType type() const override { return ASTNodeType::FunctionLiteral; }
};