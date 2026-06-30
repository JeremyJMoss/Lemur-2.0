#pragma once

/* === Imports === */

#include <vector>

/* === FunctionType === */

using TypeId = std::size_t;

struct FunctionInfo
{
    const std::vector<TypeId> paramTypes;
    const TypeId returnType;

    FunctionInfo( std::vector<TypeId> params, TypeId returnType ) 
        :  paramTypes( std::move( params ) ), returnType( returnType ) {}
};