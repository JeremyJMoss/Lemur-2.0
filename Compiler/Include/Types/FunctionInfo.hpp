#pragma once

/* === Imports === */

#include <vector>

using TypeId = std::size_t;

/* === Function Info === */

struct FunctionInfo
{
    const std::vector<TypeId> paramTypes;
    const TypeId returnType;

    FunctionInfo( std::vector<TypeId> params, TypeId returnType ) 
        :  paramTypes( std::move( params ) ), returnType( returnType ) {}
};