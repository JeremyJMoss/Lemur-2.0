#pragma once

/* === Imports === */

#include <memory>
#include <vector>
#include "AST/ASTNode.hpp"
#include "AST/ParsedType.hpp"
#include "Types/FunctionTypeInfo.hpp"
#include "AST/Parameter.hpp"
#include "AST/Identifier.hpp"
#include "AST/Block.hpp"

/* === FunctionDeclaration === */

struct FunctionDeclaration : Declaration
{
    std::unique_ptr<Identifier> identifier;
    std::unique_ptr<ParsedType> returnType;
    std::vector<std::unique_ptr<Parameter>> parameters;
    std::unique_ptr<Block> body;
    bool hasImplementation;

    explicit FunctionDeclaration(
        std::unique_ptr<Identifier>&& _identifier, 
        std::unique_ptr<ParsedType>&& _returnType, 
        std::vector<std::unique_ptr<Parameter>>&& _parameters, 
        std::unique_ptr<Block>&& _body,
        bool functionHasBody = true
    ) : identifier( std::move( _identifier ) ), returnType( std::move( _returnType ) ), 
        parameters( std::move( _parameters ) ), body( std::move( _body ) ), hasImplementation( functionHasBody ) {};

    ASTNodeType type() const override { return ASTNodeType::FunctionDeclaration; }
};