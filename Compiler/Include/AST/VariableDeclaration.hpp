#pragma once

/* === Imports === */

#include <memory>
#include "AST/ASTNode.hpp"
#include "AST/ParsedType.hpp"
#include "Types/TypeInfo.hpp"
#include "AST/Identifier.hpp"

/* === VariableDeclaration === */

struct VariableDeclaration : Declaration 
{
    std::unique_ptr<Identifier> identifier;
    std::unique_ptr<ParsedType> varType;
    bool locked;
    std::unique_ptr<Expression> initialiser;

    explicit VariableDeclaration( 
        std::unique_ptr<Identifier>&& id, 
        std::unique_ptr<ParsedType>&& type, 
        bool lock, 
        std::unique_ptr<Expression>&& initialiserStmt
    ) : identifier( std::move( id ) ), varType( std::move( type ) ), 
        locked( lock ), initialiser( std::move( initialiserStmt ) ) {};

    ASTNodeType type() const override { return ASTNodeType::VariableDeclaration; }
};