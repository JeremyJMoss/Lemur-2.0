#pragma once

#include <string>
#include <vector>
#include <memory>
#include "AST/ASTNode.hpp"
#include "AST/Identifier.hpp"
#include "Utils/OwnershipKind.hpp"

// Generic Type References
enum class ParsedTypeKind
{
    Named,
    Function,
    Ownership,
    Array,
    Inferred
};

struct ParsedType : ASTNode {
    ParsedTypeKind kind;

    virtual ~ParsedType() = default;
    ParsedType( ParsedTypeKind kind ) : kind( kind ) {}

    ASTNodeType type() const override { return ASTNodeType::ParsedType; }
};

struct ParsedInferredType: ParsedType {
    ParsedInferredType() : ParsedType( ParsedTypeKind::Inferred ) {}
};

struct ParsedNamedType : ParsedType {
    std::unique_ptr<Identifier> identifier;

    ParsedNamedType( std::unique_ptr<Identifier>&& identifier )
        : ParsedType( ParsedTypeKind::Named ), identifier( std::move(identifier) ) {}
};

struct ParsedFunctionType : ParsedType {
    std::vector<std::unique_ptr<ParsedType>> parameters;
    std::unique_ptr<ParsedType> returnType;

    ParsedFunctionType( std::vector<std::unique_ptr<ParsedType>>&& parameters, std::unique_ptr<ParsedType>&& returnType )
        : ParsedType( ParsedTypeKind::Function ), parameters( std::move( parameters ) ), returnType( std::move( returnType ) ) {}
};

struct ParsedArrayType : ParsedType {
    std::unique_ptr<ParsedType> elementType;
    std::unique_ptr<Expression> size;

    ParsedArrayType( std::unique_ptr<ParsedType>&& elementType, std::unique_ptr<Expression> size )
        : ParsedType( ParsedTypeKind::Array ), elementType( std::move( elementType ) ), size( std::move( size ) ) {}
};

struct ParsedOwnershipType : ParsedType
{
    OwnershipKind ownership;
    std::unique_ptr<ParsedType> inner;

    ParsedOwnershipType( OwnershipKind ownership, std::unique_ptr<ParsedType>&& inner )
        : ParsedType( ParsedTypeKind::Ownership ), ownership( ownership ), inner( std::move( inner ) ) {}
};

inline const std::string toString( const ParsedTypeKind& type ) 
{
    switch ( type ) 
    {
        case ParsedTypeKind::Function:   return "Function";
        case ParsedTypeKind::Ownership:  return "Ownership";
        case ParsedTypeKind::Inferred:   return "Inferred";
        case ParsedTypeKind::Array:      return "Array";
        case ParsedTypeKind::Named:      return "Named";
        default:               return "Unknown";
    }
};