#pragma once

/* === Imports === */

#include "Types/Type.hpp"

/* === InferredTypeInfo === */

struct InferredType: Type 
{
    InferredType() : Type( TypeKind::Inferred, TypeOrigin::Builtin ) {}
};