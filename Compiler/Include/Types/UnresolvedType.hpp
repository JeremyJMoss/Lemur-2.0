#pragma once

/* === Imports === */

#include "Types/Type.hpp"

/* === UnresolvedTypeInfo === */

struct UnresolvedType: Type
{
    UnresolvedType() : Type( TypeKind::Unresolved, TypeOrigin::Builtin ) {}
};