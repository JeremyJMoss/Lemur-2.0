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
    Class,
    Interface,
    Null, 
    Unresolved
};

struct TypeInfo 
{
    TypeKind kind;
    bool isBuiltIn = false;

    TypeInfo( TypeKind kind ) : kind( kind ) {};

    virtual ~TypeInfo() = default;
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
        case TypeKind::Class:         return "Class";
        case TypeKind::Interface:     return "Interface";
        case TypeKind::Null:          return "Null";
        case TypeKind::Unresolved:    return "Unresolved";
        default:                      return "Unknown";
    }
};
