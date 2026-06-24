#pragma once

#include <string>
#include <variant>
#include "Types/FunctionInfo.hpp"
#include "Types/PrimitiveInfo.hpp"
#include "Types/OwnershipInfo.hpp"
#include "Types/ArrayInfo.hpp"

enum class TypeKind 
{
    Function,
    Primitive,
    Ownership,
    Inferred,
    Array,
    Null, 
    Unresolved
};

enum class TypeOrigin {
    Builtin,
    UserDefined
};

using TypeId = std::size_t;
constexpr TypeId InvalidTypeId = static_cast<std::size_t>(-1);

using TypeData = std::variant<
    std::monostate,   // for Null / Inferred / Unresolved
    PrimitiveInfo,
    FunctionInfo,
    OwnershipInfo,
    ArrayInfo
>;

struct Type
{
    TypeId m_id;
    TypeKind m_kind;
    std::size_t m_size;
    TypeOrigin m_origin;
    TypeData m_data;

    void setId( TypeId typeId ) { m_id = typeId; }

    Type( TypeKind kind, TypeOrigin origin, TypeData data ) 
        : m_id( InvalidTypeId ), 
        m_kind( kind ), 
        m_origin( origin ),
        m_data( data ) {};

    virtual ~Type() = default;

        
};

inline const std::string toString( const TypeKind kind ) 
{
    switch ( kind ) 
    {
        case TypeKind::Primitive:     return "Primitive";
        case TypeKind::Ownership:     return "Ownership";
        case TypeKind::Inferred:      return "Inferred";
        case TypeKind::Array:         return "Array";
        case TypeKind::Function:      return "Function";
        case TypeKind::Null:          return "Null";
        case TypeKind::Unresolved:    return "Unresolved";
        default:                      return "Unknown";
    }
};
