#pragma once
#include <string>
#include <memory>

enum class TypeKind 
{
    Function,
    Primitive,
    Ownership,
    Inferred,
    Custom,
    Null, 
    Unresolved
};

enum class TypeOrigin {
    Builtin,
    UserDefined
};

using typeId = std::size_t;

struct Type
{
    static inline typeId nextId = 0;
    typeId id;
    TypeKind kind;
    std::size_t size;
    TypeOrigin origin;

    Type( TypeKind kind, TypeOrigin origin ) 
        : id( nextId++ ), kind( kind ), origin( origin ) {};

    virtual ~Type() = default;
};

inline const std::string toString( const TypeKind kind ) 
{
    switch ( kind ) 
    {
        case TypeKind::Primitive:     return "Primitive";
        case TypeKind::Ownership:     return "Ownership";
        case TypeKind::Inferred:      return "Inferred";
        case TypeKind::Custom:        return "Custom";
        case TypeKind::Function:      return "Function";
        case TypeKind::Null:          return "Null";
        case TypeKind::Unresolved:    return "Unresolved";
        default:                      return "Unknown";
    }
};
