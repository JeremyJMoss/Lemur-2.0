#pragma once

/* === Imports === */

#include "Types/TypeInfo.hpp"

/* === InferredTypeInfo === */

struct InferredTypeInfo: TypeInfo 
{
    InferredTypeInfo() : TypeInfo( TypeKind::Inferred ) {}
};