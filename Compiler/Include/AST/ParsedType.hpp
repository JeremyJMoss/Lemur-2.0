#pragma once

#include <string>
#include <vector>
#include <memory>
#include "AST/ASTNode.hpp"
#include "AST/Identifier.hpp"
#include "Utils/OwnershipKind.hpp"

// Generic Type References
enum class Type
{
    Function,
    Ownership,
    Generic,
    Array,
    Inferred
};

struct ParsedType : ASTNode {
    Type kind;
    std::unique_ptr<Identifier> identifier;  // for Identifier
    OwnershipKind ownership = OwnershipKind::None; // for Ownership
    std::unique_ptr<ParsedType> inner; // ownership or array inner type
    std::vector<std::unique_ptr<ParsedType>> typeArgs; // generic parameters
    std::unique_ptr<Expression> arraySize; // for fixed-size array

    ASTNodeType type() const override { return ASTNodeType::ParsedType; }

    static std::unique_ptr<ParsedType> makeIdentifier( std::unique_ptr<Identifier>&& id ) 
    {
        auto t = std::make_unique<ParsedType>();
        t->kind = Type::Generic; // or Identifier if you add a separate kind
        t->identifier = std::move(id);
        return t;
    }

    // Function type
    static std::unique_ptr<ParsedType> makeFunction(
        std::vector<std::unique_ptr<ParsedType>>&& params, std::unique_ptr<ParsedType>&& returnType
    )
    {
        auto t = std::make_unique<ParsedType>();
        t->kind = Type::Function;
        t->typeArgs = std::move(params); // reuse typeArgs for parameters
        t->inner = std::move(returnType); // store return type in inner
        return t;
    }

    // Ownership wrapper
    static std::unique_ptr<ParsedType> makeOwnership( 
        OwnershipKind kind, std::unique_ptr<ParsedType>&& inner 
    ) 
    {
        auto t = std::make_unique<ParsedType>();
        t->kind = Type::Ownership;
        t->ownership = kind;
        t->inner = std::move(inner);
        return t;
    }

    // Array type
    static std::unique_ptr<ParsedType> makeArray( 
        std::unique_ptr<ParsedType>&& inner, std::unique_ptr<Expression>&& size 
    ) 
    {
        auto t = std::make_unique<ParsedType>();
        t->kind = Type::Array;
        t->inner = std::move(inner);
        t->arraySize = std::move(size);
        return t;
    }

    // Generic type
    static std::unique_ptr<ParsedType> makeGeneric( 
        std::unique_ptr<Identifier>&& id, 
        std::vector<std::unique_ptr<ParsedType>>&& args 
    ) 
    {
        auto t = std::make_unique<ParsedType>();
        t->kind = Type::Generic;
        t->identifier = std::move(id);
        t->typeArgs = std::move(args);
        return t;
    }

    // Inferred type
    static std::unique_ptr<ParsedType> makeInferred() 
    {
        auto t = std::make_unique<ParsedType>();
        t->kind = Type::Inferred;
        return t;
    }
};

inline const std::string toString( const Type& type ) 
{
    switch ( type ) 
    {
        case Type::Function:   return "Function";
        case Type::Ownership:  return "Ownership";
        case Type::Inferred:   return "Inferred";
        case Type::Generic:    return "Generic";
        case Type::Array:      return "Array";
        default:               return "Unknown";
    }
};