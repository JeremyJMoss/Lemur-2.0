#pragma once

/* === Dependencies === */

#include <string>
#include <variant>

/* === Imports === */

#include "Core/Ids.hpp"
#include "Types/PrimitiveInfo.hpp"
#include "Types/OwnershipInfo.hpp"
#include "Types/ArrayInfo.hpp"

/* === Enum Declarations === */

enum class TypeKind : uint8_t
{
    Unknown,
    Function,
    Primitive,
    Ownership,
    Array,
    Enum,
    Class,
    Trait,
    Data,
    Null,
    Error
};

/* === Forward Declarations === */

struct FunctionInfo;

/* === Variants === */

using TypeData = std::variant<
    std::monostate,   // for Null / Inferred / Unresolved
    PrimitiveInfo,
    FunctionInfo*,
    OwnershipInfo,
    ArrayInfo
>;

/* === Utility === */

inline const std::string toString( const TypeKind kind ) 
{
    switch ( kind ) 
    {
        case TypeKind::Unknown:       return "Unknown";
        case TypeKind::Primitive:     return "Primitive";
        case TypeKind::Ownership:     return "Ownership";
        case TypeKind::Array:         return "Array";
        case TypeKind::Function:      return "Function";
        case TypeKind::Enum:          return "Enum";
        case TypeKind::Class:         return "Class";
        case TypeKind::Trait:         return "Trait";
        case TypeKind::Data:          return "Data";
        case TypeKind::Null:          return "Null";
        case TypeKind::Error:         return "Error";
        default:                      return "Unknown";
    }
};

/* === Type === */

struct Type
{
    TypeId id{};
    TypeKind kind;
    TypeData data;

    void setId( TypeId typeId ) { id = typeId; }

    Type( TypeKind kind, TypeData data ) 
        : kind( kind ),
        data( data ) {};

    virtual ~Type() = default;
};
