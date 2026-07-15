#pragma once

/* === Dependencies ===*/

#include <string>
#include <vector>

/* === Imports === */

#include "AST/ASTNode.hpp"
#include "Utils/OwnershipKind.hpp"

/* === Forward Declarations === */

struct Identifier;

/* === Enum Declaration === */

enum class ParsedTypeKind : std::uint8_t
{
    Named,
    Function,
    Ownership,
    Array,
    Inferred
};

/* === Parsed Type === */

struct ParsedType : ASTNode {
    const ParsedTypeKind kind;

    virtual ~ParsedType() = default;
    ParsedType( const ParsedTypeKind kind ) 
        : kind( kind ) {}

    void accept( ASTVisitor& v ) const override 
    { 
        v.visit( *this );
    }
};

struct ParsedInferredType: ParsedType {
    ParsedInferredType() : ParsedType( ParsedTypeKind::Inferred ) {}
};

struct ParsedNamedType : ParsedType {
    const Identifier* identifier;

    ParsedNamedType( const Identifier* identifier )
        : ParsedType( ParsedTypeKind::Named ), 
          identifier( identifier ) {}
};

struct ParsedFunctionType : ParsedType {
    const std::vector<ParsedType*> parameters;
    const ParsedType* returnType;

    ParsedFunctionType( 
        std::vector<ParsedType*> parameters, 
        const ParsedType* returnType 
    ) : ParsedType( ParsedTypeKind::Function ), 
        parameters( std::move( parameters ) ), 
        returnType( returnType ) {}
};

struct ParsedArrayType : ParsedType {
    const ParsedType* elementType;
    const Expression* size;

    ParsedArrayType( 
        const ParsedType* elementType, 
        const Expression* size 
    ) : ParsedType( ParsedTypeKind::Array ), 
        elementType( elementType ), 
        size( size ) {}
};

struct ParsedOwnershipType : ParsedType
{
    const OwnershipKind ownership;
    const ParsedType* inner;

    ParsedOwnershipType( 
        const OwnershipKind ownership, 
        const ParsedType* inner 
    ) : ParsedType( ParsedTypeKind::Ownership ), 
        ownership( ownership ), 
        inner( inner ) {}
};