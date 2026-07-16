#pragma once

/* === Dependencies ===*/

#include <vector>

/* === Imports === */

#include "Core/Ids.hpp"

/* === Function Info === */

struct FunctionInfo
{
    const std::vector<TypeId> paramTypes;
    const TypeId returnType;

    FunctionInfo( std::vector<TypeId> params, TypeId returnType ) 
        :  paramTypes( std::move( params ) ), returnType( returnType ) {}
};