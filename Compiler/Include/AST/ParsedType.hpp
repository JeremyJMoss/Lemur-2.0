#pragma once

#include <string>
#include <vector>
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
    const ParsedTypeKind kind;

    virtual ~ParsedType() = default;
    ParsedType( const ParsedTypeKind kind ) : kind( kind ) {}

    ASTNodeType type() const override { return ASTNodeType::ParsedType; }
};

struct ParsedInferredType: ParsedType {
    ParsedInferredType() : ParsedType( ParsedTypeKind::Inferred ) {}
};

struct ParsedNamedType : ParsedType {
    const Identifier* identifier;

    ParsedNamedType( const Identifier* identifier )
        : ParsedType( ParsedTypeKind::Named ), identifier( identifier ) {}
};

struct ParsedFunctionType : ParsedType {
    std::vector<const ParsedType*> parameters;
    const ParsedType* returnType;

    ParsedFunctionType( std::vector<const ParsedType*> parameters, const ParsedType* returnType )
        : ParsedType( ParsedTypeKind::Function ), 
        parameters( std::move( parameters ) ), 
        returnType( returnType ) {}
};

struct ParsedArrayType : ParsedType {
    const ParsedType* elementType;
    const Expression* size;

    ParsedArrayType( const ParsedType* elementType, const Expression* size )
        : ParsedType( ParsedTypeKind::Array ), 
        elementType( elementType ), 
        size( size ) {}
};

struct ParsedOwnershipType : ParsedType
{
    const OwnershipKind ownership;
    const ParsedType* inner;

    ParsedOwnershipType( const OwnershipKind ownership, const ParsedType* inner )
        : ParsedType( ParsedTypeKind::Ownership ), 
        ownership( ownership ), inner( inner ) {}
};

inline std::string toString( const ParsedTypeKind& type ) 
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