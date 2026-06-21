#pragma once

/* === Imports === */

#include "Types/Type.hpp"

/* === NullTypeInfo === */

struct NullType: Type 
{
    NullType() : Type( TypeKind::Null, TypeOrigin::Builtin ) {}
};