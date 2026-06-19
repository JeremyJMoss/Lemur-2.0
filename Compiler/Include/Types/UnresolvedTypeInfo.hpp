#pragma once

/* === Imports === */

#include "Types/TypeInfo.hpp"

/* === UnresolvedTypeInfo === */

struct UnresolvedTypeInfo: TypeInfo 
{
    UnresolvedTypeInfo() : TypeInfo( TypeKind::Unresolved ) {}
};