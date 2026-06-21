#pragma once

/* === Imports === */

#include <string>
#include "Types/Type.hpp"

/* === PrimitiveTypeInfo === */

struct PrimitiveType : Type
{
    std::string name;

    PrimitiveType( TypeOrigin origin, std::string& name ) 
        : Type( TypeKind::Primitive, origin ), name( std::move( name ) ) {}
};