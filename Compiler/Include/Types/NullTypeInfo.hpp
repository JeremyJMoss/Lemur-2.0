#pragma once

/* === Imports === */

#include "Types/TypeInfo.hpp"

/* === NullTypeInfo === */

struct NullTypeInfo: TypeInfo 
{
    NullTypeInfo() : TypeInfo( TypeKind::Null ) {}
};