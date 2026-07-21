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

enum class TypeKind 
{
    Unknown,
    Function,
    Primitive,
    Ownership,
    Array,
    Null,
    Error
};

enum class TypeState 
{
    Resolved,
    Inferred,
    Unresolved
};

enum class TypeOrigin 
{
    Builtin,
    UserDefined
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
        case TypeKind::Null:          return "Null";
        case TypeKind::Error:         return "Error";
        default:                      return "Unknown";
    }
};

/* === Type === */

struct Type
{
    TypeId id;
    TypeKind kind;
    TypeState state;
    TypeOrigin origin;
    TypeData data;

    void setId( TypeId typeId ) { id = typeId; }

    Type( TypeKind kind, TypeState state, TypeOrigin origin, TypeData data ) 
        : id( InvalidTypeId ), 
        kind( kind ),
        state( state ),
        origin( origin ),
        data( data ) {};

    virtual ~Type() = default;
};
