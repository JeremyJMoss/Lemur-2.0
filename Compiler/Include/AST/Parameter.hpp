#pragma once

/* === Imports === */

#include <string>
#include <memory>
#include "AST/ParsedType.hpp"
#include "AST/ASTNode.hpp"

/* === Parameter === */

struct Parameter : ASTNode
{
    std::unique_ptr<Identifier> identifier;
    std::unique_ptr<ParsedType> paramType;
    std::unique_ptr<Expression> defaultValue;

    Parameter( 
        std::unique_ptr<Identifier> name, 
        std::unique_ptr<ParsedType>&& type, 
        std::unique_ptr<Expression>&& defaultValue = nullptr
    ) : identifier( std::move( name ) ), 
        paramType( std::move( type ) ), 
        defaultValue( std::move( defaultValue ) ) {}

    ASTNodeType type() const override { return ASTNodeType::Parameter; }
};