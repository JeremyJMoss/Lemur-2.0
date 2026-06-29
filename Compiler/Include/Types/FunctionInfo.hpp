#pragma once

/* === Imports === */

#include <vector>

/* === FunctionType === */

using TypeId = std::size_t;

struct FunctionInfo
{
    std::vector<const TypeId> paramTypes;
    const TypeId returnType;

    FunctionInfo( std::vector<const TypeId> params, TypeId returnType ) 
        :  paramTypes( std::move( params ) ), returnType( returnType ) {}
};